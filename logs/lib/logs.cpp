#include <unistd.h>

#include <atomic>
#include <cassert>
#include <mutex>
#include <set>
#include <upp/enum_array.hpp>
#include <upp/logs.hpp>
#include <upp/nop_mutex.hpp>
#include <upp/terminal.hpp>
#include <utility>

#include "contextutils.hpp"

namespace upp::logs {

#ifdef UPP_THREAD_UNSAFE
using log_mutex = upp::npo_nop_mutex;
#else
using log_mutex = std::mutex;
#endif

namespace {
// NOLINTBEGIN
constinit std::atomic<const consumer*> g_consumer{};
/*
 * Mutex for protecting the global logging state
 * */
constinit log_mutex g_mut{};
/*
 * Flag for checking if the contexts are organized or in a potentially random
 * order.
 *
 * Organized means that all node children are placed immediately after the node.
 * Note that this means that there might be granchildren between the children.
 * */
constinit bool g_organized{false};
// NOLINTEND
static constexpr auto relaxed = std::memory_order::relaxed;

bool tty_output() noexcept { return isatty(fileno(stderr)); }

constexpr auto prefixes = upp::make_enum_array<strlit, level>({
    {level::trace, "[T] "},
    {level::debug, "[D] "},
    {level::info, "[I] "},
    {level::note, "[N] "},
    {level::warn, "[W] "},
    {level::error, "[E] "},
    {level::fatal, "[F] "},
});

std::string color_on(level lvl) noexcept {
    using enum level;
    using enum terminal::option;
    switch (lvl) {
        case level::unset:
        case level::trace:
        case level::debug:
        case level::info: return {};
        case level::note: return terminal::dynamic_escape_code(bold);
        case level::warn: return terminal::dynamic_escape_code(yellow);
        case level::error: return terminal::dynamic_escape_code(red);
        case level::fatal: return terminal::dynamic_escape_code(red, bold);
    }
    std::unreachable();
}

std::string color_off(level lvl) noexcept {
    using enum level;
    using enum terminal::option;
    switch (lvl) {
        case level::unset:
        case level::trace:
        case level::debug:
        case level::info: return {};
        case level::note:
        case level::warn:
        case level::error:
        case level::fatal: return terminal::dynamic_escape_code(clear);
    }
    std::unreachable();
}

// Find the next sibling. The data has to be organized before calling this
const context_base* find_next_sibling(const context_base* in) noexcept {
    auto* parent = in->parent();
    for (const auto* i = in->next(); i != nullptr; i = i->next()) {
        if (i->parent() == parent) return i;
    }
    return nullptr;
}

void organize() noexcept {
    assert(!g_organized);
    organize(&root_context());
    g_organized = true;
}

void ensure_organized() {
    if (g_organized) return;
    organize();
    assert(is_valid_tree(&root_context()));
}

const context_base* sync_inherited_log_levels(const context_base* branch,
                                              level lvl) noexcept {
    if (branch->explicit_level() != level::unset) {
        // Child level has been explicitly set -> jump to next sibling
        return find_next_sibling(branch);
    }
    branch->set_inherited_level({}, lvl);
    auto* i = branch->next();
    if (!i) return nullptr;
    if (i->parent() == branch->parent())
        return sync_inherited_log_levels(i, lvl);
    while (i && i->parent() == branch) {
        i = sync_inherited_log_levels(i, lvl);
    }
    return i;
}

}  // namespace

namespace detail {

void push_log(const context_base& ctx, level lvl, std::string msg) {
    auto* c = g_consumer.load(relaxed);
    if (c) (*c)(ctx, lvl, std::move(msg));
}

}  // namespace detail

context_base::context_base(std::string_view name) noexcept
    : context_base(root_context(), name) {}

context_base::context_base(const context_base& parent,
                           std::string_view name) noexcept
    : m_parent(&parent), m_name(name) {
    auto lk = std::unique_lock(g_mut);
    assert(this != &root_context());
    if (g_organized) {
        m_next = m_parent->m_next;
        m_parent->m_next = this;
        if (m_next) m_next->m_prev = this;
        m_prev = m_parent;
        m_inherited_level = m_parent->effective_level();
    } else {
        assert(is_valid_tree(&root_context()));
        m_next = root_context().m_next;
        m_prev = &root_context();
        root_context().m_next = this;
        if (m_next) m_next->m_prev = this;
        assert(is_valid_tree(&root_context()));
    }
}

constexpr context_base::context_base(detail::context_passkey) noexcept
    : m_parent(nullptr), m_name() {}

context_base::~context_base() {
    auto lk = std::unique_lock(g_mut);
    if (!m_prev) {
        assert(this == &root_context());
        return;
    }
#ifndef NDEBUG
    // Clear the parent pointer to assert elsewhere that the child loggers are
    // not used after the parent is destroyed.
    //
    // Normally the parent should be always destroyed after its children but
    // statically created loggers are constructed in arbitrary order.
    for (auto* p = &root_context(); p != nullptr; p = p->next()) {
        if (p->parent() == this) p->m_parent = nullptr;
    }
#endif
    m_prev->m_next = m_next;
    if (m_next) m_next->m_prev = m_prev;
}

level context_base::explicit_level() const noexcept {
    return m_lvl.load(relaxed);
}

level context_base::effective_level() const noexcept {
    const auto lvl = m_lvl.load(std::memory_order::relaxed);
    if (lvl != level::unset) return lvl;
    return m_inherited_level.load(std::memory_order::relaxed);
}

void context_base::set_level(level new_lvl) const noexcept {
    auto lk = std::unique_lock(g_mut);
    m_lvl.store(new_lvl, relaxed);
    ensure_organized();
    auto lvl = m_lvl.load(relaxed);
    lvl = lvl != level::unset ? lvl : m_inherited_level.load(relaxed);
    for (const auto* i = m_next; i && i->parent() == this;
         i = find_next_sibling(i)) {
        sync_inherited_log_levels(i, lvl);
    }
}

void context_base::set_next(detail::context_passkey,
                            const context_base* nxt) const noexcept {
    m_next = nxt;
}

void context_base::set_prev(detail::context_passkey,
                            const context_base* prv) const noexcept {
    m_prev = prv;
}
void context_base::set_inherited_level(detail::context_passkey,
                                       level lvl) const noexcept {
    m_inherited_level.store(lvl, relaxed);
}

namespace {
class root_ctx final : public context_base {
 public:
    constexpr root_ctx() noexcept : context_base(detail::context_passkey{}) {}
};
constinit const root_ctx g_root_ctx{};

std::size_t ctx_buf_size(const context_base* ctx, std::size_t counter = 0) {
    if (ctx == &root_context()) return 0;
    if (ctx->parent() == &root_context()) return counter + ctx->name().size();
    return ctx_buf_size(ctx->parent(), counter + ctx->name().size() + 1);
}

void context_path_recurse(const context_base* ctx, std::string& buf) {
    if (ctx->parent() != &root_context()) {
        context_path_recurse(ctx->parent(), buf);
        buf.push_back('.');
        buf += ctx->name();
    } else {
        buf = ctx->name();
    }
}

std::string context_path(const context_base& ctx) {
    std::string buf{};
    buf.reserve(ctx_buf_size(&ctx));
    context_path_recurse(&ctx, buf);
    return buf;
}

}  // namespace
const context_base& root_context() { return g_root_ctx; }

class default_consumer_impl final : public consumer {
 public:
    void operator()(const context_base& ctx, level lvl,
                    std::string_view msg) const override {
        assert((&ctx == &root_context() || ctx.parent() != nullptr) &&
               "Trying to log using context that's parent has been destroyed");
        const bool colored = tty_output();
        auto buf = colored ? color_on(lvl) : std::string();
        buf.append_range(prefixes[lvl]);
        if (!ctx.name().empty()) {
            buf.push_back('[');
            buf.append_range(context_path(ctx));
            buf.push_back(']');
            buf.push_back(' ');
        }
        buf.append_range(msg);
        if (colored) buf.append_range(color_off(lvl));
        buf.push_back('\n');
        std::fputs(buf.c_str(), stderr);
    }
};

const consumer& default_consumer() {
    static const default_consumer_impl impl{};
    return impl;
}
void set_consumer(const consumer& c) {
    g_consumer.store(&c, std::memory_order::release);
}

}  // namespace upp::logs

#include <unistd.h>

#include <atomic>
#include <cassert>
#include <print>
#include <upp/enum_array.hpp>
#include <upp/logs.hpp>
#include <upp/terminal.hpp>
#include <utility>

namespace upp::logs {

std::atomic_bool g_initialized{false};
void (*g_callback)(const context&, level lvl, std::string) = nullptr;

static constexpr auto relaxed = std::memory_order::relaxed;

namespace {

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

// TODO: Better implementation
void default_callback(const context& ctx, level lvl, std::string msg) {
    const bool colored = tty_output();
    auto buf = colored ? color_on(lvl) : std::string();
    buf.append_range(prefixes[lvl]);
    if (!ctx.name().empty()) {
        buf.push_back('[');
        buf.append_range(ctx.name());
        buf.push_back(']');
        buf.push_back(' ');
    }
    buf.append_range(msg);
    if (colored) buf.append_range(color_off(lvl));
    buf.push_back('\n');
    std::fputs(buf.c_str(), stderr);
}
}  // namespace

namespace detail {

const context*& init_list::head() noexcept {
    static auto self = init_list();
    return self.m_head;
}

void init_impl() noexcept {
    const auto* head = init_list::head();
    while (head) {
        auto* next = head->m_next_sibling;
        auto* parent = head->m_parent;
        if (parent) {
            head->m_next_sibling = std::exchange(parent->m_children, head);
        }
        head = next;
    }
}
void push_log(const context& ctx, level lvl, std::string msg) {
    g_callback(ctx, lvl, std::move(msg));
}

}  // namespace detail

const context root_context{nullptr, {.name = {}}};

void context::sync_inherited_log_levels() const noexcept {
    auto lvl = m_lvl.load(relaxed);
    auto inherited = m_inherited_level.load(relaxed);
    if (lvl == level::unset) {
        // Log level has not been explicitly set -> update children's inherited
        // log level to same as self
        auto* child = m_children;
        while (child) {
            child->m_inherited_level.store(inherited, relaxed);
            if (child->m_lvl.load(relaxed) == level::unset) {
                // Child's log level has not been set explicitly -> recurse into
                // child so the same log level is set to all offspring
                child->sync_inherited_log_levels();
            } else {
                // Child's log level has been explicitly set so its chilren have
                // the log levels already set correctly, i.e. nothing to do.
            }
            child = child->m_next_sibling;
        }
    } else {
        // Level has been explicitly set
        auto* child = m_children;
        while (child) {
            child->m_inherited_level.store(lvl, relaxed);
            if (child->m_lvl.load(relaxed) == level::unset) {
                child->sync_inherited_log_levels();
            } else {
            }
            child = child->m_next_sibling;
        }
    }
}

context::context(opts os) noexcept : m_parent(&root_context), m_name(os.name) {
    assert(!g_initialized.load(std::memory_order::relaxed));
    auto*& hd = detail::init_list::head();
    this->m_next_sibling = hd;
    hd = this;
}

context::context(const context* parent, opts os) noexcept
    : m_parent(parent), m_name(os.name) {
    assert(!g_initialized.load(std::memory_order::relaxed));
    auto*& hd = detail::init_list::head();
    this->m_next_sibling = hd;
    hd = this;
}

level context::effective_level() const noexcept {
    const auto lvl = m_lvl.load(std::memory_order::relaxed);
    if (lvl != level::unset) return lvl;
    return m_inherited_level.load(std::memory_order::relaxed);
}

void context::set_level(level new_lvl) const noexcept {
    m_lvl.store(new_lvl, relaxed);
    sync_inherited_log_levels();
}
void init_t::operator()(opts opt) const noexcept {
    auto was_initialized = g_initialized.exchange(true);
    assert(!was_initialized && "logging already initialized");
    g_callback = opt.callback ? opt.callback : default_callback;
    detail::init_impl();
    if (opt.level != level::unset) root_context.set_level(opt.level);
}

}  // namespace upp::logs

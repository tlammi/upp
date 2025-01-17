#pragma once

#include <atomic>
#include <upp/immovable.hpp>
#include <upp/strlit.hpp>
#include <upp/uncopyable.hpp>

namespace upp::logs {

enum class level : std::uint8_t {
    trace,
    debug,
    info,
    note,
    warn,
    error,
    fatal,
    unset,
};

class context;
namespace detail {

class init_list : public uncopyable, public immovable {
    const context* m_head{};

 public:
    static const context*& head() noexcept;

 private:
    constexpr init_list() noexcept = default;
};

// Utility for simple friend declarations
void init_impl() noexcept;

void push_log(const context& ctx, level lvl, std::string msg);

}  // namespace detail
extern const context root_context;

class context : public uncopyable, public immovable {
    friend void detail::init_impl() noexcept;

    mutable const context* m_parent;
    mutable const context* m_next_sibling{};
    mutable const context* m_children{};

    mutable std::atomic<level> m_lvl{level::unset};
    mutable std::atomic<level> m_inherited_level{level::unset};

    strlit m_name;

    struct opts {
        strlit name;
    };

    void sync_inherited_log_levels() const noexcept;

 public:
    explicit context(opts os) noexcept;
    explicit context(const context* parent, opts os) noexcept;

    level effective_level() const noexcept;

    void set_level(level new_lvl) const noexcept;

    strlit name() const noexcept { return m_name; }

    const context* children() const noexcept { return m_children; }
    const context* next_sibling() const noexcept { return m_next_sibling; }
};

struct init_t : uncopyable, immovable {
    struct opts {
        logs::level level = logs::level::unset;
        void (*callback)(const context&, logs::level lvl,
                         std::string msg) = nullptr;
    };
    void operator()(opts opt) const noexcept;
    void operator()() const noexcept { (*this)({.level = level::info}); }

    consteval init_t() noexcept = default;
};

constexpr init_t init{};

// NOLINTNEXTLINE(*-macro-usage)
#define UPP_LOG(lvl, ctx, ...)                                           \
    do {                                                                 \
        if consteval {                                                   \
        } else {                                                         \
            if (ctx.effective_level() <= lvl)                            \
                ::upp::logs::detail::push_log(ctx, lvl,                  \
                                              std::format(__VA_ARGS__)); \
        }                                                                \
    } while (false)

// NOLINTBEGIN(*-macro-usage)
#define UPP_TRACE(ctx, ...) UPP_LOG(::upp::logs::level::trace, ctx, __VA_ARGS__)
#define UPP_DEBUG(ctx, ...) UPP_LOG(::upp::logs::level::debug, ctx, __VA_ARGS__)
#define UPP_INFO(ctx, ...)  UPP_LOG(::upp::logs::level::info, ctx, __VA_ARGS__)
#define UPP_NOTE(ctx, ...)  UPP_LOG(::upp::logs::level::note, ctx, __VA_ARGS__)
#define UPP_WARN(ctx, ...)  UPP_LOG(::upp::logs::level::warn, ctx, __VA_ARGS__)
#define UPP_ERR(ctx, ...)   UPP_LOG(::upp::logs::level::error, ctx, __VA_ARGS__)
#define UPP_FATAL(ctx, ...) UPP_LOG(::upp::logs::level::fatal, ctx, __VA_ARGS__)
// NOLINTEND(*-macro-usage)

}  // namespace upp::logs

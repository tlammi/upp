/**
 * \file
 * \ingroup upp_logs
 * */

#pragma once

#include <atomic>
#include <format>
#include <upp/concepts.hpp>
#include <upp/immovable.hpp>
#include <upp/strlit.hpp>
#include <upp/uncopyable.hpp>

namespace upp::logs {

/**
 * \brief Log levels
 * */
enum class level : std::uint8_t {
    trace,
    debug,
    info,
    note,
    warn,
    error,
    fatal,
    unset,  ///< Used to indicate unconfigured log levels
};

class context_base;
namespace detail {

void push_log(const context_base& ctx, level lvl, std::string msg);

struct context_passkey;

}  // namespace detail

/**
 * \brief Hierarchical logging context
 *
 * Contexts share a global context: all of the contexts are accessible via
 * root_context(). Setting a log level for a logger also applies to its
 * descendants unles those have their log levels explicitly set. Setting log
 * level to unset causes the inherited log level to take effect.
 *
 * The contexts should generally be destroyed children first. This is not
 * mandatory as static storage duration contexts are constructed and destroyed
 * in an arbitrary order. The library has asserts in place that forbid logging
 * using children whose parents have already been destroyed.
 * */
class context_base : public uncopyable, public immovable {
    mutable const context_base* m_parent;
    mutable const context_base* m_next{};
    mutable const context_base* m_prev{};

    mutable std::atomic<level> m_lvl{level::unset};
    mutable std::atomic<level> m_inherited_level{level::unset};

    std::string_view m_name;

 public:
    explicit context_base(std::string_view name) noexcept;
    explicit context_base(const context_base& parent,
                          std::string_view name) noexcept;

    // Internal (construct a root logger)
    constexpr explicit context_base(detail::context_passkey) noexcept;

    /**
     * \brief Explicit level
     *
     * Access the log level set for this logger explicitly. This ignores the log
     * level inherited from the parent.
     * */
    level explicit_level() const noexcept;

    /**
     * \brief Effective level
     *
     * Access the effective log level. This is either explicitly set for the
     * logger using \a set_level() or inherited from a parent that has it's
     * level set by \a set_level().
     * */
    level effective_level() const noexcept;

    /**
     * \brief Set log level
     * */
    void set_level(level new_lvl) const noexcept;

    std::string_view name() const noexcept { return m_name; }

    const context_base* next() const noexcept { return m_next; }
    const context_base* prev() const noexcept { return m_prev; }
    const context_base* parent() const noexcept { return m_parent; }

    void set_next(detail::context_passkey,
                  const context_base* nxt) const noexcept;
    void set_prev(detail::context_passkey,
                  const context_base* prv) const noexcept;

    void set_inherited_level(detail::context_passkey, level lvl) const noexcept;

 protected:
    ~context_base();
};

namespace detail {

template <class T>
struct context_store {
    T value{};
};
}  // namespace detail

template <class T = upp::strlit>
class context : private detail::context_store<T>, public context_base {
    using store = detail::context_store<T>;

 public:
    explicit context(std::type_identity_t<T> name) noexcept
        : store(std::move(name)),
          context_base(static_cast<std::string_view>(store::value)) {}

    context(const context_base& parent, std::type_identity_t<T> name) noexcept
        : store(std::move(name)),
          context_base(parent, static_cast<std::string_view>(store::value)) {}
};

template <>
class context<std::string_view> : public context_base {
 public:
    using context_base::context_base;
};

template <>
class context<upp::strlit> : public context_base {
 public:
    explicit context(upp::strlit name) noexcept
        : context_base(static_cast<std::string_view>(name)) {}

    context(const context_base& parent, upp::strlit name) noexcept
        : context_base(parent, static_cast<std::string_view>(name)) {}
};

class consumer : public immovable {
 public:
    virtual void operator()(const context_base& ctx, level lvl,
                            std::string_view msg) const = 0;

 protected:
    constexpr ~consumer() = default;
};

/**
 * \brief Root context
 *
 * This is the context that is a (grand)parent for all of the other contexts.
 * */
const context_base& root_context();

const auto upp_context = context(root_context(), "upp");

/**
 * \brief Default log consumer
 * */
const consumer& default_consumer();

/**
 * \brief Set log consumer
 * */
void set_consumer(const consumer& c);

/**
 * \brief Utility for setting up the logging
 * */
inline void quick_init(level lvl) {
    set_consumer(default_consumer());
    root_context().set_level(lvl);
}

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

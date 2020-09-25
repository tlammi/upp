#pragma once
#include <type_traits>
namespace upp {
namespace util {

/**
 * \brief Utility class for "deferred execution". Borrowed from Golang
 *
 * The object takes a callable in it's constructor and calls it in its
 * destructor.
 *
 * \tparam Callable Callable type
 * \tparam Enable Limit allowed callables to noexcept callables
 */
template <typename Callable,
		  typename Enable =
			  std::enable_if_t<std::is_nothrow_invocable_v<Callable>>>
class Defer {
public:

		Defer(Callable&& c) : c_{c} {}

		Defer(const Defer&) = delete;
		Defer(Defer&&) = default;

		Defer& operator=(const Defer&) = delete;
		Defer& operator=(Defer&&) = default;

		~Defer() { c_(); }

private:
		Callable c_;
};
}  // namespace util
}  // namespace upp
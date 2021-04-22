#pragma once

#include <type_traits>

namespace upp{
namespace parser{
namespace detail{

/**
 * Utility for hiding callbacks
 *
 * Wraps any callable, or a nullptr. In the latter case, operator() does nothing
 */
template<class T=std::nullptr_t>
class CbHolder{
public:
	template<class T2>
	CbHolder(T2&& t): t_{std::forward<T2>(t)}{}
	CbHolder(){}

	constexpr operator bool() const noexcept {
		if constexpr (!std::is_same_v<T, std::nullptr_t>) {
			return true;
		}
		else{
			return false;
		}
	}

	template<class... Args>
	auto operator()(Args&&... args) const {
		if constexpr (std::is_same_v<T, std::nullptr_t>){
		} else if constexpr (std::is_same_v<std::invoke_result_t<T, Args...>, void>){
			t_(std::forward<Args>(args)...);
		} else {
			return t_(std::forward<Args>(args)...);
		}
	}
private:

	T t_{nullptr};
};
}
}
}

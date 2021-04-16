#pragma once

#include <type_traits>

namespace upp{
namespace parser{
namespace detail{

template<class T=std::nullptr_t>
class CbHolder{
public:
	template<class T2>
	CbHolder(T2&& t): t_{std::forward<T2>(t)}{}
	CbHolder(){}

	operator bool() const noexcept {

		if constexpr (std::is_convertible_v<T, bool>) {
			return t_;
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

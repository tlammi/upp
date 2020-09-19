#pragma once
#include <memory>

#include "upp/detail/cli/value.hpp"
namespace upp {
namespace detail {
namespace cli {
class Cmd;
class PosArgs {
		friend class Cmd;

public:
		template <typename T>
		void store_in(T& value) {
				value_ = std::make_unique<Value<T>>(value);
		}

		const ValueBase* value() const { return value_.get(); }

private:
		std::unique_ptr<ValueBase> value_{nullptr};
};
}  // namespace cli
}  // namespace detail
}  // namespace upp
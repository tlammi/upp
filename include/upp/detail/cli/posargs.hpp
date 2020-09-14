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

		std::vector<std::string_view> value_restrictions() const {
				if (value_) return value_->value_restrictions();
				return {};
		}

		bool support_multiple_values() const {
				if (value_) return value_->support_multiple_values();
				return false;
		}

private:
		std::unique_ptr<ValueBase> value_{nullptr};
};
}  // namespace cli
}  // namespace detail
}  // namespace upp
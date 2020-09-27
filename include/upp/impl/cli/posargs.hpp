/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once
#include <memory>

#include "upp/impl/cli/value.hpp"
namespace upp {
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
}  // namespace upp
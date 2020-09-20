#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string_view>

#include "upp/impl/cli/value.hpp"

namespace upp {
namespace cli {
class Cmd;

class Opt {
		friend class Cmd;

public:
		Opt();
		explicit operator bool() const;

		template <typename T>
		Opt& store_in(T& target) {
				value_ = std::make_unique<Value<T>>(target);
				return *this;
		}

		template <typename Func>
		Opt& callback(Func&& func) {
				cb_ = func;
				return *this;
		}

		Opt& set_help(std::string_view help_str);

		const ValueBase* value() const;

		std::string_view help() const;

private:
		bool parsed_{false};
		std::unique_ptr<ValueBase> value_{nullptr};
		std::string help_{};
		std::function<void()> cb_{nullptr};
};

class Opts {
public:
		struct OptMeta {
				char short_flag;
				std::string long_flag;
		};

		struct OptMetaCmp {
				bool operator()(const OptMeta& lhs, const OptMeta& rhs) const {
						return lhs.long_flag < rhs.long_flag;
				}
		};

		using Map = std::map<OptMeta, Opt, OptMetaCmp>;

		Opt& create(char sflag, const std::string& lflag);

		Opt& create(const std::string& lflag);
		Opt& create(char sflag);
		bool exists(char c);
		bool exists(const std::string& str);
		Opt& operator[](char c);

		Opt& operator[](const std::string& str);

		Map::const_iterator begin() const;
		Map::const_iterator end() const;

		size_t size() const;

private:
		bool key_exists(char sflag, std::string_view lflag) const;
		Map opts_{};
};

}  // namespace cli
}  // namespace upp
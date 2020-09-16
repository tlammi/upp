#pragma once

#include <algorithm>
#include <initializer_list>
#include <set>
#include <string_view>

#include "upp/detail/cli/exception.hpp"
namespace upp {
namespace detail {
namespace cli {

template <typename T>
struct EnumEntry {
		T val;
		std::string_view str;
		std::string_view help{""};
};

template <typename T>
struct EnumEntryCmp {
		bool operator()(const EnumEntry<T>& lhs,
						const EnumEntry<T>& rhs) const {
				return lhs.str < rhs.str;
		}
};

template <typename T>
class Enum {
public:
		using Set = std::set<EnumEntry<T>, EnumEntryCmp<T>>;
		Enum(std::initializer_list<EnumEntry<T>> args) {
				for (const auto& e : args) {
						if (set_.count(e))
								throw ParsingError(
									"Enum string already exists");
						set_.insert(e);
				}
		}

		bool operator==(T rhs) const { return value_ == rhs; }
		bool operator!=(T rhs) const { return value_ != rhs; }

		Enum& operator=(std::string_view str) {
				auto iter = std::find_if(
					set_.begin(), set_.end(),
					[&](const auto& elem) { return elem.str == str; });
				if (iter == set_.end())
						throw ParsingError("Invalid enumeration");
				value_ = *iter;
				return *this;
		}

		Enum& operator=(T rhs) { value_ = rhs; }

		std::string_view str() const {
				auto iter = std::find_if(
					set_.begin(), set_.end(),
					[&](const auto& e) { return value_.val == e.val; });
				if (iter != set_.end()) { return iter->first; }
				return "";
		}

		operator T() const { return value_.val; }

		template <typename Iter>
		class ConstEnumIterator {
		public:
				explicit ConstEnumIterator(Iter iter) : iter_{iter} {}

				bool operator==(const ConstEnumIterator& rhs) const {
						return iter_ == rhs.iter_;
				}

				bool operator!=(const ConstEnumIterator& rhs) const {
						return !(*this == rhs);
				}

				ConstEnumIterator& operator++() {
						++iter_;
						return *this;
				}

				const auto& operator*() const { return *iter_; }
				const auto* operator-> () const { return &*iter_; }

		private:
				Iter iter_;
		};

		ConstEnumIterator<typename Set::const_iterator> begin() const {
				return ConstEnumIterator(set_.begin());
		}

		ConstEnumIterator<typename Set::const_iterator> end() const {
				return ConstEnumIterator(set_.end());
		}

private:
		Set set_{};
		EnumEntry<T> value_{};
};
}  // namespace cli
}  // namespace detail
}  // namespace upp
/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#include "upp/impl/cli/opt.hpp"

namespace upp {
namespace cli {

Opt::Opt() {}
Opt::operator bool() const { return parsed_; }

Opt& Opt::set_help(std::string_view help_str) {
		help_ = help_str;
		return *this;
}

const ValueBase* Opt::value() const { return value_.get(); }

std::string_view Opt::help() const { return help_; }

Opt& Opts::create(char sflag, const std::string& lflag) {
		if (key_exists(sflag, lflag))
				throw std::invalid_argument("Option exists");
		return opts_[{sflag, lflag}] = {};
}

Opt& Opts::create(const std::string& lflag) {
		if (key_exists('\0', lflag))
				throw std::invalid_argument("Option exists");
		return opts_[{'\0', lflag}] = {};
}
Opt& Opts::create(char sflag) {
		if (key_exists(sflag, "")) throw std::invalid_argument("Option exists");
		return opts_[{sflag, ""}] = {};
}
bool Opts::exists(char c) { return key_exists(c, ""); }
bool Opts::exists(const std::string& str) { return key_exists('\0', str); }
Opt& Opts::operator[](char c) {
		auto iter = std::find_if(
			opts_.begin(), opts_.end(),
			[&](const auto& pair) { return pair.first.short_flag == c; });
		if (iter != opts_.end()) return iter->second;
		throw ParsingError("Option does not exist");
}

Opt& Opts::operator[](const std::string& str) {
		auto iter = std::find_if(
			opts_.begin(), opts_.end(),
			[&](const auto& pair) { return pair.first.long_flag == str; });
		if (iter != opts_.end()) return iter->second;
		throw ParsingError("Option does not exist");
}

Opts::Map::const_iterator Opts::begin() const { return opts_.begin(); }
Opts::Map::const_iterator Opts::end() const { return opts_.end(); }

size_t Opts::size() const { return opts_.size(); }

bool Opts::key_exists(char sflag, std::string_view lflag) const {
		return std::find_if(opts_.begin(), opts_.end(), [&](const auto& pair) {
					   return (sflag != '\0' &&
							   pair.first.short_flag == sflag) ||
							  (lflag != "" && pair.first.long_flag == lflag);
			   }) != opts_.end();
}

}  // namespace cli
}  // namespace upp
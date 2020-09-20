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

}  // namespace cli
}  // namespace upp
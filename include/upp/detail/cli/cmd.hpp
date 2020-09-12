#pragma once

#include "upp/detail/cli/opt.hpp"
namespace upp {
namespace detail {
namespace cli {

class Cmd {
public:
		explicit Cmd(const char* name = nullptr) : name_{name} {}
		Opts& opts() { return opts_; }

private:
		const char* name_;
		Opts opts_{};
};
}  // namespace cli
}  // namespace detail
}  // namespace upp
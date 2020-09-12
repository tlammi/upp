#pragma once

#include "upp/detail/cli/cmd.hpp"
#include "upp/detail/cli/convert.hpp"
#include "upp/detail/cli/enum.hpp"
#include "upp/detail/cli/opt.hpp"
#include "upp/detail/cli/parse.hpp"
#include "upp/detail/cli/value.hpp"

namespace upp {
namespace cli {
using Cmd = ::upp::detail::cli::Cmd;
template <typename T>
using Enum = ::upp::detail::cli::Enum<T>;

template <typename Iter>
Iter parse(Cmd& cmd, Iter begin, Iter end) {
		return ::upp::detail::cli::parse(cmd, std::forward<Iter>(begin),
										 std::forward<Iter>(end));
}

}  // namespace cli
}  // namespace upp
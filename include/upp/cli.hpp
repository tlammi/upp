#pragma once

#include "upp/detail/cli/cmd.hpp"
#include "upp/detail/cli/convert.hpp"
#include "upp/detail/cli/enum.hpp"
#include "upp/detail/cli/opt.hpp"
#include "upp/detail/cli/value.hpp"

namespace upp {
namespace cli {
using Cmd = ::upp::detail::cli::Cmd;
template <typename T>
using Enum = ::upp::detail::cli::Enum<T>;

}  // namespace cli
}  // namespace upp
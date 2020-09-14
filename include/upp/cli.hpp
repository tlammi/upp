#pragma once

#include "upp/detail/cli/cmd.hpp"
#include "upp/detail/cli/convert.hpp"
#include "upp/detail/cli/enum.hpp"
#include "upp/detail/cli/exception.hpp"
#include "upp/detail/cli/helpgen.hpp"
#include "upp/detail/cli/opt.hpp"
#include "upp/detail/cli/value.hpp"

namespace upp {
namespace cli {
using Cmd = ::upp::detail::cli::Cmd;
template <typename T>
using Enum = ::upp::detail::cli::Enum<T>;

using Exception = ::upp::detail::cli::Exception;
using ParsingError = ::upp::detail::cli::ParsingError;
using HelpException = ::upp::detail::cli::HelpException;

std::string helpgen(const Cmd& cmd) { return helpgenimpl(cmd); }

}  // namespace cli
}  // namespace upp
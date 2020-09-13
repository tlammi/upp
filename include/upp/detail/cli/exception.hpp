#pragma once
#include <stdexcept>
namespace upp {
namespace detail {
namespace cli {

class Exception : public std::runtime_error {
public:
		using std::runtime_error::runtime_error;
};
class ParsingError : public Exception {
public:
		using Exception::Exception;
};

class HelpException : public Exception {
public:
		using Exception::Exception;
};
}  // namespace cli
}  // namespace detail
}  // namespace upp
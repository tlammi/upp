#include "upp/cli/parser.hpp"

namespace upp {
namespace cli {

Parser::Parser() : _boolopts{}, _opts{} {}

OptContainer<bool>& Parser::boolopts() { return _boolopts; }
OptContainer<Value>& Parser::opts() { return _opts; }
OptContainer<VectValue>& Parser::vectopts() { return _vectopts; }

}  // namespace cli
}  // namespace upp
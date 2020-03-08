#pragma once
#include <iostream>
#include <optional>

#include "upp/cli/optcontainer.hpp"
#include "upp/cli/value.hpp"
#include "upp/cli/vectvalue.hpp"

namespace upp {
namespace cli {

class Parser {
 public:
    Parser();

    OptContainer<bool>& boolopts();
    OptContainer<Value>& opts();
    OptContainer<VectValue>& vectopts();

 private:
    OptContainer<bool> _boolopts;
    OptContainer<Value> _opts;
    OptContainer<VectValue> _vectopts;
};
}  // namespace cli
}  // namespace upp
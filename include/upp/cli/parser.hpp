#pragma once
#include <iostream>
#include <optional>

#include "upp/cli/optcontainer.hpp"
#include "upp/cli/value.hpp"
#include "upp/cli/vectvalue.hpp"

namespace upp {
namespace cli {

template <typename T>
class Parser {
 public:
    typedef int (*callback_t)(T*);

    Parser(callback_t callback = nullptr, T* parg = nullptr)
        : _cback{callback}, _parg{parg}, _boolopts{}, _opts{}, _vectopts{} {}

    OptContainer<bool>& boolopts() { return _boolopts; }
    OptContainer<Value>& opts() { return _opts; }
    OptContainer<VectValue>& vectopts() { return _vectopts; }

    int parse(int argc, char** argv) {
        for (size_t i = 1; i < argc; ++i) {}
        return 0;
    }

 private:
    void _reset_and_check_containers() {}

    callback_t _cback;
    T* _parg;
    OptContainer<bool> _boolopts;
    OptContainer<Value> _opts;
    OptContainer<VectValue> _vectopts;
};
}  // namespace cli
}  // namespace upp
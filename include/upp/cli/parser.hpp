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

    int parse(int argc, const char** argv) {
        std::vector<std::string> args{argv + 1, argv + argc};
        return parse(args);
    }

    int parse(const std::vector<std::string>& args) {
        _boolopts.reset_values();
        _opts.reset_values();
        _vectopts.reset_values();
        size_t i{0};
        while (i < args.size()) {
            std::string arg = args[i];
            std::cout << "parsing flag: " << arg << std::endl;
            if (auto sflag = _as_short_flag(arg)) {
                i = _handle_sflags(*sflag, i, args);
            } else if (auto lflag = _as_long_flag(arg)) {
                i = _handle_lflags(*lflag, i, args);
            } else {
            }
            ++i;
        }
        return 0;
    }

 private:
    std::optional<char> _as_short_flag(const std::string& arg) {
        if (arg.size() == 2 && arg[0] == '-' && arg[1] != '-')
            return std::make_optional(arg[1]);
        return std::nullopt;
    }

    std::optional<std::string> _as_long_flag(const std::string& arg) {
        if (arg.size() > 2 && arg[0] == '-' && arg[1] == '-' && arg[2] != '-')
            return std::make_optional(arg.substr(2));
        return std::nullopt;
    }

    size_t _handle_sflags(char sflag, size_t curindex,
                          const std::vector<std::string>& args) {
        if (_boolopts.shortflags().count(sflag)) {
            _boolopts[sflag] = true;
            return curindex;
        } else if (_opts.shortflags().count(sflag)) {
            std::string val{args[curindex + 1]};
            _opts[sflag] = val;
            return curindex + 1;
        } else if (_vectopts.shortflags().count(sflag)) {
            std::string val{args[curindex + 1]};
            _vectopts[sflag].push_back(val);
            return curindex + 1;
        }
        throw std::invalid_argument("Invalid short flag: " + args[curindex]);
    }

    size_t _handle_lflags(const std::string& lflag, size_t curindex,
                          const std::vector<std::string>& args) {
        if (_boolopts.longflags().count(lflag)) {
            _boolopts[lflag] = true;
            return curindex;
        } else if (_opts.longflags().count(lflag)) {
            std::string val{args[curindex + 1]};
            _opts[lflag] = val;
            return curindex + 1;
        } else if (_vectopts.longflags().count(lflag)) {
            std::string val{args[curindex + 1]};
            _vectopts[lflag].push_back(val);
            return curindex + 1;
        }
        throw std::invalid_argument("Invalid long flag: " + args[curindex]);
    }

    callback_t _cback;
    T* _parg;
    OptContainer<bool> _boolopts;
    OptContainer<Value> _opts;
    OptContainer<VectValue> _vectopts;
};
}  // namespace cli
}  // namespace upp
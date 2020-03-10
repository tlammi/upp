#pragma once
#include <algorithm>
#include <iomanip>
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

    Parser(callback_t callback = nullptr, T* parg = nullptr,
           std::string helpstr = "")
        : _cback{callback},
          _parg{parg},
          _helpstr{helpstr},
          _boolopts{},
          _opts{},
          _vectopts{},
          _subparsers{},
          _parsed_subcmd{""},
          _pos_args{} {
        _boolopts.add('h', "help", "Print this help and exit");
    }

    OptContainer<bool>& boolopts() { return _boolopts; }
    OptContainer<Value>& opts() { return _opts; }
    OptContainer<VectValue>& vectopts() { return _vectopts; }
    std::map<std::string, Parser<T>>& subcommands() { return _subparsers; }
    const std::string& parsed_subcmd() { return _parsed_subcmd; }
    const VectValue& pos_args() const { return _pos_args; }

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
            if (auto sflag = _as_short_flag(arg)) {
                if (*sflag == 'h') {
                    _print_help();
                    return 1;
                }
                i = _handle_sflags(*sflag, i, args);
            } else if (auto lflag = _as_long_flag(arg)) {
                if (*lflag == "help") {
                    _print_help();
                    return 1;
                }
                i = _handle_lflags(*lflag, i, args);
            } else {
                if (_subparsers.size()) {
                    std::vector<std::string> unparsed{
                        args.begin() + static_cast<long>(i) + 1, args.end()};
                    _parsed_subcmd = arg;
                    return _subparsers.at(arg).parse(unparsed);
                } else {
                    _pos_args.push_back(arg);
                }
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

    void _print_help() {
        size_t short_width{0}, long_width{0};

        std::vector<std::tuple<char, std::string, std::string>> help_data{};
        std::vector<std::string> help_rows{};
        auto tmp = _boolopts.help_data();
        for (const auto& tup : tmp) {
            long_width = std::max(long_width, std::get<1>(tup).size());
        }
        help_data.insert(help_data.end(), tmp.begin(), tmp.end());
        tmp = _opts.help_data();
        for (auto& tup : tmp) {
            long_width = std::max(long_width, std::get<1>(tup).size() + 6);
            std::get<1>(tup) += " VALUE";
        }
        help_data.insert(help_data.end(), tmp.begin(), tmp.end());
        tmp = _vectopts.help_data();
        for (auto& tup : tmp) {
            std::get<2>(tup) += " (Multiple can be specified)";
            long_width = std::max(long_width, std::get<1>(tup).size() + 6);
            std::get<1>(tup) += " VALUE";
        }
        help_data.insert(help_data.end(), tmp.begin(), tmp.end());

        std::sort(help_data.begin(), help_data.end(),
                  [](const auto& a, const auto& b) {
                      return std::get<2>(a) < std::get<2>(b);
                  });

        std::cerr << _helpstr << std::endl
                  << std::endl
                  << "Options: " << std::endl;

        for (auto [c, l, h] : help_data) {
            std::cerr << std::setw(short_width + 2) << std::right;
            if (c != '\0')
                std::cerr << "-" << c;
            else
                std::cerr << "   ";
            std::cerr << "    --" << std::setw(long_width + 2) << std::left << l
                      << " " << h << std::endl;
        }
    }

    callback_t _cback;
    T* _parg;
    std::string _helpstr;
    OptContainer<bool> _boolopts;
    OptContainer<Value> _opts;
    OptContainer<VectValue> _vectopts;
    std::map<std::string, Parser<T>> _subparsers;
    std::string _parsed_subcmd;
    VectValue _pos_args;
};
}  // namespace cli
}  // namespace upp
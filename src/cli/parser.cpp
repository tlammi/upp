#include "upp/cli/parser.hpp"

namespace upp {
namespace cli {

Parser::Parser(const std::string& helpstr, const callback_t& callback)
    : _cback{callback},
      _helpstr{helpstr},
      _parsing_data{},
      _subparsers{},
      _pos_args{} {
    _parsing_data.bool_options.add('h', "help", "Print this help and exit");
}

void Parser::add_bool_option(char shortflag, const std::string& longflag,
                             const std::string& helpstr) {
    _parsing_data.bool_options.add(shortflag, longflag, helpstr);
}
void Parser::add_bool_option(const std::string& longflag,
                             const std::string& helpstr) {
    _parsing_data.bool_options.add(longflag, helpstr);
}

void Parser::add_option(char shortflag, const std::string& longflag,
                        const std::string& helpstr) {
    _parsing_data.options.add(shortflag, longflag, helpstr);
}
void Parser::add_option(const std::string& longflag,
                        const std::string& helpstr) {
    _parsing_data.options.add(longflag, helpstr);
}

void Parser::add_vector_option(char shortflag, const std::string& longflag,
                               const std::string& helpstr) {
    _parsing_data.vector_options.add(shortflag, longflag, helpstr);
}
void Parser::add_vector_option(const std::string& longflag,
                               const std::string& helpstr) {
    _parsing_data.vector_options.add(longflag, helpstr);
}

Parser& Parser::add_subcommand(const std::string& name,
                               const std::string& helpstr,
                               const callback_t& callback) {
    _subparsers.emplace(std::make_pair(name, Parser(helpstr, callback)));
    return _subparsers.at(name);
}

int Parser::_parse(const std::vector<std::string>& args,
                   std::vector<std::string> cmdstack) {
    _parsing_data.bool_options.reset_values();
    _parsing_data.options.reset_values();
    _parsing_data.vector_options.reset_values();
    cmdstack.push_back(args.at(0));
    size_t i{1};
    while (i < args.size()) {
        std::string arg = args[i];
        if (auto sflag = _as_short_flag(arg)) {
            if (*sflag == 'h') {
                _print_help(cmdstack);
                return 1;
            }
            i = _handle_sflags(*sflag, i, args);
        } else if (auto lflag = _as_long_flag(arg)) {
            if (*lflag == "help") {
                _print_help(cmdstack);
                return 1;
            }
            i = _handle_lflags(*lflag, i, args);
        } else {
            if (_subparsers.size()) {
                if (_cback) _cback(_parsing_data);
                std::vector<std::string> unparsed{
                    args.begin() + static_cast<int64_t>(i), args.end()};
                return _subparsers.at(arg)._parse(unparsed, cmdstack);
            } else {
                _pos_args.push_back(arg);
            }
        }
        ++i;
    }
    if (_cback) _cback(_parsing_data);
    return 0;
}
std::optional<char> Parser::_as_short_flag(const std::string& arg) {
    if (arg.size() == 2 && arg[0] == '-' && arg[1] != '-')
        return std::make_optional(arg[1]);
    return std::nullopt;
}

std::optional<std::string> Parser::_as_long_flag(const std::string& arg) {
    if (arg.size() > 2 && arg[0] == '-' && arg[1] == '-' && arg[2] != '-')
        return std::make_optional(arg.substr(2));
    return std::nullopt;
}

size_t Parser::_handle_sflags(char sflag, size_t curindex,
                              const std::vector<std::string>& args) {
    if (_parsing_data.bool_options.shortflags().count(sflag)) {
        _parsing_data.bool_options[sflag] = true;
        return curindex;
    } else if (_parsing_data.options.shortflags().count(sflag)) {
        std::string val{args[curindex + 1]};
        _parsing_data.options[sflag] = val;
        return curindex + 1;
    } else if (_parsing_data.vector_options.shortflags().count(sflag)) {
        std::string val{args[curindex + 1]};
        _parsing_data.vector_options[sflag].push_back(val);
        return curindex + 1;
    }
    throw std::invalid_argument("Invalid short flag: " + args[curindex]);
}

size_t Parser::_handle_lflags(const std::string& lflag, size_t curindex,
                              const std::vector<std::string>& args) {
    if (_parsing_data.bool_options.longflags().count(lflag)) {
        _parsing_data.bool_options[lflag] = true;
        return curindex;
    } else if (_parsing_data.options.longflags().count(lflag)) {
        std::string val{args[curindex + 1]};
        _parsing_data.options[lflag] = val;
        return curindex + 1;
    } else if (_parsing_data.vector_options.longflags().count(lflag)) {
        std::string val{args[curindex + 1]};
        _parsing_data.vector_options[lflag].push_back(val);
        return curindex + 1;
    }
    throw std::invalid_argument("Invalid long flag: " + args[curindex]);
}

std::tuple<size_t, size_t> Parser::_flag_column_widths() {
    size_t short_width{0};
    size_t long_width{0};
    for (const auto& o : _parsing_data.bool_options.help_data()) {
        long_width = std::max(long_width, std::get<1>(o).size());
    }
    for (const auto& o : _parsing_data.options.help_data()) {
        long_width = std::max(long_width, std::get<1>(o).size());
    }
    for (const auto& o : _parsing_data.vector_options.help_data()) {
        long_width = std::max(long_width, std::get<1>(o).size());
    }

    if (_parsing_data.options.size() || _parsing_data.vector_options.size()) {
        long_width += 6;
        short_width += 6;
    }
    return std::make_tuple(short_width, long_width);
}
std::vector<std::tuple<std::string, std::string, std::string>>
Parser::_construct_help_vector() {
    std::vector<std::tuple<std::string, std::string, std::string>> help_data{};
    std::vector<std::string> help_rows{};
    auto tmp = _parsing_data.bool_options.help_data();
    for (const auto& tup : tmp) {
        const auto& [s, l, h] = tup;
        if (s != '\0')
            help_data.push_back(std::make_tuple(std::string(1, s), l, h));
        else
            help_data.push_back(std::make_tuple("", l, h));
    }
    tmp = _parsing_data.options.help_data();
    for (auto& tup : tmp) {
        const auto& [s, l, h] = tup;
        if (s != '\0')
            help_data.push_back(
                std::make_tuple(std::string(1, s) + " VALUE", l + " VALUE", h));
        else
            help_data.push_back(std::make_tuple("", l + " VALUE", h));
    }
    tmp = _parsing_data.vector_options.help_data();
    for (auto& tup : tmp) {
        const auto& [s, l, h] = tup;
        if (s != '\0')
            help_data.push_back(
                std::make_tuple(std::string(1, s) + " VALUE", l + " VALUE",
                                h + " (Multiple can be specified)"));
        else
            help_data.push_back(std::make_tuple(
                "", l + " VALUE", h + "(Multiple can be specified)"));
    }
    std::sort(help_data.begin(), help_data.end(),
              [](const auto& a, const auto& b) {
                  return std::get<1>(a) < std::get<1>(b);
              });
    return help_data;
}

void Parser::_print_help(const std::vector<std::string>& cmdstack) {
    const auto& [short_width, long_width] = _flag_column_widths();

    std::vector<std::tuple<std::string, std::string, std::string>> help_data{
        _construct_help_vector()};

    std::cerr << "Usage: ";
    for (size_t i{0}; i < cmdstack.size(); ++i) {
        std::cerr << cmdstack[i] << " ";
    }
    std::cerr << "[OPTIONS] ";
    if (_subparsers.size()) {
        std::cerr << "SUBCOMMAND";
    } else {
        std::cerr << "[POS ARGS]";
    }
    std::cerr << std::endl
              << std::endl
              << _helpstr << std::endl
              << std::endl
              << "Options: " << std::endl;

    for (const auto& [s, l, h] : help_data) {
        if (s != "\0")
            std::cerr << "-" << std::setw(short_width + 2) << std::left << s;
        else
            std::cerr << std::setw(short_width + 3) << "";
        std::cerr << "--" << std::setw(long_width + 2) << std::left << l << " "
                  << h << std::endl;
    }

    size_t cmd_colum_width{0};
    if (_subparsers.size()) {
        for (const auto& s : _subparsers) {
            cmd_colum_width = std::max(cmd_colum_width, s.first.size() + 4);
        }
        std::cerr << "Subcommands:" << std::endl;
        for (const auto& s : _subparsers) {
            std::cerr << std::setw(4) << "" << std::setw(cmd_colum_width)
                      << s.first << s.second._helpstr << std::endl;
        }
    }
}
}  // namespace cli
}  // namespace upp
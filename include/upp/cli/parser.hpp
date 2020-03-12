#pragma once
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>

#include "upp/cli/optcontainer.hpp"
#include "upp/cli/value.hpp"
#include "upp/cli/vectvalue.hpp"

namespace upp {
namespace cli {

class Parser {
 public:
    struct ParsingData {
        OptContainer<bool> bool_options;
        OptContainer<Value> options;
        OptContainer<VectValue> vector_options;
        VectValue positional_arguments;
    };

    typedef std::function<int(const ParsingData&)> callback_t;

    explicit Parser(const std::string& helpstr,
                    const callback_t& callback = nullptr);

    Parser(const Parser&) = delete;
    Parser(Parser&&) = default;

    void add_bool_option(char shortflag, const std::string& longflag,
                         const std::string& helpstr);
    void add_bool_option(const std::string& longflag,
                         const std::string& helpstr);
    template <typename... Args>
    void add_bool_options(char shortflag, const std::string& longflag,
                          const std::string& helpstr, Args... args) {
        add_bool_option(shortflag, longflag, helpstr);
        add_bool_options(args...);
    }
    template <typename... Args>
    void add_bool_options(const std::string& longflag,
                          const std::string& helpstr, Args... args) {
        add_bool_option(longflag, helpstr);
        add_bool_options(args...);
    }

    void add_option(char shortflag, const std::string& longflag,
                    const std::string& helpstr);
    void add_option(const std::string& longflag, const std::string& helpstr);

    template <typename... Args>
    void add_options(char shortflag, const std::string& longflag,
                     const std::string& helpstr, Args... args) {
        add_option(shortflag, longflag, helpstr);
        add_options(args...);
    }

    template <typename... Args>
    void add_options(const std::string& longflag, const std::string& helpstr,
                     Args... args) {
        add_option(longflag, helpstr);
        add_options(args...);
    }

    void add_vector_option(char shortflag, const std::string& longflag,
                           const std::string& helpstr);
    void add_vector_option(const std::string& longflag,
                           const std::string& helpstr);

    template <typename... Args>
    void add_vector_options(char shortflag, const std::string& longflag,
                            const std::string& helpstr, Args... args) {
        add_vector_option(shortflag, longflag, helpstr);
        add_vector_options(args...);
    }

    template <typename... Args>
    void add_vector_options(const std::string& longflag,
                            const std::string& helpstr, Args... args) {
        add_vector_option(longflag, helpstr);
        add_vector_options(args...);
    }

    Parser& add_subcommand(const std::string& name, const std::string& helpstr,
                           const callback_t& callback);

    inline int parse(int argc, const char** argv) {
        std::vector<std::string> args{argv, argv + argc};  // NOLINT
        return _parse(args, {});
    }

    inline int parse(const std::vector<std::string>& args) {
        return _parse(args, {});
    }

 private:
    int _parse(const std::vector<std::string>& args,
               std::vector<std::string> cmdstack);
    std::optional<char> _as_short_flag(const std::string& arg);

    std::optional<std::string> _as_long_flag(const std::string& arg);

    size_t _handle_sflags(char sflag, size_t curindex,
                          const std::vector<std::string>& args);

    size_t _handle_lflags(const std::string& lflag, size_t curindex,
                          const std::vector<std::string>& args);

    std::tuple<size_t, size_t> _flag_column_widths();

    std::vector<std::tuple<std::string, std::string, std::string>>
    _construct_help_vector();

    void _print_help(const std::vector<std::string>& cmdstack);

    bool _flag_added(char shortflag) const;
    bool _flag_added(const std::string& longflag) const;

    void _assert_no_flags(char shortflag, const std::string& longflag) const;
    void _assert_no_flags(const std::string& longflag) const;
    /*
     * Dummy function, used by variadic templates
     */
    constexpr void add_bool_options() const {}
    constexpr void add_options() const {}
    constexpr void add_vector_options() const {}

    callback_t _cback;
    std::string _helpstr;
    ParsingData _parsing_data;
    std::map<std::string, Parser> _subparsers;
    VectValue _pos_args;
};
}  // namespace cli
}  // namespace upp
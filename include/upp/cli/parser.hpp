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

/**
 * \brief Command line parser supporting hierarchical commands
 *
 * Parser is based on callbacks. Callbacks are specified for the objects during
 * the construction and are invoked once CLI arguments have been parsed. The
 * invocation is done before the subparsers are invoked.
 *
 * ## Terms:
 * - **bool option** -- Option that expects no value, e.g. ./program --debug
 * - **option** -- A "normal" option that can be specified once and expects one
 * argument, e.g. ./program --option value
 * - **vector option** -- Same as option, but can be specified multiple times,
 * e.g. ./program --option a --option b --option c
 */
class Parser {
 public:
    /**
     * \brief Struct for grouping parsing data
     */
    struct ParsingData {
        /**
         * Options that do not expect a parameter
         *
         * Used for storing flag-only options
         */
        OptContainer<bool> bool_options;

        /**
         * Options that expect a parameter
         *
         * Used for storing options consisting of a flag and a value.
         */
        OptContainer<Value> options;

        /**
         * Options that expect a parameter and can be specified multiple times
         */
        OptContainer<VectValue> vector_options;

        /**
         * Positional arguments parsed
         */
        VectValue positional_arguments;
    };

    /**
     * \brief Type for callbacks invoked by the parser
     */
    typedef std::function<int(const ParsingData&)> callback_t;

    /**
     * \brief Ctor
     *
     * Constructs a new parser
     *
     * \param helpstr Help string printed when -h/--help flag is specified
     * \param callback Callback called when parsing has been finished for a
     * command. Invoked before the subcommands are parsed
     */
    explicit Parser(const std::string& helpstr,
                    const callback_t& callback = nullptr);

    Parser(const Parser&) = delete;
    Parser(Parser&&) = default;
    Parser& operator=(const Parser&) = delete;
    Parser& operator=(Parser&&) = default;
    ~Parser();

    /**
     * \brief Add flag-only option to the parser
     * \param shortflag Short name for the option, e.g. -h (optional)
     * \param longflag Long name for the option, e.g. --help
     * \param helpstr String printed next to the option names in help message
     * \{
     */
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
    /**\}*/

    /**
     * \brief Add option expecting a value to the parser
     *
     * \param shortflag Short name for the flag (optional)
     * \param longflag Long name for the flag
     * \param helpstr Help string printed next to the flag names in the help
     * message
     * \{
     */
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
    /**\}*/

    /**
     * \brief Add options expecting 0-inf values to the parser
     *
     * \param shortflag Short name for the flag (optional)
     * \param longflag Long name for the flag
     * \param helpstr Help string printed next to the flag names in the help
     * message
     * \{
     */
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
    /**\}*/

    /**
     * \brief Add a subcommand to the CLI
     *
     * Constructs a new Parser under the parent parser handling a subcommand and
     * its arguments. A subparser and the parent have individual sets of options
     * so same flag can be specified for both separately.
     *
     * \param name Name of the subcommand
     * \param helpstr Help string printed next to the command in help message
     * \param callback Callback invoked once the CLI has been parsed
     */
    Parser& add_subcommand(const std::string& name, const std::string& helpstr,
                           const callback_t& callback);

    /**
     * \brief Parse the CLI arguments
     * \{
     */
    inline int parse(int argc, const char** argv) {
        std::vector<std::string> args{argv, argv + argc};  // NOLINT
        return _parse(args, {});
    }

    inline int parse(const std::vector<std::string>& args) {
        return _parse(args, {});
    }
    /**\}*/
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
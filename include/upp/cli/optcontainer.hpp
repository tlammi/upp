#pragma once

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "upp/cli/value.hpp"

namespace upp {
namespace cli {

/**
 * \brief Utility class for managing sets of CLI options
 *
 * Basically a wrapper on top of multiple std::maps offering
 * indexing operators for both short and long flags.
 */
template <typename T>
class OptContainer {
 public:
    OptContainer()
        : _sflag_cache{},
          _lflag_cache{},
          _stol_mapping{},
          _ltov_mapping{},
          _ltoh_mapping{},
          _caches_up_to_date{true} {}

    /**
     * \brief Add a new flag to the container
     *
     * \param shortflag Short name of the option
     * \param longflag Long name of the option
     * \param help Help string for the option
     */
    void add(char shortflag, std::string longflag, std::string help = "") {
        using namespace std::string_literals;
        if (_stol_mapping.count(shortflag))
            throw std::logic_error(
                "Key already present: "s.append(&shortflag, 1));
        _stol_mapping[shortflag] = longflag;
        add(longflag, help);
    }

    /**
     * \brief Add a new flag to the container (no shortflag -variant)
     *
     * \param longflag Long name of the option
     * \param help Help string for the option
     */
    void add(std::string longflag, std::string help = "") {
        using namespace std::string_literals;
        if (_ltov_mapping.count(longflag))
            throw std::logic_error("Key already present: "s + longflag);
        _caches_up_to_date = false;
        _ltov_mapping[longflag] = T();
        _ltoh_mapping[longflag] = help;
    }

    /**
     * \brief Accessor for the help string
     *
     * \param longflag Long flag previously stored in the OptContainer
     */
    const std::string& helpstr(const std::string& longflag) const {
        return _ltoh_mapping.at(longflag);
    }

    /**
     * \brief Accessor for the help string
     *
     * \param shortflag Short flag previously stored in the OptContainer
     */
    const std::string& helpstr(char shortflag) const {
        return helpstr(_stol_mapping.at(shortflag));
    }

    /**
     * \brief Constructs the help data of the container
     *
     * \return Vector of tuples where:
     *  - field 1 is the short flag or '\0' if not present
     *  - field 2 is the long flag
     *  - field 3 is the help string
     */
    std::vector<std::tuple<char, std::string, std::string>> help_data() {
        if (!_caches_up_to_date) _refresh_caches();

        std::vector<std::tuple<char, std::string, std::string>> out;
        for (const auto s : _sflag_cache) {
            auto l = _stol_mapping.at(s);
            auto h = _ltoh_mapping.at(l);
            _lflag_cache.erase(l);
            out.push_back(std::make_tuple(s, l, h));
        }
        for (const auto l : _lflag_cache) {
            auto h = _ltoh_mapping.at(l);
            out.push_back(std::make_tuple('\0', l, h));
        }
        _caches_up_to_date = false;
        return out;
    }

    /**
     * \brief Access longflag via the shortflag
     *
     * \param shortflag Short flag stored earlier in the object
     * \return Long flag corresponding to the short flag
     */
    const std::string& longflag(char shortflag) const {
        return _stol_mapping.at(shortflag);
    }

    /**
     * \brief Set of short flags stored in the object
     */
    const std::set<char>& shortflags() {
        if (!_caches_up_to_date) {
            _refresh_caches();
            _caches_up_to_date = true;
        }
        return _sflag_cache;
    }

    /**
     * \brief Set of long flags stored in the object
     */
    const std::set<std::string>& longflags() {
        if (!_caches_up_to_date) {
            _refresh_caches();
            _caches_up_to_date = true;
        }
        return _lflag_cache;
    }

    /**
     * \brief Resets all of the values stored in the object
     *
     * The reset is done by simply calling default constructor for all value
     * fields
     */
    void reset_values() {
        for (auto pair = _ltov_mapping.begin(); pair != _ltov_mapping.end();
             ++pair) {
            pair->second = T();
        }
    }

    /**
     * \brief Number of elements in the object
     *
     * The number is calculated as the number of long flags. Short flags are
     * optional and basically just references to long flags so they are not
     * used.
     */
    size_t size() const { return _ltov_mapping.size(); }

    /**
     * \brief Check if a matching short flag is stored in the object
     * \return true if present, false otherwise
     */
    bool contains(char shortflag) const {
        return static_cast<bool>(_stol_mapping.count(shortflag));
    }

    /**
     * \brief Check if a matching long flag is stored in the object
     * \return true if present, false otherwise
     */
    bool contains(const std::string& longflag) const {
        return static_cast<bool>(_ltov_mapping.count(longflag));
    }

    /**
     * \brief Index operator overloads
     *
     * Access values pointed by short or long flags
     * \{
     */
    const T& operator[](char shortflag) const {
        return this->operator[](_stol_mapping.at(shortflag));
    }
    const T& operator[](const std::string& longflag) const {
        return _ltov_mapping.at(longflag);
    }
    T& operator[](char shortflag) {
        return this->operator[](_stol_mapping.at(shortflag));
    }
    T& operator[](const std::string& longflag) {
        return _ltov_mapping.at(longflag);
    }
    /**\}*/

 private:
    void _refresh_caches() {
        _sflag_cache.clear();
        _lflag_cache.clear();

        std::transform(_stol_mapping.begin(), _stol_mapping.end(),
                       std::inserter(_sflag_cache, _sflag_cache.end()),
                       [](auto pair) { return pair.first; });

        std::transform(_ltov_mapping.begin(), _ltov_mapping.end(),
                       std::inserter(_lflag_cache, _lflag_cache.end()),
                       [](auto pair) { return pair.first; });
    }
    std::set<char> _sflag_cache;
    std::set<std::string> _lflag_cache;
    std::map<char, std::string> _stol_mapping;
    std::map<std::string, T> _ltov_mapping;
    std::map<std::string, std::string> _ltoh_mapping;
    bool _caches_up_to_date;
};
}  // namespace cli
}  // namespace upp
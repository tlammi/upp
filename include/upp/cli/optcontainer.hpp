#pragma once

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "upp/cli/value.hpp"

namespace upp {
namespace cli {

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
    void add(char shortflag, std::string longflag, std::string help = "") {
        _stol_mapping[shortflag] = longflag;
        add(longflag, help);
    }
    void add(std::string longflag, std::string help = "") {
        _ltov_mapping[longflag] = T();
        _caches_up_to_date = false;
        _ltoh_mapping[longflag] = help;
    }

    const std::string& helpstr(const std::string& longflag) const {
        return _ltoh_mapping.at(longflag);
    }
    const std::string& helpstr(char shortflag) const {
        return helpstr(_stol_mapping.at(shortflag));
    }

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

    const std::string& longflag(char shortflag) const {
        return _stol_mapping.at(shortflag);
    }

    const std::set<char>& shortflags() {
        if (!_caches_up_to_date) {
            _refresh_caches();
            _caches_up_to_date = true;
        }
        return _sflag_cache;
    }
    const std::set<std::string>& longflags() {
        if (!_caches_up_to_date) {
            _refresh_caches();
            _caches_up_to_date = true;
        }
        return _lflag_cache;
    }

    void reset_values() {
        for (auto pair = _ltov_mapping.begin(); pair != _ltov_mapping.end();
             ++pair) {
            pair->second = T();
        }
    }

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
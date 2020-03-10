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
          _caches_up_to_date{true} {}
    void add(char shortflag, std::string longflag) {
        _stol_mapping[shortflag] = longflag;
        add(longflag);
    }
    void add(std::string longflag) {
        _ltov_mapping[longflag] = T();
        _caches_up_to_date = false;
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

    void clear() {
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
    bool _caches_up_to_date;
};
}  // namespace cli
}  // namespace upp
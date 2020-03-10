#pragma once

#include <map>
#include <string>
#include <vector>
#include "upp/cli/value.hpp"

namespace upp {
namespace cli {

template <typename T>
class OptContainer {
 public:
    OptContainer() : _stol_mapping{}, _ltov_mapping{} {}
    void add(char shortflag, std::string longflag) {
        _stol_mapping[shortflag] = longflag;
        add(longflag);
    }
    void add(std::string longflag) { _ltov_mapping[longflag] = T(); }

    std::vector<char> shortflags() const {
        std::vector<char> out;
        for (auto pair = _stol_mapping.begin(); pair != _stol_mapping.end();
             ++pair) {
            out.push_back(pair->first);
        }
        return out;
    }
    std::vector<std::string> longflags() const {
        std::vector<std::string> out;
        for (auto pair = _ltov_mapping.begin(); pair != _ltov_mapping.end();
             ++pair) {
            out.push_back(pair->first);
        }
        return out;
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
    std::map<char, std::string> _stol_mapping;
    std::map<std::string, T> _ltov_mapping;
};
}  // namespace cli
}  // namespace upp
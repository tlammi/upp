#pragma once

#include <upp/linux/process.hpp>


namespace upp::linux{

class run_t {

    template <class T>
    static constexpr const char* to_c_str(T& t) noexcept {
        if constexpr (std::convertible_to<T, const char*>)
            return t;
        else
            return t.c_str();
    }

    struct options {
        fs::file std_in{};
        fs::file std_out{};
        fs::file std_err{};
    };

    static void execute(const char* path, char* const* argv, char* const* envp);

    template <detail::c_stringable... Ts>
    static void init_args(std::vector<char*>& out, Ts&... ts) {
        out.reserve(sizeof...(Ts)+1);
        (out.push_back(strdup(to_c_str(ts))), ...);
        out.push_back(nullptr);
    }

 public:
    constexpr explicit run_t() noexcept = default;

    template <detail::c_stringable T, detail::c_stringable... Ts>
    int operator()(T& t, Ts&... ts) const {
        auto proc = process([&]{
          auto args = std::vector<char*>();
          auto envp = std::vector<char*>{nullptr};
          // execute() never returns on success so this is only executed on errors.
          auto cleanup = upp::cleanup([&]{
              for(auto* ptr : args) ::std::free(ptr);
              for(auto* ptr: envp) ::std::free(ptr);
          });
          init_args(args, t, ts...);
          execute(args[0], args.data(), envp.data());
        });
        auto res = proc.join();
        return res.exit_code;
    }
};

constexpr run_t run{};

}

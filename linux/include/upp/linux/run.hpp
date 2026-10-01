#pragma once

#include <unistd.h>

#include <upp/exceptions.hpp>
#include <upp/fs/file.hpp>
#include <upp/linux/pipe.hpp>
#include <upp/linux/process.hpp>
#include <variant>

namespace upp::linux {

class run_t {
    static char* strview_dup(std::string_view s) {
        // NOLINTNEXTLINE
        auto* ptr = new char[s.size() + 1];
        std::copy(s.begin(), s.end(), ptr);
        ptr[s.size()] = '\0';  // NOLINT
        return ptr;
    }

    // RAII wrapper for null terminated vector of char*.
    //
    // Used for passing arguments to subprograms
    struct arg_vector {
        std::vector<char*> vector{nullptr};

        constexpr arg_vector() = default;

        template <std::ranges::range Range>
        constexpr explicit arg_vector(Range&& rng) : vector{} {
            vector.reserve(rng.size() + 1);
            for (auto& arg : UPP_FWD(rng)) vector.push_back(strview_dup(arg));
            vector.push_back(nullptr);
        }

        arg_vector(const arg_vector&) = delete;
        arg_vector& operator=(const arg_vector&) = delete;
        constexpr arg_vector(arg_vector&& other) noexcept = default;
        constexpr arg_vector& operator=(arg_vector&& other) noexcept = default;

        constexpr ~arg_vector() {
            for (auto* p : vector) delete[] p;  // NOLINT
        }
    };

    using ivariant =
        std::variant<fs::readable_file*, fs::native_handle, pipe_pair*>;

    using ovariant =
        std::variant<fs::writable_file*, fs::native_handle, pipe_pair*>;

    struct options {
        ivariant std_in{};
        ovariant std_out{};
        ovariant std_err{};
    };
    static void execute(arg_vector cmd, arg_vector envp, options opts);

 public:
    constexpr explicit run_t() noexcept = default;

    int operator()(std::initializer_list<std::string_view> cmd) const {
        return operator()(cmd, options{});
    }

    int operator()(std::initializer_list<std::string_view> cmd,
                   options opts) const;

    template <std::ranges::range Range>
    int operator()(Range&& args) const {
        static_assert(
            std::convertible_to<typename std::remove_cvref_t<Range>::value_type,
                                std::string_view>,
            "Run arguments must be convertible to std::string_view");
        auto proc = process(
            [&] { execute(arg_vector(UPP_FWD(args)), arg_vector(), {}); });
        auto res = proc.join();
        return res.exit_code;
    }
};

constexpr run_t run{};

}  // namespace upp::linux

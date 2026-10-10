#pragma once

#include <unistd.h>

#include <upp/exceptions.hpp>
#include <upp/fs/file.hpp>
#include <upp/linux/pipe.hpp>
#include <upp/linux/process.hpp>
#include <variant>

namespace upp::linux {

class run_t {
    static void execute(const char* path, char* const* argv, char* const* envp);

    static char* strview_dup(std::string_view s) {
        // NOLINTNEXTLINE
        auto* ptr = new char[s.size() + 1];
        std::copy(s.begin(), s.end(), ptr);
        ptr[s.size()] = '\0';  // NOLINT
        return ptr;
    }

    template <std::ranges::range Range>
    static void init_args(std::vector<char*>& out, Range args) {
        out.reserve(args.size() + 1);
        for (auto& arg : args) { out.push_back(strview_dup(arg)); }
        out.push_back(nullptr);
    }

    using ivariant =
        std::variant<fs::readable_file*, fs::native_handle, pipe_pair*>;

    using ovariant =
        std::variant<fs::writable_file*, fs::native_handle, pipe_pair*>;

    struct ivariant_visitor {
        void operator()(fs::readable_file* f) {
            if (!f) return;
            operator()(f->release());
        }

        void operator()(fs::native_handle f) {
            if (f < 0) return;
            if (f == STDIN_FILENO) return;
            auto res = ::dup2(f, STDIN_FILENO);
            if (res < 0) throw_errno();
            ::close(f);
        }
        void operator()(pipe_pair* pipe) {
            if (!pipe) return;
            pipe->write.close();
            operator()(&pipe->read);
        }
    };

    struct options {
        ivariant std_in;
        fs::writable_file std_out{};
        fs::writable_file std_err{};
    };

 public:
    constexpr explicit run_t() noexcept = default;

    int operator()(std::initializer_list<std::string_view> cmd) const {
        return operator()(cmd, options{});
    }

    int operator()(std::initializer_list<std::string_view> cmd,
                   options opts) const {
        auto proc = process([&] {
            auto args = std::vector<char*>();
            auto envp = std::vector<char*>{nullptr};
            // execute() never returns on success so this is only executed on
            // errors.
            auto cleanup = upp::cleanup([&] {
                for (auto* ptr : args) delete[] (ptr);  // NOLINT
                for (auto* ptr : envp) delete[] (ptr);  // NOLINT
            });
            init_args(args, cmd);
            if (opts.std_out) ::dup2(opts.std_out.native(), STDOUT_FILENO);
            if (opts.std_err) ::dup2(opts.std_err.native(), STDERR_FILENO);
            std::visit(ivariant_visitor(), opts.std_in);
            execute(args[0], args.data(), envp.data());
        });
        auto res = proc.join();
        return res.exit_code;
    }

    template <std::ranges::range Range>
    int operator()(Range&& args) const {
        static_assert(
            std::convertible_to<typename std::remove_cvref_t<Range>::value_type,
                                std::string_view>,
            "Run arguments must be const char* or convertible to one");
        auto proc = process([&] {
            auto argp = std::vector<char*>();
            auto envp = std::vector<char*>{nullptr};
            auto cleanup = upp::cleanup([&] {
                for (auto* ptr : argp) delete[] (ptr);  // NOLINT
                for (auto* ptr : envp) delete[] (ptr);  // NOLINT
            });
            init_args(argp, UPP_FWD(args));
            execute(argp[0], argp.data(), envp.data());
        });
        auto res = proc.join();
        return res.exit_code;
    }
};

constexpr run_t run{};

}  // namespace upp::linux

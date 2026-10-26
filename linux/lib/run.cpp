#include <unistd.h>

#include <upp/exceptions.hpp>
#include <upp/linux/run.hpp>
#include <upp/matcher.hpp>
#include <upp/visit.hpp>

namespace upp::linux {
namespace {
void do_close(fs::native_handle fd) noexcept {
    if (fd == fs::null_native_handle) return;
    // Just skip closing standard FDs. This does not really hurt and avoids
    // edge cases
    if (fd == any_of(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO)) return;
    ::close(fd);
}

// Move FD to another handle
fs::native_handle realloc_fd(fs::native_handle oldfd) {
    auto newfd = dup(oldfd);
    if (newfd < 0) throw_errno();
    do_close(oldfd);
    return newfd;
}

void do_dup(fs::native_handle oldfd, fs::native_handle newfd) {
    if (oldfd == fs::null_native_handle) return;
    auto res = dup2(oldfd, newfd);
    if (res < 0) throw_errno();
}

void do_execute(const char* path, char* const* argv, char* const* envp) {
    (void)::execvpe(path, argv, envp);
    // execvpe never returns on success.
    throw_errno();
}

// NOTE: This does not close the file descriptors. The closing needs to be done
// afterwards. The reason is possible aliasing: A single fd might be passed for
// instance as stdin and stderr target. Resource leaks are also not a problem
// since this is called after fork and immediately before execve.
struct ivariant_visitor {
    void operator()(fs::readable_file* f) const {
        if (!f) return;
        operator()(f->release());
    }

    void operator()(fs::native_handle f) const {
        if (f < 0) return;
        if (f == STDIN_FILENO) return;
        auto res = ::dup2(f, STDIN_FILENO);
        if (res < 0) throw_errno();
    }
    void operator()(pipe_pair* pipe) const {
        if (!pipe) return;
        operator()(&pipe->read);
    }
};

// NOTE: This does not close the file descriptors. The closing needs to be done
// afterwards. The reason is possible aliasing: A single fd might be passed for
// instance as stdin and stderr target. Resource leaks are also not a problem
// since this is called after fork and immediately before execve.
struct ovariant_visitor {
    fs::native_handle tgt;

    void operator()(fs::writable_file* f) const {
        if (!f) return;
        operator()(f->release());
    }

    void operator()(fs::native_handle f) const {
        if (f < 0) return;
        if (f == tgt) return;
        auto res = ::dup2(f, tgt);
        if (res < 0) throw_errno();
    }
    void operator()(pipe_pair* pipe) const {
        if (!pipe) return;
        operator()(&pipe->write);
    }
};

struct fds {
    using handle = fs::native_handle;
    handle std_in;
    handle std_out;
    handle std_err;
    handle std_in_secondary{};
    handle std_out_secondary{};
    handle std_err_secondary{};
};

struct unpack_fds_visitor {
    bool std_in = false;

    std::pair<fs::native_handle, fs::native_handle> operator()(
        fs::native_handle h) const noexcept {
        return {h, fs::null_native_handle};
    }

    std::pair<fs::native_handle, fs::native_handle> operator()(
        fs::writable_file* f) const noexcept {
        if (!f) return {fs::null_native_handle, fs::null_native_handle};
        return {f->release(), fs::null_native_handle};
    }

    std::pair<fs::native_handle, fs::native_handle> operator()(
        fs::readable_file* f) const noexcept {
        if (!f) return {fs::null_native_handle, fs::null_native_handle};
        return {f->release(), fs::null_native_handle};
    }

    std::pair<fs::native_handle, fs::native_handle> operator()(
        pipe_pair* pipe) const noexcept {
        if (!pipe) return {fs::null_native_handle, fs::null_native_handle};
        if (std_in) return {pipe->read.release(), pipe->write.release()};
        // Bug here, does not work if the same pipe is passed as stdin and err
        return {pipe->write.release(), pipe->read.release()};
    }
};

fds unpack_fds(const auto& opts) noexcept {
    auto in_pair = std::visit(unpack_fds_visitor{.std_in = true}, opts.std_in);

    auto out_pair =
        std::visit(unpack_fds_visitor{.std_in = false}, opts.std_out);
    auto err_pair =
        std::visit(unpack_fds_visitor{.std_in = false}, opts.std_err);
    assert(in_pair.second !=
           any_of(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));
    assert(out_pair.second !=
           any_of(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));
    assert(err_pair.second !=
           any_of(STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

    // handle aliasing
    if (opts.std_out == opts.std_err &&
        err_pair.first == fs::null_native_handle) {
        err_pair = out_pair;
    }
    return {
        .std_in = in_pair.first,
        .std_out = out_pair.first,
        .std_err = err_pair.first,
        .std_in_secondary = in_pair.second,
        .std_out_secondary = out_pair.second,
        .std_err_secondary = err_pair.second,
    };
}

void swap_fds(fs::native_handle a, fs::native_handle b) {
    auto tmp = realloc_fd(a);
    do_dup(b, a);
    do_dup(tmp, b);
}

// This is messy and has a lot of edge cases
void setup_fds(fds in) {
    assert(in.std_in != any_of(STDOUT_FILENO, STDERR_FILENO) &&
           "Trying to redirect stdin to stdout or stderr");
    if (in.std_out == STDERR_FILENO && in.std_err == STDOUT_FILENO) {
        swap_fds(in.std_out, in.std_err);
    } else if (in.std_out == STDERR_FILENO) {
        do_dup(in.std_err, STDERR_FILENO);
        do_dup(in.std_out, STDOUT_FILENO);
    } else {
        // No need for another if-clause if we keep this order
        do_dup(in.std_err, STDERR_FILENO);
        do_dup(in.std_out, STDOUT_FILENO);
        do_close(in.std_err);
        if (in.std_out != in.std_err) do_close(in.std_out);
    }
    do_dup(in.std_in, STDIN_FILENO);

    if (in.std_out_secondary != in.std_in) do_close(in.std_out_secondary);
    if (in.std_err_secondary != in.std_in &&
        in.std_err_secondary != in.std_out_secondary)
        do_close(in.std_err_secondary);
    if (in.std_in_secondary != any_of(in.std_out, in.std_err))
        do_close(in.std_in_secondary);
}

}  // namespace

void run_t::execute(arg_vector cmd, arg_vector envp, options opts) {
    // This is run in a subrocess so we don't have to care about missing cleanup
    // on exceptions. The FD dance is pretty complicated due to edge cases.
    auto fds = unpack_fds(opts);
    setup_fds(fds);
    do_execute(cmd.vector[0], cmd.vector.data(), envp.vector.data());
}

int run_t::operator()(std::initializer_list<std::string_view> cmd,
                      options opts) const {
    auto proc = process([&] { execute(arg_vector(cmd), arg_vector(), opts); });
    auto res = proc.join();
    return res.exit_code;
}
}  // namespace upp::linux

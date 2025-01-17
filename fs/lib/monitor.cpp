#include <sys/inotify.h>

#include <upp/exceptions.hpp>
#include <upp/fs/monitor.hpp>
#include <vector>

#if 0
namespace upp::fs {
namespace {

posix::fd open_inotify_watch(int inotify_fd,
                             const std::filesystem::path& path) {
    auto fd = inotify_add_watch(inotify_fd, path.native().c_str(),
                                IN_OPEN | IN_CLOSE);
    if (fd == -1) throw_errno();
    return posix::fd(fd);
}
}  // namespace

std::generator<event> monitor(std::filesystem::path path, monitor_opts opts) {
    auto fd = posix::fd(inotify_init1(IN_NONBLOCK));
    if (fd.get() == -1) throw_errno();
    auto monitored_fds = std::vector<posix::fd>();
    monitored_fds.emplace_back(open_inotify_watch(fd.get(), path));
    co_yield {};
}
}  // namespace upp::fs
#endif

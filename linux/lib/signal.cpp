#include <upp/exceptions.hpp>
#include <upp/linux/signal.hpp>

namespace upp::linux {
namespace {
sigset_t build_sigset(std::initializer_list<int> mask) {
    auto out = sigset_t{};
    int res = sigemptyset(&out);
    if (res) throw_errno();
    for (auto s : mask) {
        res = sigaddset(&out, s);
        if (res) throw_errno();
    }
    return out;
}
}  // namespace

signal_listener::signal_listener(std::initializer_list<int> mask)
    : m_sigset(build_sigset(mask)) {
    int res = pthread_sigmask(SIG_BLOCK, &m_sigset, nullptr);
    if (res) throw_errno();
}

int signal_listener::wait() {
    int sig = 0;
    auto res = sigwait(&m_sigset, &sig);
    if (res) throw_errno();
    return sig;
}

}  // namespace upp::linux

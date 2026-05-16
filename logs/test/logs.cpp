#include <cassert>
#include <upp/logs.hpp>

using level = upp::logs::level;
using upp::logs::root_context;

const auto a = upp::logs::context({.name = "a"});
const auto b = upp::logs::context({.name = "b"});
const auto aa = upp::logs::context(&a, {.name = "aa"});
const auto ab = upp::logs::context(&a, {.name = "ab"});

template <class... Ts>
void check_level(level expected, const Ts&... ctxs) {
    (assert(ctxs.effective_level() == expected), ...);
}

int main() {
    upp::logs::init();

    check_level(level::unset, root_context, a, b, aa, ab);
    root_context.set_level(level::trace);
    check_level(level::trace, root_context, a, b, aa, ab);
    root_context.set_level(level::unset);
    check_level(level::unset, root_context, a, b, aa, ab);
    root_context.set_level(level::trace);
    a.set_level(level::debug);
    check_level(level::trace, root_context, b);
    check_level(level::debug, a, aa, ab);
    a.set_level(level::unset);
    check_level(level::trace, root_context, a, b, aa, ab);
}

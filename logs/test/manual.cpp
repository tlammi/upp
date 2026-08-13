#include <format>
#include <upp/logs.hpp>

using level = upp::logs::level;
using upp::logs::root_context;

const auto a = upp::logs::context("a");
const auto b = upp::logs::context("b");
const auto aa = upp::logs::context(a, "aa");
const auto ab = upp::logs::context(a, "ab");

void do_log(const auto& ctx) {
    std::fputs("START\n", stderr);
    UPP_TRACE(ctx, "trace");
    UPP_DEBUG(ctx, "debug");
    UPP_INFO(ctx, "info");
    UPP_NOTE(ctx, "note");
    UPP_WARN(ctx, "warn");
    UPP_ERR(ctx, "error");
    UPP_FATAL(ctx, "fatal");
    std::fputs("STOP\n", stderr);
}

int main() {
    upp::logs::quick_init(level::trace);
    const auto main_ctx = upp::logs::context(b, "main");
    do_log(root_context());
    do_log(a);
    do_log(b);
    do_log(aa);
    do_log(ab);
    do_log(main_ctx);
}

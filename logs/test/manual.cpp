#include <format>
#include <upp/logs.hpp>

using level = upp::logs::level;
using upp::logs::root_context;

const auto a = upp::logs::context({.name = "a"});
const auto b = upp::logs::context({.name = "b"});
const auto aa = upp::logs::context(&a, {.name = "aa"});
const auto ab = upp::logs::context(&a, {.name = "ab"});

int main() {
    upp::logs::init({.level = upp::logs::level::trace});
    UPP_TRACE(root_context, "trace");
    UPP_DEBUG(root_context, "debug");
    UPP_INFO(root_context, "info");
    UPP_NOTE(root_context, "note");
    UPP_WARN(root_context, "warn");
    UPP_ERR(root_context, "error");
    UPP_FATAL(root_context, "fatal");
}

#include <dlfcn.h>

#include <stdexcept>
#include <upp/linux/dlopen.hpp>

namespace upp::linux {

void dynamic_library::do_close(void* handle) noexcept { dlclose(handle); }

dynamic_library::dynamic_library(const char* path)
    : dynamic_library(path, RTLD_LAZY) {}


dynamic_library::dynamic_library(const char* path, int flags)
    : m_handle(dlopen(path, flags)) {
    if (!m_handle) throw std::runtime_error(dlerror());
}

void* dynamic_library::raw_symbol(cstr name) const {
    assert(m_handle && "not a valid dynamic_library");
    assert(name.c_str() && "trying to get a null symbol");
    dlerror();
    auto* handle = dlsym(m_handle, name.c_str());
    if (!handle) throw std::runtime_error(dlerror());
    return handle;
}

}  // namespace upp::linux

#include <upp/codec/hwaccel.hpp>
#include <upp/logs.hpp>

int main() {
    upp::logs::quick_init(upp::logs::level::trace);
    for (auto spec : upp::codec::supported_hardware_accelerators()) {
        std::puts(spec.name());
    }
}

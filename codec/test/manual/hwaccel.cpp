#include <upp/codec/hwaccel.hpp>
#include <upp/logs.hpp>

int main() {
    upp::logs::init();
    for (auto spec : upp::codec::supported_hardware_accelerators()) {
        std::puts(spec.name());
    }
}

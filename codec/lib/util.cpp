#include <upp/codec/decoder.hpp>
#include <upp/codec/demux.hpp>
#include <upp/codec/util.hpp>
#include <upp/fs/file.hpp>

namespace upp::codec {

frame decode_image(std::span<const std::byte> data) {
    auto dem = demux(data);
    auto idx = dem.best_video_stream();
    auto dec = decoder({.stream = dem.stream_info()[idx]});
    auto pkt = packet();
    auto frm = frame();
    while (dem >> pkt) {
        dec << pkt;
        if (!(dec >> frm)) continue;
        return frm;
    }
    throw std::runtime_error("could not decode a frame from an image");
}

frame decode_image(const std::filesystem::path& path) {
    // TODO: This could be made to feed the data incrementally
    auto f = upp::fs::file(path);
    auto dat = f.read_bin();
    return decode_image(dat);
}
}  // namespace upp::codec

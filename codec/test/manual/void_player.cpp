#include <print>
#include <upp/codec/decoder.hpp>
#include <upp/fs/file.hpp>
#include <upp/logs.hpp>

int main(int argc, char** argv) {
    upp::logs::init({
        .level = upp::logs::level::trace,
    });
    for (int i = 1; i < argc; ++i) {
        size_t count = 1;
        // NOLINTNEXTLINE
        auto dat = upp::fs::file(argv[i]).read_bin();
        UPP_DEBUG(upp::logs::root_context, "read {} bytes from media",
                  dat.size());
        auto demux = upp::codec::demux(dat);
        auto video_stream = demux.best_video_stream();
        auto vid_decoder = upp::codec::decoder{{
            .stream = demux.stream_info()[video_stream],
        }};
        auto pkt = upp::codec::packet();
        auto frm = upp::codec::frame();
        while (demux >> pkt) {
            if (pkt.stream() != video_stream) continue;
            vid_decoder << pkt;
            while (vid_decoder >> frm) {
                std::println("decoding frame nm {}", count);
                ++count;
            }
        }
    }
}

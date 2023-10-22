extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/bsf.h>
#include <libavcodec/codec.h>
#include <libavcodec/codec_par.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libavutil/dict.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <unistd.h>

#include "libavutil/mathematics.h"
}
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

void init_stream_server() {
    auto logfield = spdlog::stdout_color_mt("stream server");
    logfield->info("init stream server");

    //
}
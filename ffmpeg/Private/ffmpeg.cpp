
#include <spdlog/logger.h>
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

#include <opencv4/opencv2/opencv.hpp>
#include <string>
using namespace std::literals::string_literals;
void ffmpeg_main() {
    AVFormatContext *fmt_ctx = avformat_alloc_context();
    avformat_open_input(&fmt_ctx, "rtsp://172.17.106.250:8554/test", NULL, NULL);
    try {
        avformat_find_stream_info(fmt_ctx, NULL);
    } catch (std::exception &e) {
        spdlog::error("avformat_find_stream_info error: {}", e.what());
    }
    int video_stream_idx = -1;
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
        }
    }
    AVCodecParameters *codecParameter = fmt_ctx->streams[video_stream_idx]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecParameter->codec_id);
    // 根据解码器参数创建解码器内容
    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecContext, codecParameter);
    avcodec_open2(codecContext, codec, NULL);
    // 用于帧计数
    int i = 0;
    auto pkt = av_packet_alloc();
    av_new_packet(pkt, codecContext->width * codecContext->height);
    AVFrame *videoFrame = av_frame_alloc();
    AVFrame *rgbFrame = av_frame_alloc();
    auto SwsCtx = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt, codecContext->width, codecContext->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_stream_idx) {
            i++;
            int got_picture_ptr = 0;
            auto ret = avcodec_send_packet(codecContext, pkt);
            // 重置pkt内容
            av_packet_unref(pkt);
            if (ret != 0) {
                continue;
            }

            ret = avcodec_receive_frame(codecContext, videoFrame);
            if (ret != 0) {
                spdlog::error("avcodec_receive_frame failed");
                continue;
            }

            int size = av_image_get_buffer_size(AV_PIX_FMT_BGR24, codecContext->width, codecContext->height, 1);
            spdlog::info("frame size {}", size);

            auto out_bufferRGB = new uint8_t[size];
            av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, out_bufferRGB, AV_PIX_FMT_BGR24, codecContext->width, codecContext->height, 1);
            spdlog::info("av_image_fill_arrays success");

            sws_scale(SwsCtx, videoFrame->data, videoFrame->linesize, 0, codecContext->height, rgbFrame->data, rgbFrame->linesize);
            spdlog::info("sws scale success");

            cv::Mat mat(codecContext->height, codecContext->width, CV_8UC3, rgbFrame->data[0], rgbFrame->linesize[0]);
            spdlog::info("memcpy success");

            spdlog::info("create map success {},\n\n{}.\n", videoFrame->width, rgbFrame->width);

            // 保存到图片，路径是output/i.png
            try {
                cv::imwrite("output/"s + std::to_string(i) + ".png", mat);
            } catch (std::exception &e) {
                spdlog::error("{}", e.what());
            }
            spdlog::info("Save frame {}.\n", i);
        }
    }
    spdlog::info("There are {} frames int total.\n", i);
    return;
}

// ffmpeg取流
void ffmepg_pull_stream() {
    auto logfield = spdlog::logger("ffmpeg"s);
    auto uri = "some url"s;
    avformat_network_init();
    auto formatContext = avformat_alloc_context();
    AVDictionary *options = nullptr;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "max_delay", "500000", 0);
    av_dict_set(&options, "stimeout", "5000000", 0);
    av_dict_set(&options, "buffer_size", "4096000", 0);
    if (avformat_open_input(&formatContext, uri.c_str(), NULL, &options) != 0) {
        logfield.error("open input failed");
        return;
    }
}

// ffmpeg 推流
void ffmpeg_push_stream() {
    auto logfield = spdlog::stdout_color_mt("ffmpeg");

    // 输入文件
    auto fileAddress = "./test.mp4"s;

    // 推流地址
    auto *rtmpAddress = "rtsp://172.17.106.250:8554/test";

    // 初始化网络库
    avformat_network_init();

    // 输入流处理部分
    AVFormatContext *inputFormatContext = nullptr;
    //打开文件
    auto ret = avformat_open_input(&inputFormatContext, fileAddress.c_str(), NULL, NULL);
    if (ret < 0) {
        logfield->error("open input file failed");
    }
    logfield->info("open input file success");

    // 获取流信息
    ret = avformat_find_stream_info(inputFormatContext, NULL);
    if (ret != 0) {
        logfield->error("find stream info failed");
    }
    logfield->info("find stream info success");

    // 输出流处理部分
    AVFormatContext *outputFormatContext;
    // 创建输出上下文
    ret = avformat_alloc_output_context2(&outputFormatContext, NULL, "rtsp", rtmpAddress);
    if (ret < 0) {
        logfield->error("create output context failed");
        return;
    }
    logfield->info("create output context success");
    // 配置输出流
    for (int i = 0; i < inputFormatContext->nb_streams; i++) {
        // 根据输入流的编码器参数创建一个编码器
        const AVCodec *inputCodec = avcodec_find_encoder(inputFormatContext->streams[i]->codecpar->codec_id);
        if (inputCodec == nullptr) {
            logfield->error("find decoder failed");
            return;
        }
        auto codeContext = avcodec_alloc_context3(inputCodec);
        avcodec_parameters_to_context(codeContext, inputFormatContext->streams[i]->codecpar);
        avcodec_open2(codeContext, inputCodec, NULL);

        // 创建一个新的流
        AVStream *outStream = avformat_new_stream(outputFormatContext, inputCodec);
        ret = avcodec_parameters_copy(outStream->codecpar, inputFormatContext->streams[i]->codecpar);
        if (ret < 0) {
            logfield->error("copy codec parameters failed");
            return;
        }
        logfield->info("copy codec parameters success");
        outStream->codecpar->codec_tag = 0;
    }

    // 打印输出流信息
    av_dump_format(outputFormatContext, 0, rtmpAddress, 1);

    // 打开io
    ret = avio_open2(&outputFormatContext->pb, rtmpAddress, AVIO_FLAG_WRITE, NULL, NULL);
    if (ret < 0) {
        logfield->error("open io failed");
    }
    logfield->info("open io success");

    // 推流每一帧数据
    AVPacket *packet = av_packet_alloc();
    long long startTime = av_gettime();

    ret = avformat_write_header(outputFormatContext, NULL);
    if (ret < 0) {
        logfield->error("Error occurred when opening output URL\n");
    }

    while (true) {
        ret = av_read_frame(inputFormatContext, packet);
        if (ret < 0) {
            logfield->error("read frame failed");
            continue;
        }
        logfield->info("read frame success");
        logfield->info("pts:{}", packet->pts);
        // 计算转换时间戳
        // 获取时间基数
        AVRational itime = inputFormatContext->streams[packet->stream_index]->time_base;
        AVRational otime = outputFormatContext->streams[packet->stream_index]->time_base;
        packet->pts = av_rescale_q_rnd(packet->pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
        packet->dts = av_rescale_q_rnd(packet->dts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
        // 到这一帧经历了多长时间
        packet->duration = av_rescale_q(packet->duration, itime, otime);
        packet->pos = -1;
        // 视频帧推送速度
        if (inputFormatContext->streams[packet->stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVRational tb = inputFormatContext->streams[packet->stream_index]->time_base;
            // 已经过去的时间
            long long curTime = av_gettime() - startTime;
            long long dts = 0;
            dts = packet->dts * (1000 * 1000 * av_q2d(tb));
            if (dts > curTime) {
                av_usleep(dts - curTime);
            }
        }
        try {
            ret = av_interleaved_write_frame(outputFormatContext, packet);
            if (ret < 0) {
                logfield->error("Error muxing packet\n");
                continue;
            }
        } catch (std::exception &e) {
            logfield->error("{}", e.what());
            continue;
        }
    }
    av_packet_free(&packet);
}
/* static void process_client(AVIOContext *client, const char *in_uri) {
     //     //     AVIOContext *input = NULL;https://www.ffmpeg.org/doxygen/3.1/dsicin_8c_source.html#l00096
     //     //     uint8_t *resource = NULL;
     //     //     int ret, n, reply_code;
     //     //     uint8_t *resource = NULL;
     //     //     while ((ret = avio_handshake(client)) > 0) {
     //     //         av_opt_get(client, "resource", AV_OPT_SEARCH_CHILDREN, &resource);
     //     //         // check for strlen(resource) is necessary,because av_opt_get()
     //     //         // may retuen empty string
     //     //         if (resource && strlen((const char *)resource)) {
     //     //             break;
     //     //         }
     //     //         av_freep(&resource);
     //     //     }
     //     //     if (ret < 0) {
     //     //         goto end;
     //     //     }
     //     //     av_log(client, AV_LOG_TRACE, "resource=%p\n", resource);
     //     //     if (resource && resource[0] == '/' && !strcmp((const char *)(resource + 1), in_uri)) {
     //     //         reply_code = 200;
     //     //     } else {
     //     //         reply_code = AVERROR_BSF_NOT_FOUND;
     //     //     }
     //     //     if ((ret == av_opt_set_int(client, "reply_code", reply_code, AV_OPT_SEARCH_CHILDREN)) < 0) {
     //     //         av_log(client, AV_LOG_ERROR, "Failed to set reply_code: %s. \n", av_err2str(ret));
     //     //         goto end;
     //     //     }
     //     //     while ((ret = avio_handshake(client)) > 0)
     //     //         ;
     //     //     if (ret < 0) {
     //     //         goto end;
     //     //     }

//     //     fprintf(stderr, "Handshake performed.\n");
//     //     if (reply_code != 200) {
//     //         goto end;
//     //     }
//     //     fprintf(stderr, "Open input file.\n");
//     //     if ((ret = avio_open2(&input, in_uri, AVIO_FLAG_READ, NULL, NULL)) < 0) {
//     //         av_log(client, AV_LOG_ERROR, "Failed to open input: %s: %s. \n", in_uri, av_err2str(ret));
//     //     }
//     // end:
//     return;
// }
// #include <iostream>
// void stream() {
//     const char *url;
//     AVFormatContext *fmtContext = nullptr;
//     avformat_open_input(&fmtContext, url, nullptr, nullptr);
//     avformat_find_stream_info(fmtContext, nullptr);
//     auto ret = av_find_best_stream(fmtContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
// }

// void streamMedia() {
//     int ret;
//     const char *input_file;
//     // 格式上下文
//     AVFormatContext *input_fmt_ctx = NULL;
//     if ((ret = avformat_open_input(&input_fmt_ctx, input_file, NULL, NULL) < 0)) {
//         std::cout << "error";
//     }
//     ret = avformat_find_stream_info(input_fmt_ctx, NULL);
//     if (ret < 0) {
//     }
//     // 匹配最佳的流
//     ret = av_find_best_stream(input_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
//     if (ret < 0) {
//     }
//     int stream_index_video = ret;
// #define NEED_MP4TOANNEXB_FILTER
// #ifdef NEED_MP4TOANNEXB_FILTER
//     const AVBitStreamFilter *bsf = av_bsf_get_by_name("h264_mp4toannexb");
//     AVBSFContext *bsf_ctx;
//     av_bsf_alloc(bsf, &bsf_ctx);
//     // 这里必须添加，否则AVPacket处理可能无效
//     avcodec_parameters_copy(bsf_ctx->par_in, input_fmt_ctx->streams[stream_index_video]->codecpar);
//     av_bsf_init(bsf_ctx);
// #endif

//     // 保存图像裸流数据到文件
//     FILE *fp = fopen("out.h264", "wb");
//     if (!fp) {
//         av_log(NULL, AV_LOG_ERROR, "Cannot open file\n");
//         return;
//     }

//     uint64_t frame_index = 0;
//     AVPacket *pkt = av_packet_alloc();
//     while (1) {
//         if (ret = av_read_frame(input_fmt_ctx, pkt) < 0) break;

//         // 保存视频
//         if (pkt->stream_index == stream_index_video) {
// #ifdef NEED_MP4TOANNEXB_FILTER
//             if (av_bsf_send_packet(bsf_ctx, pkt) == 0) {
//                 while (av_bsf_receive_packet(bsf_ctx, pkt) == 0) {
//                     fwrite(pkt->data, 1, pkt->size, fp);
//                 }
//             }
// #else
//             fwrite(pkt->data, 1, pkt->size, fp);
// #endif
//             printf("save frame: %llu , time: %lld\n", ++frame_index, av_gettime() / 1000);  // ms
//         }

//         av_packet_unref(pkt);
//     }

//     // 关闭输入
//     avformat_close_input(&input_fmt_ctx);
//     av_packet_free(&pkt);
//     fclose(fp);

// #ifdef NEED_MP4TOANNEXB_FILTER
//     av_bsf_free(&bsf_ctx);
// #endif
// }

// int pullStream() {
//     int status_error_ = -1;
//     std::string videourl = "";
//     AVFormatContext *pFormatCtx = nullptr;
//     AVDictionary *options = nullptr;
//     AVPacket *av_packet = nullptr;

//     // 执行网络库的全局初始化
//     // 此函数仅用于解决旧版GNUTLS或OPENSSL库的线程安全问题
//     // 一旦删除较旧的GUNTLS和OPENSSL库的支持，此函数将被启用，并且此函数不再有任何用户
//     avformat_network_init();

//     // 设置缓存大小
//     av_dict_set(&options, "buffer_size", "4096000", 0);
//     // 设置以tcp的方式打开rtsp
//     av_dict_set(&options, "rtsp_transport", "tcp", 0);
//     // 设置超时断开链接时间
//     av_dict_set(&options, "stimeout", "5000000", 0);
//     // 设置最大时延
//     av_dict_set(&options, "max_delay", "500000", 0);

//     // 用来申请AVFormatContex的空间
//     pFormatCtx = avformat_alloc_context();

//     // 打开网络流或文件流
//     if (avformat_open_input(&pFormatCtx, videourl.c_str(), NULL, &options) != 0) {
//         std::cout << "Couldn't open input stream.\n" << std::endl;
//         return status_error_;
//     }

//     // 打开视频文件信息
//     if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
//         std::cout << "Couldn't find stream information." << std::endl;
//         return status_error_;
//     }

//     std::cout << "av_dict_get:" << std::endl;
//     AVDictionaryEntry *tag = NULL;
//     while ((tag = av_dict_get(pFormatCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
//         std::string key = tag->key;
//         std::string value = tag->value;
//         std::cout << "av_dict_get:" << key << ":" << value << std::endl;
//     }

//     // 查找码流中是否有视频流
//     int videoindex = -1;
//     unsigned i = 0;
//     for (i = 0; i < pFormatCtx->nb_streams; i++) {
//         if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//             videoindex = i;
//             break;
//         }
//     }
//     if (videoindex == -1) {
//         std::cout << "Didn't find a video stream.\n" << std::endl;

//         return status_error_;
//     }

//     av_packet = (AVPacket *)av_malloc(sizeof(AVPacket));

//     while (true) {
//         if (av_read_frame(pFormatCtx, av_packet) >= 0) {
//             if (av_packet->stream_index == videoindex) {
//                 std::cout << "\ndata size is:" << av_packet->size << av_packet->data;
//                 //这里就是接收到的未解码之前的数据
//             }
//             if (av_packet != NULL) av_packet_unref(av_packet);
//         }
//     }

//     av_free(av_packet);
//     avformat_close_input(&pFormatCtx);
// }
*/
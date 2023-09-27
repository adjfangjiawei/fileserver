#include <libavcodec/avcodec.h>
#include <libavcodec/bsf.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <unistd.h>

static void process_client(AVIOContext *client, const char *in_uri) {
    //     AVIOContext *input = NULL;
    //     uint8_t *resource = NULL;
    //     int ret, n, reply_code;
    //     uint8_t *resource = NULL;
    //     while ((ret = avio_handshake(client)) > 0) {
    //         av_opt_get(client, "resource", AV_OPT_SEARCH_CHILDREN, &resource);
    //         // check for strlen(resource) is necessary,because av_opt_get()
    //         // may retuen empty string
    //         if (resource && strlen((const char *)resource)) {
    //             break;
    //         }
    //         av_freep(&resource);
    //     }
    //     if (ret < 0) {
    //         goto end;
    //     }
    //     av_log(client, AV_LOG_TRACE, "resource=%p\n", resource);
    //     if (resource && resource[0] == '/' && !strcmp((const char *)(resource + 1), in_uri)) {
    //         reply_code = 200;
    //     } else {
    //         reply_code = AVERROR_BSF_NOT_FOUND;
    //     }
    //     if ((ret == av_opt_set_int(client, "reply_code", reply_code, AV_OPT_SEARCH_CHILDREN)) < 0) {
    //         av_log(client, AV_LOG_ERROR, "Failed to set reply_code: %s. \n", av_err2str(ret));
    //         goto end;
    //     }
    //     while ((ret = avio_handshake(client)) > 0)
    //         ;
    //     if (ret < 0) {
    //         goto end;
    //     }

    //     fprintf(stderr, "Handshake performed.\n");
    //     if (reply_code != 200) {
    //         goto end;
    //     }
    //     fprintf(stderr, "Open input file.\n");
    //     if ((ret = avio_open2(&input, in_uri, AVIO_FLAG_READ, NULL, NULL)) < 0) {
    //         av_log(client, AV_LOG_ERROR, "Failed to open input: %s: %s. \n", in_uri, av_err2str(ret));
    //     }
    // end:
    return;
}
#include <iostream>
void stream() {
    const char *url;
    AVFormatContext *fmtContext = nullptr;
    avformat_open_input(&fmtContext, url, nullptr, nullptr);
    avformat_find_stream_info(fmtContext, nullptr);
    auto ret = av_find_best_stream(fmtContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
}

void streamMedia() {
    int ret;
    const char *input_file;
    // 格式上下文
    AVFormatContext *input_fmt_ctx = NULL;
    if ((ret = avformat_open_input(&input_fmt_ctx, input_file, NULL, NULL) < 0)) {
        std::cout << "error";
    }
    ret = avformat_find_stream_info(input_fmt_ctx, NULL);
    if (ret < 0) {
    }
    // 匹配最佳的流
    ret = av_find_best_stream(input_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0) {
    }
    int stream_index_video = ret;
#define NEED_MP4TOANNEXB_FILTER
#ifdef NEED_MP4TOANNEXB_FILTER
    const AVBitStreamFilter *bsf = av_bsf_get_by_name("h264_mp4toannexb");
    AVBSFContext *bsf_ctx;
    av_bsf_alloc(bsf, &bsf_ctx);
    // 这里必须添加，否则AVPacket处理可能无效
    avcodec_parameters_copy(bsf_ctx->par_in, input_fmt_ctx->streams[stream_index_video]->codecpar);
    av_bsf_init(bsf_ctx);
#endif

    // 保存图像裸流数据到文件
    FILE *fp = fopen("out.h264", "wb");
    if (!fp) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open file\n");
        return;
    }

    uint64_t frame_index = 0;
    AVPacket *pkt = av_packet_alloc();
    while (1) {
        if (ret = av_read_frame(input_fmt_ctx, pkt) < 0) break;

        // 保存视频
        if (pkt->stream_index == stream_index_video) {
#ifdef NEED_MP4TOANNEXB_FILTER
            if (av_bsf_send_packet(bsf_ctx, pkt) == 0) {
                while (av_bsf_receive_packet(bsf_ctx, pkt) == 0) {
                    fwrite(pkt->data, 1, pkt->size, fp);
                }
            }
#else
            fwrite(pkt->data, 1, pkt->size, fp);
#endif
            printf("save frame: %llu , time: %lld\n", ++frame_index, av_gettime() / 1000);  // ms
        }

        av_packet_unref(pkt);
    }

    // 关闭输入
    avformat_close_input(&input_fmt_ctx);
    av_packet_free(&pkt);
    fclose(fp);

#ifdef NEED_MP4TOANNEXB_FILTER
    av_bsf_free(&bsf_ctx);
#endif
}

int pullStream() {
    int status_error_ = -1;
    std::string videourl = "";
    AVFormatContext *pFormatCtx = nullptr;
    AVDictionary *options = nullptr;
    AVPacket *av_packet = nullptr;

    // 执行网络库的全局初始化
    // 此函数仅用于解决旧版GNUTLS或OPENSSL库的线程安全问题
    // 一旦删除较旧的GUNTLS和OPENSSL库的支持，此函数将被启用，并且此函数不再有任何用户
    avformat_network_init();

    // 设置缓存大小
    av_dict_set(&options, "buffer_size", "4096000", 0);
    // 设置以tcp的方式打开rtsp
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    // 设置超时断开链接时间
    av_dict_set(&options, "stimeout", "5000000", 0);
    // 设置最大时延
    av_dict_set(&options, "max_delay", "500000", 0);

    // 用来申请AVFormatContex的空间
    pFormatCtx = avformat_alloc_context();

    // 打开网络流或文件流
    if (avformat_open_input(&pFormatCtx, videourl.c_str(), NULL, &options) != 0) {
        std::cout << "Couldn't open input stream.\n" << std::endl;
        return status_error_;
    }

    // 打开视频文件信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        std::cout << "Couldn't find stream information." << std::endl;
        return status_error_;
    }

    std::cout << "av_dict_get:" << std::endl;
    AVDictionaryEntry *tag = NULL;
    while ((tag = av_dict_get(pFormatCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        std::string key = tag->key;
        std::string value = tag->value;
        std::cout << "av_dict_get:" << key << ":" << value << std::endl;
    }

    // 查找码流中是否有视频流
    int videoindex = -1;
    unsigned i = 0;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    }
    if (videoindex == -1) {
        std::cout << "Didn't find a video stream.\n" << std::endl;

        return status_error_;
    }

    av_packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    while (true) {
        if (av_read_frame(pFormatCtx, av_packet) >= 0) {
            if (av_packet->stream_index == videoindex) {
                std::cout << "\ndata size is:" << av_packet->size << av_packet->data;
                //这里就是接收到的未解码之前的数据
            }
            if (av_packet != NULL) av_packet_unref(av_packet);
        }
    }

    av_free(av_packet);
    avformat_close_input(&pFormatCtx);
}
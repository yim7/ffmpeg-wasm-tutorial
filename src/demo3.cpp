#include <emscripten.h>
#include <emscripten/bind.h>
#include <inttypes.h>

#include <string>
#include <vector>

using namespace emscripten;

extern "C" {
#include <libavformat/avformat.h>
};

typedef struct Response {
    int64_t duration;
    int64_t dts;
    int64_t pts;
} Response;

std::vector<Response>
run(std::string filename) {
    // Open the file and read header.

    AVFormatContext *fmt_ctx = avformat_alloc_context();
    int ret;
    if ((ret = avformat_open_input(&fmt_ctx, filename.c_str(), NULL, NULL)) < 0) {
        printf("%s", av_err2str(ret));
        return {};
    }

    // Read container data.
    printf("format: %s, duration: %lld us, streams: %d\n", fmt_ctx->iformat->name, fmt_ctx->duration,
           fmt_ctx->nb_streams);

    avformat_find_stream_info(fmt_ctx, NULL);

    int video_stream_index = -1;
    for (size_t i = 0; i < fmt_ctx->nb_streams; i++) {
        AVStream *stream = fmt_ctx->streams[i];
        AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
        if (codec != NULL && codec->type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        } else {
            stream->discard = AVDISCARD_ALL;
        }
    }

    if (video_stream_index == -1) {
        return {};
    }

    std::vector<Response> v = {};
    AVPacket *packet = av_packet_alloc();
    while (av_read_frame(fmt_ctx, packet) == 0) {
        if (packet->stream_index != video_stream_index) {
            continue;
        }

        Response r = {
            .duration = packet->duration,
            .dts = packet->dts,
            .pts = packet->pts,
        };
        v.push_back(r);
    }

    avformat_free_context(fmt_ctx);
    av_packet_free(&packet);
    return v;
}

EMSCRIPTEN_BINDINGS(structs) {
    value_object<Response>("Response")
        .field("duration", &Response::duration)
        .field("dts", &Response::dts)
        .field("pts", &Response::pts);

    register_vector<Response>("ResponseArray");

    function("run", &run);
}
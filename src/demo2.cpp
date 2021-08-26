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
    int64_t start_time;
    std::string codec;
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

    std::vector<Response> v = {};
    for (size_t i = 0; i < fmt_ctx->nb_streams; i++) {
        AVStream *stream = fmt_ctx->streams[i];
        AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
        if (codec == NULL) {
            printf("not found codec\n");
            return {};
        }

        Response r = {
            .duration = stream->duration,
            .start_time = stream->start_time,
            .codec = codec->name,
        };
        v.push_back(r);
    }

    avformat_free_context(fmt_ctx);
    return v;
}

EMSCRIPTEN_BINDINGS(structs) {
    value_object<Response>("Response")
        .field("duration", &Response::duration)
        .field("start_time", &Response::start_time)
        .field("codec", &Response::codec);

    register_vector<Response>("ResponseArray");

    function("run", &run);
}
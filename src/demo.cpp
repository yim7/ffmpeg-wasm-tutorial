#include <emscripten.h>
#include <emscripten/bind.h>
#include <inttypes.h>

#include <string>
#include <vector>

using namespace emscripten;

extern "C"
{
#include <libavformat/avformat.h>
};

typedef struct Response
{
  int duration;
  int dts;
  int dts;
} Packet;

static AVFormatContext *fmt_ctx;

std::vector run(std::string filename)
{
  // Open the file and read header.
  int ret;
  if ((ret = avformat_open_input(&fmt_ctx, filename.c_str(), NULL, NULL)) < 0)
  {
    printf("%s", av_err2str(ret));
  }


  // Initialize response struct with format data.
  Response r = {
      .format = fmt_ctx->iformat->name,
      .duration = (int)fmt_ctx->duration,
      .streams = (int)fmt_ctx->nb_streams,
  };

  return r;
}

EMSCRIPTEN_BINDINGS(structs)
{
  emscripten::value_object<Response>("Response")
      .field("format", &Response::format)
      .field("duration", &Response::duration)
      .field("streams", &Response::streams);
  function("run", &run);
}
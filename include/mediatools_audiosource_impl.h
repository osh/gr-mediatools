

#define __STDC_CONSTANT_MACROS

#include <string>
#include <vector>
#include <map>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


#ifndef CODEC_TYPE_AUDIO
#define CODEC_TYPE_AUDIO AVMEDIA_TYPE_AUDIO
#endif 


class mediatools_audiosource_impl {
    public:
        int d_type;
        bool d_ready;
        std::string d_filename;

        mediatools_audiosource_impl();
        bool open(std::string);
        void close();
        void readData(std::vector<int16_t> &r);
    
        AVFormatContext* d_format_ctx;
        AVCodecContext* d_codec_ctx;
        AVCodec *d_codec;
        AVFrame *d_frame;
        AVPacket d_packet;

        std::map<std::string, std::string> d_meta;
        int d_nfail;
};

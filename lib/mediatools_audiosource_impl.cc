#include <mediatools_audiosource_impl.h>
#include <string.h>
#include <assert.h>
#include <boost/thread.hpp>

// switch to enable debugging output
//#define DEBUG(x)    x
#define DEBUG(x)    

static boost::mutex avcodec_guard;

mediatools_audiosource_impl::mediatools_audiosource_impl(){
    d_ready = false;
    d_codec = NULL;
    d_frame = NULL;
    d_codec_ctx = NULL;
    d_format_ctx = NULL;
    av_register_all();   
    d_nfail = 0;
}

bool mediatools_audiosource_impl::open(std::string filename){
    boost::mutex::scoped_lock lock(avcodec_guard);
    d_nfail = 0;

    // close & free old contexts
    if(d_frame != NULL){ av_free(d_frame); d_frame=NULL; }
    if(d_codec_ctx != NULL){ avcodec_close(d_codec_ctx); av_free(d_codec_ctx); d_codec_ctx=NULL; }
    if(d_format_ctx != NULL){ av_free(d_format_ctx); d_format_ctx = NULL; }

    d_filename = filename;
    d_format_ctx = avformat_alloc_context(); // should be automatic
    int rc = avformat_open_input(&d_format_ctx, d_filename.c_str(), NULL, NULL);
    DEBUG(printf("filename = %s\n", filename.c_str());)
    if(rc<0){ printf("could not open file %s\n",filename.c_str()); return false; }
    //dump_format(d_format_ctx, 0, filename.c_str(), 0);

    // make sure we have audio
    bool have_audio = false;
    for(int stream_idx = 0; stream_idx < d_format_ctx->nb_streams; stream_idx++){
        DEBUG(printf("Stream: %d\n", stream_idx);)
        d_codec_ctx = d_format_ctx->streams[stream_idx]->codec;
        if(d_codec_ctx->codec_type == CODEC_TYPE_AUDIO){
            have_audio = true;
            break;
            }
    }
    if(!have_audio){
        DEBUG(printf("no audio stream found in file\n");)
        return false;
    }
    
    // allocate the right codec
    d_codec = avcodec_find_decoder(d_codec_ctx->codec_id);
    DEBUG(printf("using codec: %s\n", d_codec->name);)
    rc = avcodec_open2(d_codec_ctx, d_codec, NULL);
    if(rc<0){
        DEBUG(printf("could not open codec: %d\n",rc);)
        assert(0);
        }

    // get metadata - store it for the block
    AVDictionaryEntry *tag = NULL;
    while ((tag = av_dict_get(d_format_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))){
        //printf("%s=%s\n", tag->key, tag->value);
        d_meta[tag->key] = tag->value;
    }

 
    // open the file for reading
//    d_file = fopen(d_filename.c_str(), "rb");

    DEBUG(printf("file open and ready\n");)
    d_ready = true;
    return true;
     
}

void mediatools_audiosource_impl::close() {
    // Also frees the context
    avformat_close_input(&d_format_ctx);
}

void mediatools_audiosource_impl::readData(std::vector<int16_t> &r){
    // set up packet
    av_init_packet(&d_packet);
    int rf = av_read_frame(d_format_ctx, &d_packet);
    if(rf != 0){ d_ready = false; return; } // end of file ?
    // ensure output frame is allocated
    if(!d_frame)
        d_frame = avcodec_alloc_frame();

    // decode the frame
    int got_frame = 0;
    int rc = avcodec_decode_audio4(d_codec_ctx, d_frame, &got_frame, &d_packet);   

    // handle error
    if(rc<0){
        printf("error decoding file\n");
        if(d_nfail++ > 4) d_ready = false;
        return;
        }
    d_nfail = 0;
    //printf("avcodec_decode_audio4 -> %d (got frame = %d)\n", rc, got_frame);
    
    // get output size
    int data_size=0;
    if(got_frame){
        data_size = av_samples_get_buffer_size(NULL, 
                d_codec_ctx->channels,
                d_frame->nb_samples,
                d_codec_ctx->sample_fmt, 1);
        }

    // ship the output samples back out
    int16_t* x = (int16_t*) d_frame->data[0];
    int samples_out = data_size/sizeof(short);
    std::copy( x, x+samples_out, back_inserter(r) );
    return;
}



#include <mediatools_audiosource_impl.h>
#include <string.h>
#include <assert.h>

// switch to enable debugging output
//#define DEBUG(x)    x
#define DEBUG(x)    

mediatools_audiosource_impl::mediatools_audiosource_impl(){
    d_ready = false;
    
    avcodec_init();
    av_register_all();   

    d_frame = NULL;
    d_codec_ctx = NULL;
    d_format_ctx = NULL;

}


bool mediatools_audiosource_impl::open(std::string filename){

    // close & free old contexts
    if(d_frame != NULL){ av_free(d_frame); d_frame=NULL; }
    if(d_codec_ctx != NULL){ avcodec_close(d_codec_ctx); av_free(d_codec_ctx); d_codec_ctx=NULL; }
    if(d_format_ctx != NULL){ av_free(d_format_ctx); d_format_ctx = NULL; }

    d_filename = filename;

    int rc = av_open_input_file(&d_format_ctx, filename.c_str(), NULL, 0, NULL);
    DEBUG(printf("filename = %s\n", filename.c_str());)
    if(rc<0){ DEBUG(printf("could not open file.\n");) return false; }

    DEBUG(dump_format(d_format_ctx, 0, filename.c_str(), 0);)

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
    
    d_codec = avcodec_find_decoder(d_codec_ctx->codec_id);
    DEBUG(printf("using codec: %s\n", d_codec->name);)
    rc = avcodec_open(d_codec_ctx, d_codec);
    if(rc<0){
        DEBUG(printf("could not open codec: %d\n",rc);)
        assert(0);
        }
    d_frame = avcodec_alloc_frame();
 
    DEBUG(printf("file open and ready\n");)
    d_ready = true;
    return true;
     
}


void mediatools_audiosource_impl::readData(std::vector<int16_t> &r){
    uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3)/2] __attribute__ ((aligned (16)));;
    int audio_buf_size = 0;
    int audio_buf_index = 0;
    int data_size = (AVCODEC_MAX_AUDIO_FRAME_SIZE*4);

    int rc = av_read_frame(d_format_ctx, &d_packet);
    
    // make sure we can read another frame
    if(rc < 0){
        // otherwise move on from this file
        d_ready = false;
        return;
    }
 
    //d_packet->data = audio_buf;
    //d_packet->size =   
    
    // decode the data frame and check return code
    //int audio_size = avcodec_decode_audio2(d_codec_ctx, (int16_t*)audio_buf, &data_size, d_packet.data, d_packet.size);   
    int got_frame_ptr;
    int audio_size = avcodec_decode_audio4(d_codec_ctx, d_frame, &got_frame_ptr, &d_packet);   
    if(audio_size < 0){ 
        // ignore non-audio / erronious frame
        return;
    }

    // copy output data into our buffer
    int16_t* x = (int16_t*) audio_buf;
    int samples_out = data_size/sizeof(short);

    std::vector<int16_t> bufwrapper(x, x+samples_out);
    std::copy( x, x+samples_out, back_inserter(r) );

    return;
}

bool mediatools_audiosource_impl::open_mpeg(){
}



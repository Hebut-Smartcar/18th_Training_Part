/*
 * @Name       core
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-03
 */


#include "inputstream.h"


#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL;//, *audio_dec_ctx;
static int width, height;
static enum AVPixelFormat pix_fmt;
static AVStream *video_stream = NULL;//, *audio_stream = NULL;
static const char *src_filename = NULL;
//static const char *video_dst_filename = NULL;
//static const char *audio_dst_filename = NULL;
static FILE *video_dst_file = NULL;
//static FILE *audio_dst_file = NULL;

static uint8_t *video_dst_data[4] = {NULL};
static int      video_dst_linesize[4];
static int video_dst_bufsize;

static int video_stream_idx = -1;//, audio_stream_idx = -1;
static AVFrame *frame = NULL;
static AVPacket pkt;
//static int video_frame_count = 0;
//static int audio_frame_count = 0;


static int64_t total_frames_count=0;
static int main_got_frame;
static AVFrame *tmp_frame;
static struct SwsContext *sws_ctx;
#define SCALE_FLAGS SWS_BICUBIC

static unsigned char *r_byte_;
static unsigned char *g_byte_;
static unsigned char *b_byte_;

static void *caller_p_;

#define UNUSED(x) (void)x

static void (*read_callback_)(void *caller_p);


void cvt_to_rgb24() {
    if (!sws_ctx) {
         sws_ctx = sws_getContext(width, height,
                                          pix_fmt, //src format
                                          width, height,
                                          AV_PIX_FMT_RGB24, //des format
                                          SCALE_FLAGS, NULL, NULL, NULL);
        if (!sws_ctx) {
            fprintf(stderr,
            "Could not initialize the conversion context\n");
            exit(1);
         }
     }

    sws_scale(sws_ctx,
              (const uint8_t * const *) frame->data, frame->linesize, //src frame
              0, height,
              tmp_frame->data, tmp_frame->linesize); //des frame


    unsigned char *src_data = tmp_frame->data[0]; //AV_PIX_FMT_RGB24 array


    int line_size = tmp_frame->linesize[0];
    //tmp_frame data to my frame data
    int src_base_index;
    int des_base_index;

    for(int y = 0; y < height; y ++)
        for(int x = 0; x < width; x ++) {

            des_base_index = width * y + x;
            src_base_index = line_size *y + x*3;

            r_byte_[des_base_index] = src_data[src_base_index +0];
            g_byte_[des_base_index] = src_data[src_base_index +1];
            b_byte_[des_base_index] = src_data[src_base_index +2];
        }
}

/* Enable or disable frame reference counting. You are not supposed to support
 * both paths in your application but pick the one most appropriate to your
 * needs. Look for the use of refcount in this example to see what are the
 * differences of API usage between them. */
static int refcount = 0;

static int decode_packet(int *got_frame, int cached)
{
    int ret = 0;
    int decoded = pkt.size;

    UNUSED(cached);

    *got_frame = 0;


    if (pkt.stream_index == video_stream_idx) {
        // decode video frame
        ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding video frame (%s)\n", av_err2str(ret));
            return ret;
        }

        if (*got_frame) {

            if (frame->width != width || frame->height != height ||
                frame->format != pix_fmt) {
                //To handle this change, one could call av_image_alloc again and
                 // decode the following frames into another rawvideo file.
                fprintf(stderr, "Error: Width, height and pixel format have to be "
                        "constant in a rawvideo file, but the width, height or "
                        "pixel format of the input video changed:\n"
                        "old: width = %d, height = %d, format = %s\n"
                        "new: width = %d, height = %d, format = %s\n",
                        width, height, av_get_pix_fmt_name(pix_fmt),
                        frame->width, frame->height,
                        av_get_pix_fmt_name(frame->format));
                return -1;
            }

         /*   printf("video_frame%s n:%d coded_n:%d\n",
                   cached ? "(cached)" : "",
                   video_frame_count++, frame->coded_picture_number);
                   */
            cvt_to_rgb24();
        }
    }

    // If we use frame reference counting, we own the data and need
     // to de-reference it when we don't use it anymore
    if (*got_frame && refcount)
        av_frame_unref(frame);





    return decoded;
}

static int open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), src_filename);
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders, with or without reference counting */
        av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
        if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }

    return 0;
}

static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    int ret;
    picture = av_frame_alloc();
    if (!picture)
        return NULL;
    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;
    /* allocate the buffers for the frame data */

    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }
    return picture;
}


int open_import_file(char *src_file_name) {

    int ret = 0;
    int ret_value = 0;

    src_filename = src_file_name;

    /* open input file, and allocate format context */
    if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", src_filename);
        ret = 1;
    }

    /* retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        ret = 2;
    }

    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];

        total_frames_count = video_stream->nb_frames;
        printf("video nb_frames: %lld\n", total_frames_count);

        /* allocate image where the decoded image will be put */
        width = video_dec_ctx->width;
        height = video_dec_ctx->height;
        pix_fmt = video_dec_ctx->pix_fmt;
        ret_value = av_image_alloc(video_dst_data, video_dst_linesize,
                             width, height, pix_fmt, 1);
        if (ret_value < 0) {
            fprintf(stderr, "Could not allocate raw video buffer\n");
            ret = 3;
        }
        video_dst_bufsize = ret_value;
    }

    /* dump input information to stderr */
    av_dump_format(fmt_ctx, 0, src_filename, 0);

    if (!video_stream) {
        fprintf(stderr, "Could not find video stream in the input, aborting\n");
        ret = 4;
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        //ret = AVERROR(ENOMEM);
        ret = 5;
    }

    tmp_frame = alloc_picture(AV_PIX_FMT_RGB24, width, height);
    if (!tmp_frame) {
        fprintf(stderr, "Could not allocate temporary picture\n");
        ret = 6;
    }

    return ret;
}

void get_video_info(int64_t *frame_count, int *width1, int *height1) {
    *frame_count = total_frames_count;
    *width1 = width;
    *height1 = height;
}

int read_a_frame(unsigned char *r_byte1,
                             unsigned char *g_byte1,
                             unsigned char *b_byte1) {

    r_byte_ = r_byte1;
    g_byte_ = g_byte1;
    b_byte_ = b_byte1;

    int ret = 0;

    // initialize packet, set data to NULL, let the demuxer fill it
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    // read frames from the file
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        do {
            ret = decode_packet(&main_got_frame, 0); //one pkg  contains only one video frame
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;

            //callback
            read_callback_(caller_p_);

        } while (pkt.size > 0);

        av_packet_unref(&orig_pkt);

    }
    return ret;
}

int close_import_file() {

    int ret = 0;

    /* flush cached frames */
    pkt.data = NULL;
    pkt.size = 0;
    do {
        decode_packet(&main_got_frame, 1);
    } while (main_got_frame);


    avcodec_free_context(&video_dec_ctx);
    avformat_close_input(&fmt_ctx);
    if (video_dst_file)
        fclose(video_dst_file);
    av_frame_free(&frame);
    av_free(video_dst_data[0]);

    av_frame_free(&tmp_frame);
    sws_freeContext(sws_ctx);
    sws_ctx = 0;

    return ret;
}

void set_read_callback(void *caller_p, void (*read_callback)( void *caller_p)) {
    caller_p_ = caller_p;
    read_callback_ = read_callback;

}


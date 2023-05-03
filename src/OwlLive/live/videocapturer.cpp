#include "videocapturer.h"
#include "dlog.h"
#include "timeutil.h"
#include "avtimebase.h"
namespace LIVE {

//Show Dshow Device
static void show_dshow_device() {
  AVFormatContext* pFormatCtx = avformat_alloc_context();
  AVDictionary* options = nullptr;
  av_dict_set(&options, "list_devices", "true", 0);
  AVInputFormat* iformat = av_find_input_format("dshow");
  printf("========Device Info=============\n");
  avformat_open_input(&pFormatCtx, "video=dummy", iformat, &options);
  printf("================================\n");
}

//Show Dshow Device Option
static void show_dshow_device_option() {
  AVFormatContext* pFormatCtx = avformat_alloc_context();
  AVDictionary* options = nullptr;
  av_dict_set(&options, "list_options", "true", 0);
  AVInputFormat* iformat = av_find_input_format("dshow");
  printf("========Device Option Info======\n");
  avformat_open_input(&pFormatCtx, "video=Integrated Camera", iformat, &options);
  printf("================================\n");
}
VideoCapturer::VideoCapturer() {

}

VideoCapturer::~VideoCapturer() {
  Stop();
  if (video_frame_) {
    av_frame_free(&video_frame_);
  }
  if (yuv_frame_) {
    av_frame_free(&yuv_frame_);
  }
  if (fmt_ctx_) {
    avformat_close_input(&fmt_ctx_);
    fmt_ctx_ = nullptr;
  }
  if (codec_ctx_) {
    avcodec_free_context(&codec_ctx_);
    codec_ctx_ = nullptr;
  }
}

RET_CODE VideoCapturer::Init(const Properties& properties) {
  device_name_ = properties.GetProperty("video_device_name", "");

  video_enc_width_ = properties.GetProperty("video_enc_width", 0);
  video_enc_height_ = properties.GetProperty("video_enc_height", 0);
  video_enc_pix_fmt_ = properties.GetProperty("video_enc_pix_fmt", -1);
  capture_fps_ = properties.GetProperty("fps", 25);

  if (video_enc_width_ == 0
      || video_enc_height_ == 0
      || video_enc_pix_fmt_ == -1
      || capture_fps_ == 0) {
    LogError("AudioCapturer init properties have null parameter, please check it");
    return RET_FAIL;
  }
  if (device_name_.empty()) {
    LogError("VideoCapturer init failed, device_name is null");
    return RET_FAIL;
  }

  x_ = properties.GetProperty("x", 0);
  y_ = properties.GetProperty("y", 0);


  AVCodec*  pCodec = nullptr;

  fmt_ctx_ = avformat_alloc_context();
#if defined(WIN32)

  //Show Dshow Device
  show_dshow_device();
  //Show Device Options
  show_dshow_device_option();


  AVInputFormat* ifmt = av_find_input_format("dshow"); //使用dshow

  if (avformat_open_input(&fmt_ctx_, device_name_.c_str(), ifmt, nullptr) != 0) {
    fprintf(stderr, "Couldn't open input stream video.（无法打开输入流）\n");
    return RET_FAIL;
  }
#else

#endif

  video_stream_ = -1;
  codec_ctx_   = nullptr;

  for (unsigned int i = 0; i < fmt_ctx_->nb_streams; i++) {
    if (fmt_ctx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_ = static_cast<int>(i);
      break;
    }
  }

  if (video_stream_ == -1) {
    printf("Didn't find a video stream.（没有找到视频流）\n");
    return RET_FAIL;
  }

//    pCodecCtx = pFormatCtx->streams[videoStream]->codec;

  //find the decoder
  codec_ctx_ = avcodec_alloc_context3(nullptr);
  avcodec_parameters_to_context(codec_ctx_, fmt_ctx_->streams[video_stream_]->codecpar);

  pCodec = avcodec_find_decoder(codec_ctx_->codec_id);

  if (pCodec == nullptr) {
    printf("video Codec not found.\n");
    return RET_FAIL;
  }

  if (avcodec_open2(codec_ctx_, pCodec, nullptr) < 0) {
    printf("Could not open video codec.\n");
    return RET_FAIL;
  }

  video_frame_ = av_frame_alloc();
  yuv_frame_ = av_frame_alloc();


  return RET_OK;
}


static AVFrame* alloc_picture(enum AVPixelFormat pix_fmt, int width, int height) {
  AVFrame* picture;
  int ret;

  picture = av_frame_alloc();
  if (!picture)
    return NULL;

  picture->format = pix_fmt;
  picture->width  = width;
  picture->height = height;

  /* allocate the buffers for the frame data */
  ret = av_frame_get_buffer(picture, 0);
  if (ret < 0) {
    fprintf(stderr, "Could not allocate frame data.\n");
    exit(1);
  }

  return picture;
}

void VideoCapturer::Loop() {
  LogInfo("into loop");

  struct SwsContext* img_convert_ctx = nullptr;

  int y_size = 0;
  int yuv_size = 0;
  int64_t pts = 0;
  if (codec_ctx_) {
    ///将数据转成YUV420P格式
    img_convert_ctx = sws_getContext(codec_ctx_->width, codec_ctx_->height, (enum AVPixelFormat)codec_ctx_->pix_fmt,
                                     video_enc_width_, video_enc_height_, (enum AVPixelFormat)video_enc_pix_fmt_,
                                     SWS_BICUBIC, nullptr, nullptr, nullptr);

    yuv_frame_ = alloc_picture(AV_PIX_FMT_YUV420P, video_enc_width_, video_enc_height_);
  }

  AVPacket packet;

  int64_t firstTime =  TimesUtil::GetTimeMillisecond();


  LogInfo("into loop while");
  while (true) {
    if (request_exit_)
      break;
    if (av_read_frame(fmt_ctx_, &packet) < 0) {
      fprintf(stderr, "read failed! \n");
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
      continue;
    }

    if (!is_first_frame_) {
      is_first_frame_ = true;
      LogInfo("%s:t%u", AVPublishTime::GetInstance()->getVInTag(),
              AVPublishTime::GetInstance()->getCurrenTime());
    }
    if (packet.stream_index == video_stream_) {
      pts = AVPublishTime::GetInstance()->get_video_pts();
      if (avcodec_send_packet(codec_ctx_, &packet) != 0) {
        fprintf(stderr, "input AVPacket to decoder failed!\n");
        av_packet_unref(&packet);
        continue;
      }

      while (0 == avcodec_receive_frame(codec_ctx_, video_frame_)) {
        /// 转换成YUV420
        /// 由于解码后的数据不一定是yuv420p，比如硬件解码后会是yuv420sp，因此这里统一转成yuv420p
        sws_scale(img_convert_ctx, (const uint8_t* const*)video_frame_->data, video_frame_->linesize, 0, codec_ctx_->height, yuv_frame_->data, yuv_frame_->linesize);
        yuv_frame_->width = video_frame_->width;
        yuv_frame_->height = video_frame_->height;
        yuv_frame_->format = AV_PIX_FMT_YUV420P;
        yuv_frame_->pts = pts;      //TimesUtil::GetTimeMillisecond() - firstTime;
        // pts += 33;      // 帧率叠加
        callback_get_frame_(yuv_frame_);
      }
    } else {
      fprintf(stderr, "other %d \n", packet.stream_index);
    }

    av_packet_unref(&packet);

    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }

}
}

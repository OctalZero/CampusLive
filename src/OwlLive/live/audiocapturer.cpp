#include "audiocapturer.h"

#include "dlog.h"
#include "timeutil.h"
#include "avtimebase.h"
namespace LIVE {

//Show Dshow Device
//static void show_dshow_device() {
//  AVFormatContext* pFormatCtx = avformat_alloc_context();
//  AVDictionary* options = nullptr;
//  av_dict_set(&options, "list_devices", "true", 0);
//  AVInputFormat* iformat = av_find_input_format("dshow");
//  printf("========Device Info=============\n");
//  avformat_open_input(&pFormatCtx, "video=dummy", iformat, &options);
//  printf("================================\n");
//}

//Show Dshow Device Option
//static void show_dshow_device_option() {
//  AVFormatContext* pFormatCtx = avformat_alloc_context();
//  AVDictionary* options = nullptr;
//  av_dict_set(&options, "list_options", "true", 0);
//  AVInputFormat* iformat = av_find_input_format("dshow");
//  printf("========Device Option Info======\n");
//  avformat_open_input(&pFormatCtx, "video=Integrated Camera", iformat, &options);
//  printf("================================\n");
//}



AudioCapturer::AudioCapturer() {

}

AudioCapturer::~AudioCapturer() {
  Stop();
  if (pcm_buf_)
    delete [] pcm_buf_;
  if (audio_resampler_) {
    audio_resampler_free(audio_resampler_);
    audio_resampler_ = nullptr;
  }
  if (pFormatCtx) {
    avformat_close_input(&pFormatCtx);
    pFormatCtx = nullptr;
  }
  if (aCodecCtx) {
    avcodec_free_context(&aCodecCtx);
    aCodecCtx = nullptr;
  }
}

RET_CODE AudioCapturer::Init(const Properties& properties) {
  audio_enc_sample_rate_ = properties.GetProperty("audio_enc_sample_rate", 0);
  audio_enc_sample_fmt_ = properties.GetProperty("audio_enc_sample_fmt", 0);
  audio_enc_channel_layout_ = properties.GetProperty("audio_enc_channel_layout", 0);
  audio_enc_frame_size_ = properties.GetProperty("audio_enc_frame_size", 0);
  device_name_ = properties.GetProperty("audio_device_name", "");

  if (audio_enc_sample_rate_ == 0
      || audio_enc_sample_fmt_ == 0
      || audio_enc_channel_layout_ == 0
      || audio_enc_frame_size_ == 0) {
    LogError("AudioCapturer init properties have null parameter, please check it");
    return RET_FAIL;
  }
  if (device_name_.empty()) {
    LogError("AudioCapturer init failed, device_name is null");
    return RET_FAIL;
  }


  pcm_buf_ = new uint8_t[PCM_BUF_MAX_SIZE];
  if (!pcm_buf_) {
    return RET_ERR_OUTOFMEMORY;
  }

  AVCodec*     pCodec = nullptr;

  pFormatCtx = avformat_alloc_context();
#if defined(WIN32)
  //Show Dshow Device
//  show_dshow_device();
  //Show Device Options
//  show_dshow_device_option();

  AVInputFormat* ifmt = av_find_input_format("dshow"); //使用dshow
  AVDictionary* param = NULL;
//    set audio device buffer latency size in milliseconds
  av_dict_set(&param, "audio_buffer_size", "25", 0);      // 设置20ms
//    av_dict_set(&param, "sample_size", "5", 0);
  int ret = avformat_open_input(&pFormatCtx, device_name_.c_str(), ifmt, &param);
  if (ret != 0) {
    av_strerror(ret, err2str, sizeof(err2str));
    LogError("Couldn't open input audio stream, ret:%d, err2str:%s", ret, err2str);
    return RET_FAIL;
  }

#else
//    assert();
#endif

  audio_stream_ = -1;
  aCodecCtx   = nullptr;

  for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
    if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
      audio_stream_ = static_cast<int>(i);
      break;
    }
  }

  if (audio_stream_ == -1) {
    printf("Didn't find a audio stream.（没有找到音频流）\n");
    return RET_FAIL;
  }

//    pCodecCtx = pFormatCtx->streams[videoStream]->codec;

  //find the decoder
  aCodecCtx = avcodec_alloc_context3(nullptr);
  avcodec_parameters_to_context(aCodecCtx, pFormatCtx->streams[audio_stream_]->codecpar);

  pCodec = avcodec_find_decoder(aCodecCtx->codec_id);

  if (pCodec == nullptr) {
    printf("audio Codec not found.\n");
    return RET_FAIL;
  }

  if (avcodec_open2(aCodecCtx, pCodec, nullptr) < 0) {
    printf("Could not open audio codec.\n");
    return RET_FAIL;
  }

  ///解码音频相关
  audio_frame_ = av_frame_alloc();
  initResample();
  return RET_OK;
}




void AudioCapturer::Loop() {
  LogInfo("into loop");

  AVPacket packet;
  audio_pts_ = 0;
  int64_t out_pts = 0;
  pcm_total_duration_ = 0;
  pcm_start_time_ = TimesUtil::GetTimeMillisecond();
  LogInfo("into loop while");
  AVRational tb = {1, aCodecCtx->sample_rate};
  int64_t pts = 0;
  int64_t pre_pts = 0;
  int64_t pre_pts_base = 0;
  while (true) {
    if (request_exit_)
      break;

    if (av_read_frame(pFormatCtx, &packet) < 0) {
      fprintf(stderr, "read failed! \n");
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
      continue;
    }

    if (!is_first_frame_) {
      is_first_frame_ = true;
      LogInfo("%s:t%u", AVPublishTime::GetInstance()->getAInTag(),
              AVPublishTime::GetInstance()->getCurrenTime());

    }

    if (packet.stream_index == audio_stream_) {

      if (int ret = avcodec_send_packet(aCodecCtx, &packet) && ret != 0) {
        char buffer[1024] = {0};
        av_strerror(ret, buffer, 1024);
        fprintf(stderr, "input AVPacket to decoder failed! ret = %d %s\n", ret, buffer);
      } else {
        while (1) {
          int ret = avcodec_receive_frame(aCodecCtx, audio_frame_);
          if (ret != 0) {
            //            char buffer[1024] = {0};
            //            av_strerror(ret, buffer, 1024);
            //            fprintf(stderr, "avcodec_receive_frame = %d %s\n", ret, buffer);
            break;
          }

          ///解码一帧后才能获取到采样率等信息，因此将初始化放到这里
          if (frame_resample_ == nullptr) {
            frame_resample_ = av_frame_alloc();

            frame_resample_->format = audio_enc_sample_fmt_;
            frame_resample_->channel_layout = audio_enc_channel_layout_;
            frame_resample_->sample_rate = audio_enc_sample_rate_;
            frame_resample_->nb_samples = audio_enc_frame_size_;  // aac

            ret = av_frame_get_buffer(frame_resample_, 0);
            if (ret < 0) {
              fprintf(stderr, "Error allocating an audio buffer\n");
              break;
            }
          }

          int ret_size = audio_resampler_send_frame2(audio_resampler_, audio_frame_->data, audio_frame_->nb_samples, audio_pts_);
          audio_pts_ += audio_frame_->nb_samples;
          do {
            ret_size = audio_resampler_receive_frame2(audio_resampler_, frame_resample_->data, audio_enc_frame_size_, &out_pts);
            if (ret_size > 0) {
              pts = AVPublishTime::GetInstance()->get_audio_pts();
//                            printf("in_pts:%ldms  ", TimesUtil::GetTimeMillisecond() - pcm_start_time_ );
//                            printf("out_pts:%ldms ",  int64_t(out_pts * av_q2d(tb) *1000));
//                            printf("ret_size:%d\n",  ret_size);
              frame_resample_->pts = pts;
              LogInfo("out_pts:%ums, delta:%ums", (uint32_t)frame_resample_->pts, (uint32_t)(frame_resample_->pts - pre_pts));
              pre_pts = frame_resample_->pts;
//                            pts+= (1024*1000/audio_enc_sample_rate_);
              callback_get_frame_(frame_resample_);
            } else {
              //                        printf("can't get %d samples, ret_size:%d, cur_size:%d\n", audio_enc_frame_size_, ret_size,
              //                               audio_resampler_get_fifo_size(audio_resampler_));
              break;
            }
          } while (1);

        }
      }
    } else {
      fprintf(stderr, "other %d \n", packet.stream_index);
    }

    av_packet_unref(&packet);

    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
  LogInfo("Audio capture thread leave");
}



bool AudioCapturer::initResample() {
  //重采样设置选项-----------------------------------------------------------start
  frame_resample_ = nullptr;

  /// 由于ffmpeg编码aac需要输入FLTP格式的数据。
  /// 因此这里将音频重采样成44100 双声道  AV_SAMPLE_FMT_FLTP
  //重采样设置选项----------------
  //输入的采样格式
  capture_sample_fmt_ = aCodecCtx->sample_fmt;
  //输入的采样率
  capture_sample_rate_ = aCodecCtx->sample_rate;
  //输入的声道布局
  capture_channel_layout_ = aCodecCtx->channel_layout;


  if (capture_channel_layout_ <= 0) {
    if (aCodecCtx->channels == 2) {
      capture_channel_layout_ = AV_CH_LAYOUT_STEREO;
    } else {
      capture_channel_layout_ = AV_CH_LAYOUT_MONO;
    }
  }

  // 重采样实例
  audio_resampler_params_t resampler_params;
  resampler_params.src_channel_layout = capture_channel_layout_;
  resampler_params.src_sample_fmt     = (enum AVSampleFormat)capture_sample_fmt_;
  resampler_params.src_sample_rate    =  capture_sample_rate_;

  resampler_params.dst_channel_layout = audio_enc_channel_layout_;  // 2通道
  resampler_params.dst_sample_fmt     = (enum AVSampleFormat)audio_enc_sample_fmt_;
  resampler_params.dst_sample_rate    = audio_enc_sample_rate_;

  audio_resampler_  = audio_resampler_alloc(resampler_params);

  if (audio_resampler_)
    return true;
  else {
    LogError("audio_resampler_alloc failed");
    return false;
  }
}
}


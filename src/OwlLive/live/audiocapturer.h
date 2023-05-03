#ifndef AUDIOCAPTURER_H
#define AUDIOCAPTURER_H
#include <functional>
#include "commonlooper.h"
#include "mediabase.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

#ifdef __cplusplus
}
#endif
#include "audioresampler.h"

#define USE_DSHOW 1


namespace LIVE {
using std::function;

class AudioCapturer : public CommonLooper {
 public:
  AudioCapturer();
  virtual ~AudioCapturer();
  /**
   * @brief Init
   * @param "audio_test": 缺省为0，为1时数据读取本地文件进行播放
   *        "input_pcm_name": 测试模式时读取的文件路径
   *        "sample_rate": 采样率
   *        "channels": 采样通道
   *        "sample_fmt": 采样格式
   * @return
   */
  RET_CODE Init(const Properties& properties);

  bool initResample();
  virtual void Loop();
//    void AddCallback(function<void(uint8_t*, int32_t)> callback)
//    {
//        callback_get_pcm_ = callback;
//    }

  void AddCallback(function<void(AVFrame*)> callback) {
    callback_get_frame_ = callback;
  }

 private:
  // 初始化参数
  int audio_test_ = 0;

  // PCM file只是用来测试, 写死为s16格式 2通道 采样率48Khz
  // 1帧1024采样点持续的时间21.333333333333333333333333333333ms

  int64_t pcm_start_time_ = 0;    // 起始时间
  double pcm_total_duration_ = 0;        // PCM读取累计的时间
  FILE* pcm_fp_ = NULL;


  function<void(AVFrame*)> callback_get_frame_ = NULL;
  uint8_t* pcm_buf_;
  int32_t pcm_buf_size_;
  const int PCM_BUF_MAX_SIZE = 32768; //

  bool is_first_frame_ = false;


  int audio_enc_sample_rate_ = 0;
  int audio_enc_sample_fmt_ = 0;
  uint64_t audio_enc_channel_layout_ = 0;
  int audio_enc_frame_size_ = 0;

  int capture_sample_rate_ = 0;
  int capture_sample_fmt_ = 0;
  uint64_t capture_channel_layout_ = 0;
  int capture_frame_size_ = 0;


  // 重采样相关
  float audio_pts_;

  ///以下变量用于音频重采样
  /// 由于ffmpeg解码出来后的pcm数据有可能是带平面的pcm，因此这里统一做重采样处理，
  /// 重采样成44100的16 bits 双声道数据(AV_SAMPLE_FMT_S16)
  AVFrame* frame_resample_;


  audio_resampler_t*  audio_resampler_ = nullptr;


  // 音频采集相关
  std::string device_name_;
  AVFormatContext* pFormatCtx;
  int             audio_stream_;
  AVCodecContext*  aCodecCtx;

  AVFrame* audio_frame_;

  char err2str[256] = {0};
};
}
#endif // AUDIOCAPTURER_H

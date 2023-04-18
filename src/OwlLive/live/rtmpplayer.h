// RTMP播放器
#ifndef RTMPPLAYER_H
#define RTMPPLAYER_H
#include <functional>
#include <thread>
#include <vector>
#include "rtmpbase.h"
#include "mediabase.h"


namespace LIVE {

/**
 * @brief The RTMPPlayer class
 * 1.把接收的数据包封装成AVPacket
 */
class RTMPPlayer : public RTMPBase {
 public:
  RTMPPlayer();
  virtual ~RTMPPlayer();
  RET_CODE Start();  // 开始播放
  void Stop();  // 停止播放
  void*  readPacketThread();
  // 收到音频数据包调用回调, 是否清空队列也有pullwork进行
  void AddAudioInfoCallback(std::function<void(int what, MsgBaseObj* data, bool flush)> callback) {
    audio_info_callback_ = callback;
  }
  // 收到视频数据包调用回调
  void AddVideoInfoCallback(std::function<void(int what, MsgBaseObj* data, bool flush)> callback) {
    video_info_callback_ = callback;
  }
  // 解析音频数据包后的回调函数
  void AddAudioPacketCallback(std::function<void(void*)> callback) {
    audio_packet_callable_object_ = callback;
  }
  // 解析视频数据包后的回调函数
  void AddVideoPacketCallback(std::function<void(void*)> callback) {
    video_packet_callable_object_ = callback;
  }

 private:

  void parseScriptTag(RTMPPacket& packet);
  bool request_exit_thread_ = false;
  std::thread* worker_ = NULL;
  std::function<void(int what, MsgBaseObj* data, bool flush)> audio_info_callback_ = NULL;
  std::function<void(int what, MsgBaseObj* data, bool flush)> video_info_callback_ = NULL;

  std::function<void(void*)> audio_packet_callable_object_ = NULL;
  std::function<void(void*)> video_packet_callable_object_ = NULL;
 private:
  //video and audio info
  int video_codec_id = 0;
  int video_width = 0;
  int video_height = 0;
  int video_frame_rate = 0;
  int audio_codec_id = 0;
  int audio_sample_rate = 0;
  int audio_bit_rate = 0;
  int audio_sample_size = 0;
  int audio_channel = 2;
  int file_size = 0;

  uint32_t video_frame_duration_ = 40; // 默认是40毫秒
  uint32_t audio_frame_duration_ = 21; // 默认是21毫秒 aac 48kh

  uint8_t profile_ = 0;
  uint8_t sample_frequency_index_ = 0;
  uint8_t channels_ = 0;
  std::vector<std::string> sps_vector_;    // 可以存储多个sps
  std::vector<std::string> pps_vector_;    // 可以存储多个pps

  int64_t audio_pre_pts_ = -1;
  int64_t video_pre_pts_ = -1;

  // 性能指标统计
  uint32_t PRINT_MAX_FRAMES = 30;    // 打印前xx帧，根据实际调试情况进行修改，如果打印太多影响性能
  bool is_got_metadta_ = false;
  bool is_got_video_sequence_ = false;    // video sequence
  bool is_got_video_iframe_ = false; //打印第一次收到i帧
  uint32_t got_video_frames_ = 0;     // 对接收的video帧进行计数, 打印前xx帧的时间

  bool is_got_audio_sequence_ = false;
  uint32_t got_audio_frames_ = 0;

  bool firt_entry = false;  // 是否第一次进入
};
}


#endif // RTMPPLAYER_H

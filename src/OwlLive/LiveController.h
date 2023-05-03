// 直播控制器，负责直播功能与QML的交互
#ifndef LIVECONTROLLER_H
#define LIVECONTROLLER_H

#include <QObject>
#include "live/pullwork.h"
#include "live/pushwork.h"
#include "live/mediabase.h"
#include "live/qpainterdrawable.h"
#include "Client.h"

extern "C" {
#include "libavdevice/avdevice.h"
}

class LiveController : public QPainterDrawable {
  Q_OBJECT

 public:
  explicit LiveController(QPainterDrawable* parent = 0);
  ~LiveController();
  int DrawVideo(const Frame* video_frame);
  int EventCallback(int what, int arg1, int arg2, int arg3, void* data);
  // 向服务器发送进入课堂的消息
  Q_INVOKABLE void JoinClass(const QString& courseId);
  // 拉流
  Q_INVOKABLE void PullStream(const QString& streamUrl);
  // 推流
  Q_INVOKABLE void PushStream(const QString& streamUrl, const QString& videoDevice, const QString& audioDevice);
  // 更新课堂成员列表
  Q_INVOKABLE void UpdateClassList(const QString& courseId);
  // 退出课堂
  Q_INVOKABLE void ExitClass(const QString& courseId);
  // 考勤签到
  Q_INVOKABLE void Attendance(const QString& courseId, int time);
  // 确认签到
  Q_INVOKABLE void ConfirmAttendanceResult(const QString& courseId);
  // 查找音视频设备
  Q_INVOKABLE void FindDevices();
  // 属性相关
  Q_INVOKABLE QString getStreamAddress();
  Q_INVOKABLE QString getCourseId();
  Q_INVOKABLE QVariantList getClassMembers();
  Q_INVOKABLE QVariantList getAbsentMembers();
  Q_INVOKABLE QVariantList getVideoDevices();
  Q_INVOKABLE QVariantList getAudioDevices();
  Q_INVOKABLE int getAudioCache();
  Q_INVOKABLE int getVideoCache();
  Q_INVOKABLE void setMaxCache(QString cache);
  Q_INVOKABLE void setJitter(QString jitter);
 signals:
  void sendToServer(const QString& message);
  void joinClassSuccess();
  void updateClassMembersSuccess();
  void updateAudioCache();
  void updateVideoCache();
  void showAttendanceResult();
  void confirmAttendance();
 private:
  Client* client_ = nullptr;
  std::string url_ = "";
  PullWork* pull_work_ = nullptr;
  PushWork* push_work_ = nullptr;
  QVariantList video_devices_;  // 视频设备列表
  QVariantList audio_devices_;  // 音频设备列表
  int max_cache_duration_ = 400;  // 最大缓冲区，默认400ms
  int network_jitter_duration_ = 100; // 网络抖动范围,默认100ms
  float accelerate_speed_factor_ = 1.2; // 默认加速是1.2
  float normal_speed_factor_ = 1.0;     // 正常播放速度1.0
  int audio_cache_ = 0;  // 音频缓存
  int video_cache_ = 0;  // 视频缓存
  float audio_bitrate_duration = 0;  // 码率
  float video_bitrate_duration = 0;
};

#endif // LIVECONTROLLER_H

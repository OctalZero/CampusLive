#include "LiveController.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QAudioDeviceInfo>
#include <QCameraInfo>
#include <QList>
#include "types.h"
using namespace LIVE;


LiveController::LiveController(QPainterDrawable* parent) :
  QPainterDrawable{parent} {
  client_ = &Client::GetClient();

  // 连接与客户端控制器的信号与槽
  connect(this, &LiveController::sendToServer, client_, &Client::sendToServer);
  connect(client_, &Client::updateClassMembersSuccess, this, &LiveController::updateClassMembersSuccess);
  connect(client_, &Client::showAttendanceResult, this, &LiveController::showAttendanceResult);
  connect(client_, &Client::confirmAttendance, this, &LiveController::confirmAttendance);
  connect(client_, &Client::joinClassSuccess, this, &LiveController::joinClassSuccess);
}

LiveController::~LiveController() {
  if (pull_work_) {
    delete pull_work_;
    pull_work_ = nullptr;
  }
  if (push_work_) {
    delete push_work_;
    push_work_ = nullptr;
  }
  qDebug() << "~LiveController()";
}

int LiveController::DrawVideo(const Frame* video_frame) {
  return draw(video_frame, false, false);
}

int LiveController::EventCallback(int what, int arg1, int arg2, int arg3, void* data) {
  switch (what) {
    case EVENT_UPDATE_AUDIO_CACHE_DURATION:
      audio_cache_ = arg1 + arg2;
      emit updateAudioCache();
      break;
    case EVENT_UPDATE_VIDEO_CACHE_DURATION:
      video_cache_ = arg1 + arg2;
      emit updateVideoCache();
      break;
    default:
      break;
  }
  return 0;
}

void LiveController::JoinClass(const QString& courseId) {
  QJsonObject j;
  j["type"] = MessageType::JoinClass;
  QJsonObject data;
  data["course_id"] = courseId;
  data["user_id"] = client_->getUserId();
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "JoinClass:" << message;
  emit sendToServer(message);
}

void LiveController::PullStream(const QString& streamUrl) {
  if (!pull_work_) {
    // 启动推流
    pull_work_ = new PullWork();
    // 读取url
    url_ = streamUrl.toStdString();
    qDebug() << "PullStream:" << streamUrl;

    Properties pull_properties;
    pull_properties.SetProperty("rtmp_url", url_);
    pull_properties.SetProperty("video_out_width", 1920);
    pull_properties.SetProperty("video_out_height", 1080);
    pull_properties.SetProperty("audio_out_sample_rate", 48000);

    pull_properties.SetProperty("network_jitter_duration", network_jitter_duration_);
    pull_properties.SetProperty("accelerate_speed_factor", accelerate_speed_factor_);
    pull_properties.SetProperty("max_cache_duration", max_cache_duration_);    //缓存时间

    pull_work_->AddVideoRefreshCallback(std::bind(&LiveController::DrawVideo, this,
                                                  std::placeholders::_1));
    pull_work_->AddEventCallback(std::bind(&LiveController::EventCallback, this,
                                           std::placeholders::_1,
                                           std::placeholders::_2,
                                           std::placeholders::_3,
                                           std::placeholders::_4,
                                           std::placeholders::_5));
    if (pull_work_->Init(pull_properties) != RET_OK) {
      LogError("pushwork.Init failed");
      delete pull_work_;
      pull_work_ = NULL;
      return ;
    }
  }
}

void LiveController::PushStream(const QString& streamUrl, const QString& videoDevice, const QString& audioDevice) {
  if (!push_work_) {
    // 启动推流
    push_work_ = new PushWork();
    // 读取url
    url_ = streamUrl.toStdString();
    std::cout << "PushStream:" << url_ << std::endl;

    Properties properties;
    // rtmp推流地址
    properties.SetProperty("rtmp_url", url_);

    std::string audio = audioDevice.toStdString();
    std::string video = videoDevice.toStdString();
    std::cout << "audio:" << audio << std::endl;
    std::cout << "video:" << video << std::endl;
    // audio采集设备名称
    properties.SetProperty("audio_device_name", audio);
    // video采集设备名称
    properties.SetProperty("video_device_name", video);


    // 音频编码属性 设置采样格式，码率，声道数量  固定使用aac编码
    properties.SetProperty("audio_sample_rate", 44100);
    properties.SetProperty("audio_channels", 2);
    properties.SetProperty("audio_bitrate", 64 * 1024);


    // 桌面录制属性 分辨率、帧率、码率、像素格式  固定使用h264编码
    properties.SetProperty("desktop_x", 0);
    properties.SetProperty("desktop_y", 0);
    properties.SetProperty("desktop_width", 1920); //屏幕分辨率
    properties.SetProperty("desktop_height", 1080);  // 屏幕分辨率
    properties.SetProperty("desktop_pixel_format", AV_PIX_FMT_YUV420P);
    properties.SetProperty("desktop_fps", 25);//测试模式时和yuv文件的帧率一致


    // 视频编码属性 视频的分辨率和可以桌面采集不一样
    properties.SetProperty("video_bitrate", 3 * 1024 * 1024); // 设置码率


    properties.SetProperty("rtmp_debug", 0);  // 必须设置，否则循环直接退出了
    if (push_work_->Init(properties) != RET_OK) {
      LogError("pushwork.Init failed");
      push_work_->DeInit();
      return;
    }
  }
  push_work_->Loop();
}

void LiveController::UpdateClassList(const QString& courseId) {
  QJsonObject j;
  j["type"] = MessageType::UpdateClassList;
  QJsonObject data;
  data["course_id"] = courseId;
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "UpdateClassList:" << message;
  emit sendToServer(message);
}

void LiveController::ExitClass(const QString& courseId) {
  QJsonObject j;
  j["type"] = MessageType::ExitClass;
  QJsonObject data;
  data["course_id"] = courseId;
  data["user_id"] = client_->getUserId();
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "ExitClass:" << message;
  emit sendToServer(message);
}

void LiveController::Attendance(const QString& courseId, int time) {
  QJsonObject j;
  j["type"] = MessageType::Attendance;
  QJsonObject data;
  data["course_id"] = courseId;
  data["time"] = time;
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "Attendance:" << message;
  emit sendToServer(message);
}

void LiveController::ConfirmAttendanceResult(const QString& courseId) {
  QJsonObject j;
  j["type"] = MessageType::ConfirmAttendanceResult;
  QJsonObject data;
  data["course_id"] = courseId;
  data["user_id"] = client_->getUserId();
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "ConfirmAttendanceResult:" << message;
  emit sendToServer(message);
}

void LiveController::FindDevices() {
  QString deviceName;
  QAudioDeviceInfo audioInfo;
  QCameraInfo videoInfo;
  QList<QAudioDeviceInfo> audioDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  for (int i = 0; i < audioDevices.count(); ++i) {
    audioInfo = audioDevices.at(i);
    deviceName = audioInfo.deviceName();
    qDebug() << "Audio device name: " << deviceName;
    audio_devices_.append(deviceName);
  }

  QList<QCameraInfo> videoDevices = QCameraInfo::availableCameras();
  for (int i = 0; i < videoDevices.count(); ++i) {
    videoInfo = videoDevices.at(i);
    deviceName = videoInfo.description();
    qDebug() << "Video device name: " << deviceName;
    video_devices_.append(deviceName);
  }
}

QString LiveController::getStreamAddress() {
  return client_->getStreamAddress();
}

QString LiveController::getCourseId() {
  return client_->getCourseId();
}

QVariantList LiveController::getClassMembers() {
  return client_->getClassMembers();
}

QVariantList LiveController::getAbsentMembers() {
  return client_->getAbsentMembers();
}

QVariantList LiveController::getVideoDevices() {
  return video_devices_;
}

QVariantList LiveController::getAudioDevices() {
  return audio_devices_;
}

int LiveController::getAudioCache() {
  return audio_cache_;
}

int LiveController::getVideoCache() {
  return video_cache_;
}

void LiveController::setMaxCache(QString cache) {
  cache = cache.remove("ms");
  network_jitter_duration_ = cache.toInt();
  if (pull_work_) {
    pull_work_->setMax_cache_duration(max_cache_duration_);
    LogInfo("max_cache_duration_ set to %dms", max_cache_duration_);
  }
}

void LiveController::setJitter(QString jitter) {
  jitter = jitter.remove("ms");
  network_jitter_duration_ = jitter.toInt();
  if (pull_work_) {
    pull_work_->setNetwork_jitter_duration(network_jitter_duration_);
    LogInfo("network_jitter_duration_ set to %dms", network_jitter_duration_);
  }
}



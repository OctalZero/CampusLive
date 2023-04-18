#include "LiveController.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/audio_fifo.h>
}
#include "live/pullwork.h"
#include <QJsonDocument>
#include <QJsonObject>
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
}

LiveController::~LiveController() {
  if (pull_work_) {
    delete pull_work_;
    pull_work_ = nullptr;
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

void LiveController::Broadcast(const QString& streamUrl) {
  if (!pull_work_) {
    // 启动推流
    pull_work_ = new PullWork();
    // 读取url
    if (streamUrl != "") {
      url_ = streamUrl.toStdString();
    } else {
      url_ = "rtmp://112.74.93.160/live/livestream";
    }
    qDebug() << "BroadcastUrl:" << streamUrl;
//    url_ = "rtmp://112.74.93.160/live/livestream";
//    url_ = "rtmp://120.78.82.230:1935/test/s";

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

QString LiveController::getCourseId() {
  return client_->getCourseId();
}

QVariantList LiveController::getClassMembers() {
  return client_->getClassMembers();
}

QVariantList LiveController::getAbsentMembers() {
  return client_->getAbsentMembers();
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



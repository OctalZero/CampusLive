#include "Client.h"
#include "Network.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "types.h"

using namespace std;

Client& Client::GetClient() {
  static Client client;   // 静态变量，多次访问只会创建一次

  return client;
}

Client::Client(QObject* parent)
  : QObject{parent} {
  courses_.clear();
  class_members_.clear();
  absent_members_.clear();
  class_playbacks_.clear();

  if (!network_)
    network_ = &Network::GetNetwork();
  // 连接与网络的信号与槽
  connect(network_, &Network::ServerMessage, this, &Client::ProcessServerMessage, Qt::QueuedConnection);
  connect(this, &Client::sendToServer, network_, &Network::Send, Qt::QueuedConnection);

  // 启动网络线程
  network_->start();
}

Client::~Client() {
  if (network_) {
    network_->quit();  // 停止线程事件循环
    network_->wait();  // 等待线程结束
    network_->deleteLater();  // 释放线程资源
  }
}

QString Client::getUserId() {
  return user_id_;
}

QString Client::getCourseId() {
  return course_id_;
}


QVariantList Client::getCourses() {
  return courses_;
}

QVariantList Client::getClassMembers() {
  return class_members_;
}

QVariantList Client::getAbsentMembers() {
  return absent_members_;
}

QVariantList Client::getClassPlaybacks() {
  return class_playbacks_;
}

void Client::setCourseId(const QString& courseId) {
  course_id_ = courseId;
}

void Client::setUserId(const QString& userId) {
  user_id_ = userId;
}

void Client::ProcessLoginResult(const QJsonObject& data) {
  QString ret = data["result"].toString();
  if (ret == "success") {
    emit loginSuccess();
    qDebug() << "login success";
  } else if (ret == "failed") {
    QString reason = data["reason"].toString();
    // TODO:界面显示失败原因
    qDebug() << reason;
  }

}

void Client::ProcessRegisterResult(const QJsonObject& data) {
  QString ret = data["result"].toString();
  if (ret == "success") {
    user_id_ = data["user_id"].toString();
    emit registerSuccess();
    qDebug() << "register success";
  } else if (ret == "failed") {
    // TODO:注册失败先不考虑
    QString reason = data["reason"].toString();
    qDebug() << reason;
  }
}

void Client::ProcessUpdateCoursesResult(const QJsonObject& data) {
  courses_.clear();
  QJsonArray array = data["courses"].toArray();
  QJsonArray::iterator it;
  for (it = array.begin(); it != array.end(); ++it) {
    QJsonObject course = (*it).toObject();
    QVariantMap c;
    c.insert("id", course["id"].toString());
    c.insert("name", course["name"].toString());
    c.insert("status", course["status"].toBool());
    c.insert("master", course["master"].toString());
    c.insert("stream_address", course["stream_address"].toString());
    courses_ .append(c);
  }

  emit updateCoursesSuccess();
}

void Client::ProcessCreateCourseResult(const QJsonObject& data) {
  QString ret = data["result"].toString();
  if (ret == "success") {
    ProcessUpdateCoursesResult(data);
  } else if (ret == "failed") {
    // TODO:创建失败先不考虑
  }
}

void Client::ProcessDeleteCourseResult(const QJsonObject& data) {
  QString ret = data["result"].toString();
  if (ret == "success") {
    ProcessUpdateCoursesResult(data);
  } else if (ret == "failed") {
    // TODO:创建失败先不考虑
  }
}

void Client::ProcessJoinCourseResult(const QJsonObject& data) {
  QString ret = data["result"].toString();
  if (ret == "success") {
    ProcessUpdateCoursesResult(data);
  } else if (ret == "failed") {
    // TODO:加入失败后显示失败消息
  }
}

void Client::ProcessDeselectionCourseResult(const QJsonObject& data) {
  QString ret = data["result"].toString();
  if (ret == "success") {
    ProcessUpdateCoursesResult(data);
  } else if (ret == "failed") {
    // TODO:退选失败先不考虑
  }
}

void Client::ProcessJoinClassResult(const QJsonObject& data) {
  ProcessUpdateClassListResult(data);
}

void Client::ProcessUpdateClassListResult(const QJsonObject& data) {
  QString ret = data["result"].toString();
  if (ret == "success") {
    class_members_.clear();
    QJsonArray array = data["members"].toArray();
    QJsonArray::iterator it;
    for (it = array.begin(); it != array.end(); ++it) {
      QJsonObject member = (*it).toObject();
      QVariantMap m;
      if (member["identification"].toString() == "student") {
        m.insert("identification", "学生");
        qDebug() << "学生";
      } else if (member["identification"].toString() == "teacher") {
        m.insert("identification", "教师");
        qDebug() << "教师";
      }
      m.insert("name", member["name"].toString());
      qDebug() << member["name"].toString();
      class_members_.append(m);
    }

    emit updateClassMembersSuccess();
  } else if (ret == "failed") {
    // TODO:更新失败先不考虑
  }

}

void Client::ProcessAttendanceResult(const QJsonObject& data) {
  QString ret = data["result"].toString();
  if (ret == "success") {
    absent_members_.clear();
    QJsonArray array = data["members"].toArray();
    QJsonArray::iterator it;
    for (it = array.begin(); it != array.end(); ++it) {
      QJsonObject member = (*it).toObject();
      QVariantMap m;
      m.insert("name", member["name"].toString());
      absent_members_.append(m);
    }

    emit showAttendanceResult();
  } else if (ret == "failed") {
    // TODO:考勤失败先不考虑
  }
}

void Client::ProcessDiscussResult(const QJsonObject& data) {
  emit discussResult(data["name"].toString(), data["message"].toString());
}

void Client::ProcessUpdatePlaybackListResult(const QJsonObject& data) {
  QString ret = data["result"].toString();
  if (ret == "success") {
    class_playbacks_.clear();
    QJsonArray array = data["playbacks"].toArray();
    QJsonArray::iterator it;
    for (it = array.begin(); it != array.end(); ++it) {
      QJsonObject playback = (*it).toObject();
      QVariantMap p;
      p.insert("fileName", playback["fileName"].toString());
      p.insert("url", playback["url"].toString());
      class_playbacks_.append(p);
    }

    emit updatePlaybackSuccess();
  } else if (ret == "failed") {
    // TODO:更新失败先不考虑
  }
}


void Client::ProcessServerMessage(const QString& message) {
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &error);

  if (error.error != QJsonParseError::NoError) {
    qDebug() << "Error: " << error.errorString();
    return;
  }
  QJsonObject j = doc.object();
  int type = j["type"].toInt();
  QJsonObject data = j["data"].toObject();
  qDebug() << "data:" << data;
  switch (type) {
    case MessageType::LoginResult :
      qDebug() << "ProcessLoginResult():" << data;
      ProcessLoginResult(data);
      break;
    case MessageType::RegisterResult :
      qDebug() << "ProcessRegisterResult():" << data;
      ProcessRegisterResult(data);
      break;
    case MessageType::UpdateCourseListResult:
      qDebug() << "ProcessUpdateCoursesResult():" << data;
      ProcessUpdateCoursesResult(data);
      break;
    case MessageType::CreateCourseResult:
      qDebug() << "ProcessCreateCourseResult():" << data;
      ProcessCreateCourseResult(data);
      break;
    case MessageType::DeleteCourseResult:
      qDebug() << "ProcessDeleteCourseResult():" << data;
      ProcessDeleteCourseResult(data);
      break;
    case MessageType::JoinCourseResult :
      qDebug() << "JoinCourseResult():" << data;
      ProcessJoinCourseResult(data);
      break;
    case MessageType::DeselectionCourseResult :
      qDebug() << "DeselectionCourseResult():" << data;
      ProcessDeselectionCourseResult(data);
      break;
    case MessageType::JoinClassResult :
      qDebug() << "JoinClassResult():" << data;
      ProcessJoinClassResult(data);
      break;
    case MessageType::UpdateClassListResult :
      qDebug() << "UpdateClassListResult():" << data;
      ProcessUpdateClassListResult(data);
      break;
    case MessageType::AttendanceResult :
      qDebug() << "AttendanceResult():" << data;
      ProcessAttendanceResult(data);
      break;
    case MessageType::ConfirmAttendance :
      qDebug() << "ConfirmAttendance():" << data;
      emit confirmAttendance();
      break;
    case MessageType::DiscussResult :
      qDebug() << "DiscussResult():" << data;
      ProcessDiscussResult(data);
      break;
    case MessageType::UpdatePlaybackListResult :
      qDebug() << "UpdatePlaybackListResult():" << data;
      ProcessUpdatePlaybackListResult(data);
      break;
    default:
      break;
  }

}

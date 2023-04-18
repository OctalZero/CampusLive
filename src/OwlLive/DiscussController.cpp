#include "DiscussController.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QGuiApplication>
#include <QClipboard>
#include "types.h"

DiscussController::DiscussController(QObject* parent)
  : QObject{parent} {
  client_ = &Client::GetClient();
  // 连接与客户端控制器的信号与槽
  connect(this, &DiscussController::sendToServer, client_, &Client::sendToServer);
  connect(client_, &Client::discussResult, this, &DiscussController::DiscussResult);
}

void DiscussController::SendDiscussInfo(const QString& courseId, const QString& message) {
  QJsonObject j;
  j["type"] = MessageType::Discuss;
  QJsonObject data;
  data["course_id"] = courseId;
  data["user_id"] = client_->getUserId();
  data["message"] = message;
  j["data"] = data;
  QJsonDocument doc(j);
  QString msg = QString::fromUtf8(doc.toJson());
  qDebug() << "SendDiscussInfo:" << msg;

  emit sendToServer(msg);
}

void DiscussController::clipText(const QString& text) {
  QClipboard* clipboard = QGuiApplication::clipboard();
  clipboard->setText(text);
}

const QString& DiscussController::getName() const {
  return name_;
}

const QString& DiscussController::getMessage() const {
  return message_;
}

void DiscussController::DiscussResult(const QString& name, const QString& message) {
  name_ = name;
  message_ = message;
  emit discussResult();
}

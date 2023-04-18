#include "PlaybackController.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "types.h"

PlaybackController::PlaybackController(QObject* parent)
  : QObject{parent} {
  client_ = &Client::GetClient();
  // 连接与客户端控制器的信号与槽
  connect(this, &PlaybackController::sendToServer, client_, &Client::sendToServer);
  connect(client_, &Client::updatePlaybackSuccess, this, &PlaybackController::updatePlaybackSuccess);
}

void PlaybackController::UpdatePlaybackList() {
  QJsonObject j;
  j["type"] = MessageType::UpdatePlaybackList;
  QJsonObject data;
  data["user_id"] = client_->getUserId();
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "UpdatePlaybackList:" << message;
  emit sendToServer(message);
}

QVariantList PlaybackController::getPlaybacks() {
  return client_->getClassPlaybacks();
}

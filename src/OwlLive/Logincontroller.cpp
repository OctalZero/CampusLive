#include "Logincontroller.h"
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include "Network.h"
#include "types.h"

LoginController::LoginController(QObject* parent)
  : QObject{parent} {
  client_ = &Client::GetClient();
  // 连接与客户端控制器的信号与槽
  connect(client_, &Client::loginSuccess, this, &LoginController::loginSuccess);
  connect(this, &LoginController::sendToServer, client_, &Client::sendToServer);
  connect(client_, &Client::registerSuccess, this, &LoginController::registerSuccess);
}

void LoginController::SendLoginInfo(const QString& userId, const QString& password) {
  QJsonObject j;
  j["type"] = MessageType::Login;
  QJsonObject data;
  data["user_id"] = userId;
  data["password"] = password;
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "LoginInfo:" << message;
  emit sendToServer(message);
}

void LoginController::SendRegisterInfo(const QString& name, const QString& password, const QString& id) {
  QJsonObject j;
  j["type"] = MessageType::Register;
  QJsonObject data;
  data["user_name"] = name;
  data["password"] = password;
  data["identification"] = id;
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "RegisterInfo:" << message;
  emit sendToServer(message);
}

void LoginController::setUserId(const QString& userId) {
  client_->setUserId(userId);
}

QString LoginController::getUserId() {
  return client_->getUserId();
}



#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H
#include <QObject>
#include "Client.h"

class LoginController : public QObject {
  Q_OBJECT
 public:
  explicit LoginController(QObject* parent = nullptr);
  // 向服务器发送登录的消息
  Q_INVOKABLE void SendLoginInfo(const QString& userId, const QString& password);
  // 向服务器发送注册的消息
  Q_INVOKABLE void SendRegisterInfo(const QString& name, const QString& password, const QString& id);
  // 属性相关
  Q_INVOKABLE void setUserId(const QString& userId);
  Q_INVOKABLE QString getUserId();
  Q_INVOKABLE QString getIdentification();
 signals:
  void sendToServer(const QString& message);
  void loginSuccess();
  void registerSuccess();
 private:
  Client* client_ = nullptr;
};

#endif // LOGINCONTROLLER_H

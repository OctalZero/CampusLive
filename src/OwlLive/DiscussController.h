#ifndef DISCUSSCONTROLLER_H
#define DISCUSSCONTROLLER_H
#include <QObject>
#include "Client.h"
#include <string>

class DiscussController : public QObject {
  Q_OBJECT
 public:
  explicit DiscussController(QObject* parent = nullptr);
  // 向服务器发送讨论区的消息
  Q_INVOKABLE void SendDiscussInfo(const QString& courseId, const QString& message);
  // 文本剪切板
  Q_INVOKABLE void clipText(const QString& text);
  // 属性相关
  Q_INVOKABLE const QString& getName() const;
  Q_INVOKABLE const QString& getMessage() const;
 signals:
  void sendToServer(const QString& message) ;
  void discussResult();
 public slots:
  void DiscussResult(const QString& name, const QString& message);
 private:
  Client* client_ = nullptr;
  QString name_ = "";
  QString message_ = "";
};

#endif // DISCUSSCONTROLLER_H

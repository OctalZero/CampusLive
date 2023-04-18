#ifndef PLAYBACKCONTROLLER
#define PLAYBACKCONTROLLER
#include <QObject>
#include "Client.h"

class PlaybackController : public QObject {
  Q_OBJECT
 public:
  explicit PlaybackController(QObject* parent = nullptr);
  // 向服务器发送更新课堂回放列表的消息
  Q_INVOKABLE void UpdatePlaybackList();
  // 属性相关
  Q_INVOKABLE QVariantList getPlaybacks();
 signals:
  void sendToServer(const QString& message);
  void updatePlaybackSuccess();
 private:
  Client* client_ = nullptr;
};

#endif // PLAYBACKCONTROLLER

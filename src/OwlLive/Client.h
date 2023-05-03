// 客户端总控制器，处理各个控制器之间的交互
#ifndef CLIENT_H
#define CLIENT_H
#include <QObject>
#include <QString>
#include <QVariantList>
#include "Network.h"
class QJsonObject;

class Client : public QObject {
  Q_OBJECT
 public:
  // 单例模式，获取单个实例
  static Client& GetClient();
  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;
  ~Client();

  // 属性相关
  QString getUserId();
  QString getCourseId();
  QString getStreamAddress();
  QString getIdentification();
  QVariantList getCourses();
  QVariantList getClassMembers();
  QVariantList getAbsentMembers();
  QVariantList getClassPlaybacks();
  void setCourseId(const QString& courseId);
  void setUserId(const QString& userId);
 private:
  explicit Client(QObject* parent = nullptr);
  // 处理登录结果
  void ProcessLoginResult(const QJsonObject& data);
  // 处理注册结果
  void ProcessRegisterResult(const QJsonObject& data);
  // 处理更新课程列表
  void ProcessUpdateCoursesResult(const QJsonObject& data);
  // 处理创建课程结果
  void ProcessCreateCourseResult(const QJsonObject& data);
  // 处理删除课程结果
  void ProcessDeleteCourseResult(const QJsonObject& data);
  // 处理加入课程结果
  void ProcessJoinCourseResult(const QJsonObject& data);
  // 处理退选课程结果
  void ProcessDeselectionCourseResult(const QJsonObject& data);
  // 处理进入课堂结果
  void ProcessJoinClassResult(const QJsonObject& data);
  // 处理更新课堂成员列表结果
  void ProcessUpdateClassListResult(const QJsonObject& data);
  // 处理考勤签到结果
  void ProcessAttendanceResult(const QJsonObject& data);
  // 处理讨论结果
  void ProcessDiscussResult(const QJsonObject& data);
  // 处理更新回放列表结果
  void ProcessUpdatePlaybackListResult(const QJsonObject& data);
 signals:
  // 发送消息给服务器
  void sendToServer(const QString& message);
  // 登录成功
  void loginSuccess();
  // 注册成功
  void registerSuccess();
  // 更新课程成功
  void updateCoursesSuccess();
  // 加入课堂成功
  void joinClassSuccess();
  // 更新课堂成员成功
  void updateClassMembersSuccess();
  // 显示考勤签到结果
  void showAttendanceResult();
  // 确认签到
  void confirmAttendance();
  // 课堂讨论结果
  void discussResult(const QString& name, const QString& message);
  //  更新回放列表成功
  void updatePlaybackSuccess();
 private slots:
  // 解析接收到的服务端的消息
  void ProcessServerMessage(const QString& message);

 private:
  Network* network_ = nullptr;
  QString user_id_ = "";
  QString identification_ = "";
  QString course_id_ = "";
  QString stream_address_ = "";
  QVariantList courses_;
  QVariantList class_members_;
  QVariantList absent_members_;
  QVariantList class_playbacks_;
};


#endif // CLIENT_H

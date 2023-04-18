#ifndef COURSECONTROLLER_H
#define COURSECONTROLLER_H

#include <QObject>
#include <QVariantList>
#include "Client.h"

class CourseController : public QObject {
  Q_OBJECT
 public:
  explicit CourseController(QObject* parent = nullptr);
  // 向服务器发送更新课程列表的消息
  Q_INVOKABLE void SendUpdateCourses();
  // 向服务器发送创建课程的消息
  Q_INVOKABLE void SendCreateInfo(const QString& courseName);
  // 向服务器发送删除课程的消息
  Q_INVOKABLE void DeleteCourse(const QString& courseId);
  // 向服务器发送加入课程的消息
  Q_INVOKABLE void SendJoinInfo(const QString& courseId);
  // 向服务器发送退选课程的消息
  Q_INVOKABLE void DeselectionCourse(const QString& courseId);
  // 向服务器发送进入课堂的消息
  Q_INVOKABLE void JoinClass(const QString& courseId);
  // 向服务器发送更新课堂成员列表的消息
  Q_INVOKABLE void UpdateClassList(const QString& courseId);
  // 属性相关
  Q_INVOKABLE QVariantList getCourses();
  Q_INVOKABLE void setCourseId(const QString& courseId);
 signals:
  void sendToServer(const QString& message);
  void updateCoursesSuccess();
 private:
  Client* client_ = nullptr;
};

#endif // COURSECONTROLLER_H

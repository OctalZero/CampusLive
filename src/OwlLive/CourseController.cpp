#include "CourseController.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "types.h"

CourseController::CourseController(QObject* parent)
  : QObject{parent} {
  client_ = &Client::GetClient();

  // 连接与客户端控制器的信号与槽
  connect(this, &CourseController::sendToServer, client_, &Client::sendToServer);
  connect(client_, &Client::updateCoursesSuccess, this, &CourseController::updateCoursesSuccess);
//  connect(client_, &Client::joinClassSuccess, this, &CourseController::joinClassSuccess);
}

void CourseController::SendUpdateCourses() {
  QJsonObject j;
  j["type"] = MessageType::UpdateCourseList;
  QJsonObject data;
  data["user_id"] = client_->getUserId();
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "UpdateCourses:" << message;
  emit sendToServer(message);
}

void CourseController::SendCreateInfo(const QString& courseName) {
  QJsonObject j;
  j["type"] = MessageType::CreateCourse;
  QJsonObject data;
  data["course_name"] = courseName;
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "CreateInfo:" << message;
  emit sendToServer(message);
}

void CourseController::DeleteCourse(const QString& courseId) {
  QJsonObject j;
  j["type"] = MessageType::DeleteCourse;
  QJsonObject data;
  data["course_id"] = courseId;
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "DeleteCourse:" << message;
  emit sendToServer(message);
}

void CourseController::SendJoinInfo(const QString& courseId) {
  QJsonObject j;
  j["type"] = MessageType::JoinCourse;
  QJsonObject data;
  data["course_id"] = courseId;
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "JoinInfo:" << message;
  emit sendToServer(message);
}

void CourseController::DeselectionCourse(const QString& courseId) {
  QJsonObject j;
  j["type"] = MessageType::DeselectionCourse;
  QJsonObject data;
  data["course_id"] = courseId;
  j["data"] = data;
  QJsonDocument doc(j);
  QString message = QString::fromUtf8(doc.toJson());
  qDebug() << "DeselectInfo:" << message;
  emit sendToServer(message);
}

//void CourseController::JoinClass(const QString& courseId) {
//  QJsonObject j;
//  j["type"] = MessageType::JoinClass;
//  QJsonObject data;
//  data["course_id"] = courseId;
//  data["user_id"] = client_->getUserId();
//  j["data"] = data;
//  QJsonDocument doc(j);
//  QString message = QString::fromUtf8(doc.toJson());
//  qDebug() << "JoinClass:" << message;
//  emit sendToServer(message);
//}

void CourseController::UpdateClassList(const QString& courseId) {
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

QVariantList CourseController::getCourses() {
  return client_->getCourses();
}

//QString CourseController::getStreamAddress() {
//  return client_->getStreamAddress();
//}

void CourseController::setCourseId(const QString& courseId) {
  client_->setCourseId(courseId);
}


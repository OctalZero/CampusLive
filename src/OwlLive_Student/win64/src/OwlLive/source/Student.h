/*********************************************************************************
  *Date:  2022.06.19
  *Description:  学生实体类，
  *				 主要用于存取学生的信息，暂时未使用。
**********************************************************************************/
#pragma once
#include <QString>
#include <QHash>
#include <QObject>
class QJsonObject;
class Course;

class Student : public QObject
{
	Q_OBJECT
public:
	// 获取学生信息
	QJsonObject getInfo() const;

	// 获取关联的所有课程信息
	QJsonArray getCoursesInfo() const;

	// 获取学生关联的所有课程的概要信息
	QJsonArray getCoursesAbstract() const;

	// 获取课程相关的所有课堂信息
	QJsonObject getClassesInfo(QString class_id) const;

	// 添加一个课程
	void AddCourse(const QJsonObject& data);

	// 添加一个课堂
	void setClass(const QJsonObject& data);

	// 删除一个课程
	void DeleteCourse(const QJsonObject& data);

	// 获取帐号
	QString getUserId();

	// 设置帐号
	void setUserId(const QString& id);

	// 获取姓名
	QString getName();

	// 设置姓名
	void setName(const QString& name);


	// 设置学生信息
	void setInfo(const QJsonObject& data);

	// 单例模式，获取单个实例
	static Student& GetStudent();
	Student(const Student&) = delete;
	Student& operator=(const Student&) = delete;
	~Student() = default;
private:
	Student() = default;
private:
	QHash<QString, Course*> courses_;  // 已选的课程，key为课程id
	QString user_id_ = "";  // 账号
	QString name_ = "";  // 姓名
};


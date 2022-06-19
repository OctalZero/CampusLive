/*********************************************************************************
  *Date:  2022.06.19
  *Description:  课程实体类，
  *				 主要用于存取课程的信息，暂时未使用。
**********************************************************************************/
#pragma once
#include <QString>
#include <QHash>
#include "Types.h"
class QJsonObject;
class QJsonArray;
class Class;

class Course
{
public:
	// 获取课程信息
	QJsonObject getInfo() const;

	// 获取课程概要信息
	QJsonObject getAbstract() const;

	// 获取相关的所有课堂信息
	QJsonObject getClassesInfo(QString class_id) const;

	// 判断课堂是否在该课程中
	bool ClassIsExit(QString class_id) const;

	// 设置课程信息
	void setInfo(const QJsonObject& data);

	// 添加一个课堂
	void setClass(const QJsonObject& data);

	// 删除一个课堂
	void DeleteClass(const QJsonObject& data);

	// 获取课程号
	QString getId();

	// 设置课程号
	void setId(const QString& id);

	// 获取课程名
	QString getName();

	// 设置课程名
	void setName(const QString& name);

	// 获取课程状态
	quint16 getStatus();

	// 设置课程状态
	void setStatus(const quint16& status);

	Course() = default;
	~Course();
private:
	QHash<QString, Class*> classes_;  // 课程中的所有课堂，key为课堂id
	QString id_ = "";  // 课程号
	QString name_ = "";  // 课程名
	quint16 status_ = CourseStatus::NotInClass;  // 课程状态，默认为未上课
};


#include "Course.h"
#include <QJsonObject>
#include <QJsonArray>
#include "Class.h"

QJsonObject Course::getInfo() const
{
	QJsonObject course_info;
	course_info["id"] = id_;
	course_info["name"] = name_;
    QJsonArray class_array;  // 构建课堂信息数组
    for(auto member: classes_){
        class_array.append(member->getInfo());
    }
    course_info["classes_info"] = class_array;  // 将课堂信息数组加入课程信息中

    return course_info;
}

QJsonObject Course::getAbstract() const
{
    QJsonObject course_info;
    course_info["id"] = id_;
    course_info["name"] = name_;
    course_info["status"] = status_;

    return course_info;
}

QJsonObject Course::getClassesInfo(QString class_id) const
{
    QJsonObject class_object;  // 构建课堂信息对象
    for (auto i = classes_.begin(); i != classes_.end(); i++) {
        if(i.key() == class_id){
            class_object = i.value()->getInfo();

        }
	}
    return class_object;
}

bool Course::ClassIsExit(QString class_id) const
{
    for(auto i = classes_.begin(); i != classes_.end(); i++){
        if(i.key() == class_id)return true;
    }
    return false;
}

void Course::setInfo(const QJsonObject& data)
{
	id_ = data["id"].toString();
	name_ = data["name"].toString();
	status_ = data["status"].toInt();
	QJsonArray class_array;  // 解析课堂信息数组
	class_array = data["classes_info"].toArray();
	for (auto c : class_array) {
		QJsonObject class_info = c.toObject();
		// 解析学生的课堂id
		QString class_id = class_info["id"].toString();
		// 存入学生参与的课堂
		Class* clas = new Class();
		classes_[class_id] = clas;
		// 存入课堂相应的课堂信息
		classes_[class_id]->setInfo(class_info);
    }
}

void Course::setClass(const QJsonObject &data)
{
    QString class_id = data["id"].toString();

    // 存入学生参与的课堂
    Class* clas = new Class();
    classes_[class_id] = clas;
    // 存入课堂相应的课堂信息
    classes_[class_id]->setInfo(data);
}

void Course::DeleteClass(const QJsonObject &data)
{
    QString class_id = data["id"].toString();
    for(auto i = classes_.begin(); i != classes_.end(); i++){
        if(i.key() == class_id){
            i.value()->~Class();
            break;
        }
    }
}

QString Course::getId()
{
    return id_;
}

void Course::setId(const QString &id)
{
    id_ = id;
}

QString Course::getName()
{
    return name_;
}

void Course::setName(const QString &name)
{
    name_ = name;
}

quint16 Course::getStatus()
{
    return status_;
}

void Course::setStatus(const quint16 &status)
{
    status_ = status;
}

Course::~Course()
{
    for(auto member: classes_){
        member->~Class();
        delete member;
    }
}

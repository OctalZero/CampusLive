#include "SetInfoControl.h"
#include <QJsonObject>
#include "Student.h"
#include "Types.h"

SetInfoControl::SetInfoControl()
	: student_(&Student::GetStudent())
{

}

SetInfoControl::~SetInfoControl()
{
}

void SetInfoControl::ParseChangeType(const QJsonObject& data)
{
	int type = data["type"].toInt();
	QJsonObject info = data["data"].toObject();
	switch (type) {
	case SetInfo::SetStudentInfo:
		SetStudentInfo(info);
		break;
	case SetInfo::AddClassInfo:
		AddClassInfo(info);
	default:
		break;
	}
}

void SetInfoControl::SetStudentInfo(const QJsonObject& data)
{
	QJsonObject info = data["student_info"].toObject();
	student_->setInfo(info);
}

void SetInfoControl::AddClassInfo(const QJsonObject& data)
{
	QJsonObject info, course_info;
	info = data["data"].toObject();
	course_info = info["course_info"].toObject();
	student_->setClass(course_info);
}

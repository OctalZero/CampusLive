#include "ClassroomControl.h"
#include <QJsonObject>
#include <QJsonArray>
#include "ClassroomWindow.h"
#include "Types.h"

ClassroomControl::ClassroomControl(const QString& course_id)
	: classroom_window_(new ClassroomWindow), course_id_(course_id)
{
	// 连接课堂控制器与教室界面的信号与信号
	connect(classroom_window_, &ClassroomWindow::SendClientRequestInfo, this, &ClassroomControl::SendClientRequestInfo);
	connect(classroom_window_, &ClassroomWindow::SendChatMessage, this, &ClassroomControl::SendChatMessage);
	connect(classroom_window_, &ClassroomWindow::ExitClassroom, this, &ClassroomControl::ExitClassroom);
}

ClassroomControl::~ClassroomControl()
{
	if (classroom_window_) {
		// 退出时会自动回收页面资源
		classroom_window_ = nullptr;
	}
}

void ClassroomControl::CloseWindow()
{
	if (classroom_window_) classroom_window_->close();
}

void ClassroomControl::ParseServerInfo(const QJsonObject& data)
{
	int type = data["type"].toInt();
	QJsonObject info = data["data"].toObject();
	switch (type)
	{
	case Classroom::InitClassroomResult:
		qDebug() << info;
		InitClassInfo(info);
		break;
	case Classroom::UpdateChatMessage:
		qDebug() << info;
		UpdateChatMessage(info);
		break;
	case Classroom::UpdateClassMembers:
		qDebug() << info;
		InitClassMembers(info);
		break;
	case Classroom::CloseClassroomResult:
		qDebug() << info;
		Q_EMIT CloseClassroom();
	default:
		break;
	}
}

void ClassroomControl::ParseFetchCache(const QJsonObject& data)
{
	int type = data["type"].toInt();
	QJsonObject info = data["data"].toObject();
	switch (type)
	{
	default:
		break;
	}
}

void ClassroomControl::ExitClassroom()
{
	QJsonObject data;
	QJsonObject info;
	data["to"] = MessageForwordControl::Classroom;
	data["type"] = Classroom::ExitClassroom;
	info["course_id"] = course_id_;
	data["data"] = info;

	Q_EMIT SendClientRequestInfo(data);
}

void ClassroomControl::SendInitClassroom()
{
	QJsonObject info, data;
	info["to"] = MessageForwordControl::Classroom;  // 标识数据请求者
	info["type"] = Classroom::InitClassroom;  // 标识请求类型
	data["course_id"] = course_id_;
	info["data"] = data;
	qDebug() << info;
	Q_EMIT SendClientRequestInfo(info);
}

void ClassroomControl::InitClassInfo(const QJsonObject& data)
{
	class_id_ = data["id"].toString();
	stream_address_ = data["stream_address"].toString();
	QJsonObject info = data["class_info"].toObject();  // 解析出课程信息
	QJsonArray members = info["member_abstract"].toArray();
	qDebug() << info;
	info["stream_address"] = stream_address_;
	if (classroom_window_) {  // 将解析的数据进行显示
		classroom_window_->InitClassMembers(members);
		//classroom_window_->PullStream(stream_address_);
	}
	qDebug() << "stream_address_" << stream_address_;
}

void ClassroomControl::SendChatMessage(const QJsonObject& data)
{
	QJsonObject info;
	QJsonObject object;
	info["to"] = MessageForwordControl::Classroom;
	info["type"] = Classroom::SendChatMessage;
	object["message"] = data["message"];
	qDebug() << "course id " << course_id_;
	object["course_id"] = course_id_;
	info["data"] = object;
	qDebug() << info;
	Q_EMIT SendClientRequestInfo(info);
}

void ClassroomControl::UpdateChatMessage(const QJsonObject& data)
{
	if (classroom_window_) {  // 将解析的数据进行显示
		classroom_window_->UpdateChatInfo(data);
	}
}

void ClassroomControl::InitClassMembers(const QJsonObject& data)
{
	QJsonArray members_info = data["member_abstract"].toArray();  // 解析出课堂成员信息
	if (classroom_window_) {  // 将解析的数据进行显示
		classroom_window_->InitClassMembers(members_info);
	}
}

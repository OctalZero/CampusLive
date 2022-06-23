#include "HomepageControl.h"
#include "HomepageWinodw.h"
#include "Types.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

HomepageControl::HomepageControl()
	: homepage_window_(new HomepageWindow())
{
	// 连接首页控制器与首页之间的信号与槽
	connect(homepage_window_, &HomepageWindow::InitClassroom, this, &HomepageControl::InitClassroom);
	connect(homepage_window_, &HomepageWindow::SendJoinCourse, this, &HomepageControl::SendAddCourse);
	connect(homepage_window_, &HomepageWindow::SendDeselectionCourse, this, &HomepageControl::SendDeselectionCourse);
	connect(homepage_window_, &HomepageWindow::SendQueryCourseDetails, this, &HomepageControl::SendQueryCourseDetails);
}

HomepageControl::~HomepageControl()
{
	if (homepage_window_) {
		// 退出时会自动回收页面资源
		homepage_window_ = nullptr;
	}
}

void HomepageControl::CloseWindow()
{
	if (homepage_window_) homepage_window_->close();
}

void HomepageControl::ParseServerInfo(const QJsonObject& data)
{
	int type = data["type"].toInt();
	QJsonObject info = data["data"].toObject();  // 解析出数据的部分
	switch (type)
	{
	case Homepage::InitHomepageResult:
		qDebug() << info["user_info"].toObject();
		UpdateCourseInfo(info["user_info"].toObject());
		break;
	case Homepage::AddCourseResult:
		ParseAddCourseResult(info);
		break;
	case Homepage::DeselectionCourseResult:
		ParseDeselectionCourseResult(info);
		break;
	case Homepage::QueryCourseDetailsResult:
		ParseQueryCourseDetailsResult(info);
	default:
		break;
	}
}

void HomepageControl::ParseFetchCache(const QJsonObject& data)
{
	int type = data["type"].toInt();
	QJsonObject info = data["data"].toObject();
	switch (type)
	{
	case GetInfo::GetCoursesAbstract:
		data["type"] = SetInfo::SetStudentInfo;

		//        Q_EMIT UpdateCache(data);
		UpdateCourseInfo(info);
	default:
		break;
	}
}

void HomepageControl::UpdateCourseInfo(const QJsonObject& data)
{
	qDebug() << "!!!!!!!!!!";
	QJsonArray info = data["courses_info"].toArray();

	if (homepage_window_) {  // 将解析的数据进行显示
		homepage_window_->UpdateCourseInfo(info);
	}

}

void HomepageControl::InitClassroom(const QJsonObject& data)
{
	QString course_id = data["course_id"].toString();
	qDebug() << "course id " << course_id;
	Q_EMIT OpenClassroom(course_id);

}

void HomepageControl::SendAddCourse(const QJsonObject& data)
{
	QJsonObject info, info_data;
	info["to"] = MessageForwordControl::Homepage;
	info["type"] = Homepage::AddCourse;
	info_data["course_id"] = data["course_id"];
	info["data"] = info_data;
	qDebug() << info;

	Q_EMIT SendClientRequestInfo(info);
}

void HomepageControl::SendDeselectionCourse(const QJsonObject& data)
{
	QJsonObject info, info_data;
	info["to"] = MessageForwordControl::Homepage;
	info["type"] = Homepage::DeselectionCourse;
	info_data["course_id"] = data["course_id"];
	info["data"] = info_data;
	qDebug() << info;

	Q_EMIT SendClientRequestInfo(info);
}

void HomepageControl::SendQueryCourseDetails(const QJsonObject& data)
{
	QJsonObject info, info_data;
	info["to"] = MessageForwordControl::Homepage;
	info["type"] = Homepage::QueryCourseDetails;
	info_data["course_id"] = data["course_id"];
	info["data"] = info_data;
	qDebug() << info;

	Q_EMIT SendClientRequestInfo(info);
}

void HomepageControl::SendInitHomepage()
{
	QJsonObject info;
	info["to"] = MessageForwordControl::Homepage;  // 标识数据请求者
	info["type"] = Homepage::InitHomepage;  // 标识请求类型
	Q_EMIT SendClientRequestInfo(info);
}

void HomepageControl::ParseAddCourseResult(const QJsonObject& data)
{
	QString result = data["result"].toString();
	if (result == "success") {
		Q_EMIT SendInitHomepage();
		homepage_window_->CloseJoinCourseForm();
	}
	else if (result == "failed") {
		QString reason = data["reason"].toString();
		QMessageBox::warning(0, "ERROR", "加入课程失败!\n" + reason);
	}
}

void HomepageControl::ParseDeselectionCourseResult(const QJsonObject& data)
{
	QString result = data["result"].toString();
	if (result == "success") {
		SendInitHomepage();  // 刷新课程概要信息
	}
}

void HomepageControl::ParseQueryCourseDetailsResult(const QJsonObject& data)
{
	homepage_window_->InitCourseDetails(data);
}

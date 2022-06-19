/*********************************************************************************
  *Date:  2022.06.19
  *Description:  首页控制类，
  *				 主要用于处理首页功能逻辑的类，并将结果反馈于首页窗口类。
**********************************************************************************/
#pragma once
#include <QObject>
#include <QHash>
#include "Types.h"
class HomepageWindow;
class QJsonArray;
class QString;

class HomepageControl : public QObject
{
	Q_OBJECT
public:
	HomepageControl();
	~HomepageControl();

	// 向服务端请求初始化首页的信息
	void SendInitHomepage();

	// 不使用界面退出时，手动关闭窗口
	void CloseWindow();

	// 解析来自服务器的信息
	void ParseServerInfo(const QJsonObject& data);

	// 解析获取的缓存信息
	void ParseFetchCache(const QJsonObject& data);

	// TEST:
	// 解析更新的课程信息，并反馈给注册界面
	void UpdateCourseInfo(const QJsonObject& data);
signals:
	// 发送客户端的请求信息的信号
	void SendClientRequestInfo(const QJsonObject& data);

	// 向信息类索取缓存信息的信号
	void FetchCache(const QJsonObject& data);

	// 打开教室界面的信号
	void OpenClassroom(const QString& course_id);

	// 更新缓存的信号
	void UpdateCache(const QJsonObject& data);
private slots:
	// 初始化教室的槽
	void InitClassroom(const QJsonObject& data);

	// 请求添加新的课程的槽
	void SendAddCourse(const QJsonObject& data);

	// 请求退选已有课程的槽
	void SendDeselectionCourse(const QJsonObject& data);

	// 请求查询课程详情的槽
	void SendQueryCourseDetails(const QJsonObject& data);
private:
	// 解析加入课程返回的结果
	void ParseAddCourseResult(const QJsonObject& data);

	// 解析退选课程返回的结果
	void ParseDeselectionCourseResult(const QJsonObject& data);

	// 解析查询课程详细信息结果
	void ParseQueryCourseDetailsResult(const QJsonObject& data);
private:
	HomepageWindow* homepage_window_;
};


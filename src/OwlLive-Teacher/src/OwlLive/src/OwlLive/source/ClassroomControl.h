#pragma once
#include <QObject>
#include <QString>
class ClassroomWindow;

class ClassroomControl : public QObject
{
	Q_OBJECT
public:
	ClassroomControl(const QString& course_id);
	~ClassroomControl();

	// 向服务端请求初始化教室的信息
	void SendInitClassroom();

	// 不使用界面退出时，手动关闭窗口
	void CloseWindow();

	// 解析来自服务器的信息
	void ParseServerInfo(const QJsonObject& data);

	// 解析获取的缓存信息
	void ParseFetchCache(const QJsonObject& data);

	// 退出课堂
	void ExitClassroom();
signals:
	// 发送客户端的请求信息的信号
	void SendClientRequestInfo(const QJsonObject& data);

	// 向信息类索取缓存信息
	void FetchCache(const QJsonObject& data);

	// 更新缓存的信号
	void UpdateCache(const QJsonObject& data);

	// 发送关闭教室的信号
	void CloseClassroom();
private:
	// 解析课堂的初始化信息，并反馈给教室界面
	void InitClassInfo(const QJsonObject& data);

	// 发送聊天信息
	void SendChatMessage(const QJsonObject& data);

	// 解析更新的聊天信息，并反馈给聊天框
	void UpdateChatMessage(const QJsonObject& data);

	// 解析进入课堂的所有成员信息，并反馈给成员列表
	void InitClassMembers(const QJsonObject& data);
private:
	ClassroomWindow* classroom_window_;  // 教室窗口类
	QString course_id_ = "";  // 当前进行的课堂所属的课程号
	QString class_id_ = "";  // 当前进行的课堂的课堂号
	QString stream_address_ = "";  // 当前进行的课堂的拉流地址
};


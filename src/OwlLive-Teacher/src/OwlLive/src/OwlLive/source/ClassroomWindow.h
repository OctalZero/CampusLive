#pragma once
#include "OwlLiveWindow.h"
#include "ui_Classroom.h"
class OwlPlay;
class OwlChat;

class ClassroomWindow : public OwlLiveWindow
{
	Q_OBJECT

public:
	ClassroomWindow(OwlLiveWindow* parent = Q_NULLPTR);
	~ClassroomWindow();

	// 拉取直播流
	void PullStream(const QString& stream_address);

	// 更新聊天框的聊天信息
	void UpdateChatInfo(const QJsonObject& data);

	// 初始化课堂成员列表的成员信息
	void InitClassMembers(const QJsonArray& data);
signals:
	// 发送客户端的请求信息的信号
	void SendClientRequestInfo(const QJsonObject& data);

	// 转发聊天信息到聊天框
	void ForwardChatMessage(const QJsonObject& data);

	// 发送聊天信息
	void SendChatMessage(const QJsonObject& data);

	// 退出课堂
	void ExitClassroom();
private:
	Ui::Classroom* ui_classroom_ = nullptr;  // 教室UI
	OwlPlay* owl_play_ = nullptr;  // 播放器类
	OwlChat* owl_chat_ = nullptr;  // 聊天框类
};

#pragma once
#include "ChatMessage.h"
#include <QWidget>
#include "ui_OwlChat.h"
#include "ui_ClassMembers.h"

class OwlChat : public QWidget
{
	Q_OBJECT
public:
	OwlChat(QWidget* parent = Q_NULLPTR);
	~OwlChat();

	// 解析消息的数据并显示到界面
	void ShowChatMessage(const QJsonObject& data);

	// 显示所有进入课堂的成员
	void ShowMembers(const QJsonArray& members_info);

signals:
	// 发送客户端的请求信息的信号
	void SendClientRequestInfo(const QJsonObject& data);
public slots:
	// 封装消息并发送聊天的槽
	void SendChatMessage();
protected:
	// 继承的刷新函数，根据消息框改变消息框大小
	void resizeEvent(QResizeEvent* event);
private slots:
	// 切换到聊天框
	void SwitchChatWidget();

	// 切换到成员列表框
	void SwitchMemberListWidget();
private:
	// 处理消息和时间
	// 参数：姓名、一行消息的样式、一行消息、消息内容、时间、消息的类型
	void dealMessage(QString name, ChatMessage* messageW, QListWidgetItem* item, QString text, QString time, ChatMessage::User_Type type);

	// 处理时间
	// 根据消息，如果没有消息不写时间，如果大于1分钟没写，就写时间
	void dealMessageTime(QString curMsgTime, QString name);
private:
	Ui::ChatWidget* ui_chat_widget_;  // 聊天界面UI
	QWidget* class_members_ = nullptr;  // 课堂成员信息实体
	Ui::ClassMembers* ui_class_members_ = nullptr;  // 课堂成员信息UI 
};

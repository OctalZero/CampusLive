#pragma once
#include "OwlLiveWindow.h"
#include "ui_Login.h"
class LoginControl;
struct Message;

class LoginWindow : public OwlLiveWindow
{
	Q_OBJECT

public:
	LoginWindow(OwlLiveWindow* parent = Q_NULLPTR);
	~LoginWindow();

	// 向界面反馈登录失败的原因
	void LoginFailed(const QString fail_reason);

	// 填充登录信息
	void FillLoginMessage(const Message& data);

	// 获取需要保存的信息
	QJsonObject AcquireSaveMessage();
signals:
	// 跳转到注册界面的信号
	void JumpRegister();

	// 向服务器发送登录的消息
	void SendLoginInfo(QJsonObject& login_info);
private slots:
	// 登录的槽
	void Login();

	// 判断自动登录的槽
	void JudgeAutoLogin(int state);
private:
	Ui::Login* ui_login_ = nullptr;  // 登录UI
};


#pragma once
#include "OwlLiveWindow.h"
#include "ui_Register.h"
class RegisterControl;

class RegisterWindow : public OwlLiveWindow
{
	Q_OBJECT

public:
	RegisterWindow(OwlLiveWindow* parent = Q_NULLPTR);
	~RegisterWindow();

	// 向界面反馈注册成功的账号信息
	void RegisterSuccess(const QString user_id);

	// 向界面反馈注册失败的原因
	void RegisterFailed(const QString fail_reason);
signals:
	// 退回到登录页面的信号
	void RegisterBackToLogin();

	// 向服务器发送注册的消息
	void SendRegisterInfo(QJsonObject& register_info);
private slots:
	// 注册
	void Register();
protected:
	Ui::Register* ui_register_ = nullptr;  // 注册UI
};


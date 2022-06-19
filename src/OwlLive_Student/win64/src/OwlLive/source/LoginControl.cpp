#include "LoginControl.h"
#include "LoginWindow.h"
#include <QJsonObject>
#include "Types.h"
#include "Password.h"
#include <QDebug>

LoginControl::LoginControl()
	: login_window_(new LoginWindow), password_(new Password)
{
	// 连接登录控制器与登录页面之间的信号与信号
	connect(login_window_, &LoginWindow::JumpRegister, this, &LoginControl::JumpRegister);
	// 连接登录控制器与登录页面之间的信号与槽
	connect(login_window_, &LoginWindow::SendLoginInfo, this, &LoginControl::SendLoginInfo);
}

LoginControl::~LoginControl()
{
	if (login_window_) {
		// 退出时会自动回收页面资源
		login_window_ = nullptr;
	}
}

void LoginControl::CloseWindow()
{
	if (login_window_) login_window_->close();
}

void LoginControl::ParseServerInfo(const QJsonObject data)
{
	int type = data["type"].toInt();
	QJsonObject info = data["data"].toObject();  // 解析出数据的部分
	switch (type)
	{
	case Login::LoginResult:
		ParseLoginResult(info);
	default:
		break;
	}
}

void LoginControl::AcquireMessage()
{
	if (password_) {
		Message deafault_account;
		deafault_account = password_->AcquireDefaultAccount();
		if (deafault_account.flag != -1) {
			if (login_window_) {
				login_window_->FillLoginMessage(deafault_account);
			}
		}
	}
}

void LoginControl::SendLoginInfo(QJsonObject& login_info)
{
	login_info["to"] = MessageForwordControl::Login;
	login_info["type"] = Login::Login;
	qDebug() << login_info;
	Q_EMIT SendClientRequestInfo(login_info);
}

void LoginControl::ParseLoginResult(const QJsonObject& data)
{
	if (!login_window_) return;

	QString result = data["result"].toString();
	if (result == "success") {
		QJsonObject data = login_window_->AcquireSaveMessage();
		password_->SaveMessage(data["account"].toString().toStdString(), data["password"].toString().toStdString(), data["flag"].toInt());
		Q_EMIT JumpHomepage();
	}
	else if (result == "failed") {
		QString reason = data["reason"].toString();
		login_window_->LoginFailed(reason);
	}
}

#include "RegisterControl.h"
#include <QJsonObject>
#include "RegisterWindow.h"
#include "Types.h"

RegisterControl::RegisterControl()
	: register_window_(new RegisterWindow)
{
	// 连接注册控制器与注册页面之间的信号与槽
	connect(register_window_, &RegisterWindow::RegisterBackToLogin, this, &RegisterControl::RegisterBackToLogin);
	connect(register_window_, &RegisterWindow::SendRegisterInfo, this, &RegisterControl::SendRegisterInfo);
}

RegisterControl::~RegisterControl()
{
	if (register_window_) {
		// 退出时会自动回收页面资源
		register_window_ = nullptr;
	}
}

void RegisterControl::CloseWindow()
{
	if (register_window_) register_window_->close();
}

void RegisterControl::ParseServerInfo(const QJsonObject& data)
{
	int type = data["type"].toInt();
	QJsonObject info = data["data"].toObject();  // 解析出数据的部分
	switch (type)
	{
	case Register::RegisterResult:
		ParseRegisterResult(info);
	default:
		break;
	}
}

void RegisterControl::SendRegisterInfo(QJsonObject& register_info)
{
	register_info["to"] = MessageForwordControl::Register;
	register_info["type"] = Register::Register;
	qDebug() << "register_info: " << register_info;
	Q_EMIT SendClientRequestInfo(register_info);
}

void RegisterControl::ParseRegisterResult(const QJsonObject& data)
{
	if (!register_window_) return;
	QString result = data["result"].toString();
	qDebug() << result;
	if (result == "success") {
		QString user_id = data["user_id"].toString();
		register_window_->RegisterSuccess(user_id);
	}
	else if (result == "failed") {
		QString reason = data["reason"].toString();
		register_window_->RegisterFailed(reason);
	}
}


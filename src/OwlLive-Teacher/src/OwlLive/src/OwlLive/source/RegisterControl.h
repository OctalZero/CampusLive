#pragma once
#include <QObject>
class RegisterWindow;
class QJsonObject;

class RegisterControl : public QObject
{
	Q_OBJECT

public:
	RegisterControl();
	~RegisterControl();

	// 不使用界面退出时，手动关闭窗口
	void CloseWindow();

	// 解析来自服务器的信息
	void ParseServerInfo(const QJsonObject& data);
signals:
	// 退回到登录页面的信号
	void RegisterBackToLogin();

	// 发送客户端的请求信息的信号
	void SendClientRequestInfo(const QJsonObject& data);

private slots:
	// 发送填写的注册信息的槽
	void SendRegisterInfo(QJsonObject& register_info);
private:
	// 处理注册结果，并反馈给注册界面
	void ParseRegisterResult(const QJsonObject& data);
private:
	RegisterWindow* register_window_;
};


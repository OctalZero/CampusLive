#pragma once
#include <QObject>

class QJsonObject;
class LoginWindow;
class Password;

class LoginControl : public QObject
{
	Q_OBJECT
public:
	LoginControl();
	~LoginControl();

	// 不使用界面退出时，手动关闭窗口
	void CloseWindow();

	// 解析并处理来自服务器的信息
	void ParseServerInfo(const QJsonObject data);

	// 从文件获取用户登录信息
	void AcquireMessage();
signals:
	// 跳转到注册界面的信号
	void JumpRegister();

	// 跳转到首页的信号
	void JumpHomepage();

	// 发送客户端的请求信息的信号
	void SendClientRequestInfo(const QJsonObject& data);

	// 更新学生缓存信息
	void UpdateStudentCache(const QJsonObject& data);

private slots:
	// 向服务器发送登录的消息
	void SendLoginInfo(QJsonObject& login_info);
private:
	// 处理登录结果，并反馈给登录界面
	void ParseLoginResult(const QJsonObject& data);
private:
	LoginWindow* login_window_;
	Password* password_ = nullptr;
};


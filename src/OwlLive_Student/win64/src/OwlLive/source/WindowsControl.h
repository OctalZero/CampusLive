/*********************************************************************************
  *Date:  2022.06.19
  *Description:  窗口控制类，
  *				 管理所有窗口控制类。
**********************************************************************************/
#pragma once
#include <QObject>
#include <QString>
#include <QHash>

class JsonObject;
class LoginControl;
class RegisterControl;
class HomepageControl;
class ClassroomControl;

class WindowsControl : public QObject
{
	Q_OBJECT

public:
	WindowsControl();
	~WindowsControl();

	// TEST:
	void OpenClassroom(const QString& course_id);

	// 打开登录控制器
	void OpenLogin();

	// 关闭登录登录控制器，若不是通过窗口UI关闭，需手动清理资源
	void CloseLogin();

	// 打开注册控制器
	void OpenRegister();

	// 关闭注册页面，若不是通过窗口UI关闭，需手动清理资源
	void CloseRegister();

	// 打开首页
	void OpenHomePage();

	// 关闭首页，若不是通过窗口UI关闭，需手动清理资源
	void CloseHomepage();

	// 关闭教室界面，若不是通过窗口UI关闭，需手动清理资源
	void CloseClassroom();

	// TEST:
	// 从登录页面跳转到注册页面的槽，并获取注册页面相关信息
	void JumpHomepage();
signals:
	// 发送客户端的请求信息的信号
	void SendClientRequestInfo(const QJsonObject& data);

	// 操作缓存信息的信号
	void ManipulateCache(const QJsonObject& data);
public slots:
	// 转发服务器发送的消息到相关的界面控制器
	void ForwardServerInfo(const QJsonObject& data);

	// 解析获取的缓存信息
	void ParseFetchCache(const QJsonObject& data);
private slots:
	// 从登录页面跳转到注册页面的槽
	void JumpRegister();

	// 从注册页面退回到登录界面
	void RegisterBackToLogin();

	// 向信息类索取缓存信息
	void FetchCache(const QJsonObject& data);

	// 更新缓存信息
	void UpdateCache(const QJsonObject& data);

	// TEST:	
public:
	HomepageControl* homepage_control_ = nullptr;
private:
	LoginControl* login_control_ = nullptr;
	RegisterControl* register_control_ = nullptr;
	ClassroomControl* classroom_control_ = nullptr;
};

#include "RemoteAttendClassControl.h"
#include "InfoControl.h"
#include "WindowsControl.h"
#include "ClientSocket.h"
// TEST:测试添加的库
#include <QJsonArray>
#include <QJsonObject>
#include "HomepageControl.h"

RemoteAttendClassControl::RemoteAttendClassControl()
	: info_control_(new InfoControl()), windows_control_(new WindowsControl()),
	client_socket_(&ClientSocket::GetClientSocket())
{
	// 连接界面控制类、信息控制类、客户端通信套接字类之间的信号与槽
	connect(windows_control_, &WindowsControl::SendClientRequestInfo, client_socket_, &ClientSocket::SendClientMessage);
	connect(client_socket_, &ClientSocket::ForwardServerMessage, windows_control_, &WindowsControl::ForwardServerInfo);
	connect(windows_control_, &WindowsControl::ManipulateCache, info_control_, &InfoControl::ParseOperationType);
	connect(info_control_, &InfoControl::ReturnQueryResults, windows_control_, &WindowsControl::ParseFetchCache);
}

RemoteAttendClassControl::~RemoteAttendClassControl()
{
	if (info_control_) {
		delete info_control_;
		info_control_ = nullptr;
	}
	if (windows_control_) {
		delete windows_control_;
		windows_control_ = nullptr;
	}
}

void RemoteAttendClassControl::Start()
{
	// 连接界面控制类与客户端套接字类之间的信号与槽
	connect(windows_control_, &WindowsControl::SendClientRequestInfo, client_socket_, &ClientSocket::SendClientMessage);
	// 连接服务器返回消息的信号与界面控制类的解析服务器返回消息的槽
	connect(client_socket_, &ClientSocket::ForwardServerMessage, windows_control_, &WindowsControl::ForwardServerInfo);
	// 连接信息控制类的返回消息与界面控制类的获取缓存信息的槽
	connect(info_control_, &InfoControl::ReturnQueryResults, windows_control_, &WindowsControl::ParseFetchCache);
	// 连接界面控制类的获取缓存信息与信息控制类的解析操作类型
	connect(windows_control_, &WindowsControl::ManipulateCache, info_control_, &InfoControl::ParseOperationType);

	windows_control_->OpenLogin();
}

void RemoteAttendClassControl::Test()
{

	//QJsonArray test;
	//QJsonObject course_object;
	//for (int i = 0; i < 49; ++i) {
	//	course_object["id"] = "id" + QString::number(i);
	//	course_object["name"] = "name" + QString::number(i);
	//	course_object["status"] = 0;
	//	test.append(course_object);
	//}
	//QJsonObject info;
	//info["courses_abstract"] = test;

	windows_control_->OpenLogin();
	//windows_control_->OpenRegister();
	//windows_control_->JumpHomepage();

	//windows_control_->OpenHomePage();
	//windows_control_->homepage_control_->UpdateCourseInfo(info);
	//windows_control_->OpenClassroom("1");
	//config::Config config;
	//config.Open();


}

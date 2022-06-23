#include "WindowsControl.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "LoginControl.h"
#include "RegisterControl.h"
#include "HomepageControl.h"
#include "ClassroomControl.h"
#include "Types.h"

WindowsControl::WindowsControl()
{
}

WindowsControl::~WindowsControl()
{
	if (login_control_) {
		delete login_control_;
		login_control_ = nullptr;
	}
	if (register_control_) {
		delete register_control_;
		register_control_ = nullptr;
	}
	if (homepage_control_) {
		delete homepage_control_;
		homepage_control_ = nullptr;
	}
	if (classroom_control_) {
		delete classroom_control_;
		classroom_control_ = nullptr;
	}
}

void WindowsControl::OpenLogin()
{
	login_control_ = new LoginControl();
	// 连接登录控制器与窗口控制器之间的信号与槽
	connect(login_control_, &LoginControl::JumpRegister, this, &WindowsControl::JumpRegister);
	connect(login_control_, &LoginControl::SendClientRequestInfo, this, &WindowsControl::SendClientRequestInfo);
	connect(login_control_, &LoginControl::JumpHomepage, this, &WindowsControl::JumpHomepage);
	connect(login_control_, &LoginControl::UpdateStudentCache, this, &WindowsControl::UpdateCache);

	// 获取登录信息
	login_control_->AcquireMessage();
}

void WindowsControl::CloseLogin()
{
	// 没有通过窗口的关闭按钮进行资源回收，手动回收资源
	if (login_control_) {
		login_control_->CloseWindow();
		delete login_control_;
		login_control_ = nullptr;
	}
}

void WindowsControl::OpenRegister()
{
	register_control_ = new RegisterControl();
	// 连接注册控制器与窗口控制器之间的信号与槽
	connect(register_control_, &RegisterControl::RegisterBackToLogin, this, &WindowsControl::RegisterBackToLogin);
	connect(register_control_, &RegisterControl::SendClientRequestInfo, this, &WindowsControl::SendClientRequestInfo);
}

void WindowsControl::CloseRegister()
{
	if (register_control_) {
		register_control_->CloseWindow();
		delete register_control_;
		register_control_ = nullptr;
	}
}

void WindowsControl::OpenHomePage()
{
	homepage_control_ = new HomepageControl();
	// 连接首页控制器与窗口控制器之间的信号与槽
	connect(homepage_control_, &HomepageControl::FetchCache, this, &WindowsControl::FetchCache);
	connect(homepage_control_, &HomepageControl::OpenClassroom, this, &WindowsControl::OpenClassroom);
	connect(homepage_control_, &HomepageControl::UpdateCache, this, &WindowsControl::UpdateCache);
	connect(homepage_control_, &HomepageControl::SendClientRequestInfo, this, &WindowsControl::SendClientRequestInfo);
	homepage_control_->SendInitHomepage();
}

void WindowsControl::CloseHomepage()
{
	if (homepage_control_) homepage_control_->CloseWindow();
}

void WindowsControl::OpenClassroom(const QString& course_id)
{
	classroom_control_ = new ClassroomControl(course_id);
	// 连接教室界面控制器与窗口控制器之间的信号与槽
	connect(classroom_control_, &ClassroomControl::FetchCache, this, &WindowsControl::FetchCache);
	connect(classroom_control_, &ClassroomControl::SendClientRequestInfo, this, &WindowsControl::SendClientRequestInfo);
	connect(classroom_control_, &ClassroomControl::UpdateCache, this, &WindowsControl::UpdateCache);
	connect(classroom_control_, &ClassroomControl::CloseClassroom, this, &WindowsControl::CloseClassroom);

	classroom_control_->SendInitClassroom();  // 向服务端发送初始化教室请求
}

void WindowsControl::CloseClassroom()
{
	// 没有通过窗口的关闭按钮进行资源回收，手动回收资源
	if (classroom_control_) {
		classroom_control_->CloseWindow();
		delete classroom_control_;
		classroom_control_ = nullptr;
	}
}

void WindowsControl::JumpRegister()
{
	CloseLogin();
	OpenRegister();
}

void WindowsControl::RegisterBackToLogin()
{
	// 没有通过窗口的关闭按钮进行资源回收，手动回收资源
	if (register_control_) {
		register_control_->CloseWindow();
		delete register_control_;
		register_control_ = nullptr;
	}

	OpenLogin();
}

void WindowsControl::FetchCache(const QJsonObject& data)
{
	data["to"] = MessageForwordControl::GetInfo;  // 标识信息传递给谁
	Q_EMIT ManipulateCache(data);
}

void WindowsControl::UpdateCache(const QJsonObject& data)
{
	data["to"] = MessageForwordControl::SetInfo;  // 标识信息传递给谁
	Q_EMIT ManipulateCache(data);
}

void WindowsControl::ForwardServerInfo(const QJsonObject& data)
{
	int to = data["to"].toInt();
	switch (to) {
	case MessageForwordControl::Login:
		login_control_->ParseServerInfo(data);
		break;
	case MessageForwordControl::Register:
		register_control_->ParseServerInfo(data);
		break;
	case MessageForwordControl::Homepage:
		homepage_control_->ParseServerInfo(data);
		break;
	case MessageForwordControl::Classroom:
		classroom_control_->ParseServerInfo(data);
		break;
	default:
		break;
	}
}

void WindowsControl::ParseFetchCache(const QJsonObject& data)
{
	int to = data["to"].toInt();
	switch (to)
	{
	case MessageForwordControl::Homepage:
		if (homepage_control_) homepage_control_->ParseFetchCache(data);
		break;
	case MessageForwordControl::Classroom:
		if (classroom_control_) classroom_control_->ParseFetchCache(data);
	default:
		break;
	}
}

void WindowsControl::JumpHomepage()
{
	CloseLogin();
	OpenHomePage();
}

//void WindowsControl::on_test_button_clicked()
//{
//	QWidget* member_card = new QWidget();
//	ui_member_card_->setupUi(member_card);
//	ui_member_card_->member_info->setText("学生");
//
//	ui_classroom_->member_info_layout->addWidget(ui_member_card_->member_info);
//}

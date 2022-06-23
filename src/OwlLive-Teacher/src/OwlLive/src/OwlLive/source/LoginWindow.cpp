#include "LoginWindow.h"
#include <QJsonObject>
#include <QMessageBox>
#include "Types.h"
#include "Password.h"

LoginWindow::LoginWindow(OwlLiveWindow* parent)
	: OwlLiveWindow(parent), ui_login_(new Ui::Login)
{
	this->setAttribute(Qt::WA_DeleteOnClose);  // 确保关闭窗口时清理资源
	ui_login_->setupUi(this);

	this->show();
	// 连接登录界面与登录控制类之间的信号与槽
	connect(ui_login_->register_id, SIGNAL(clicked()), this, SIGNAL(JumpRegister()));
	connect(ui_login_->login, SIGNAL(clicked()), this, SLOT(Login()));
	connect(ui_login_->auto_login, &QCheckBox::stateChanged, this, &LoginWindow::JudgeAutoLogin);
}

LoginWindow::~LoginWindow()
{
	if (ui_login_) {
		delete ui_login_;
		ui_login_ = nullptr;
	}
}

void LoginWindow::LoginFailed(const QString fail_reason)
{
	QMessageBox::warning(0, "ERROR", "Login Failed!\n" + fail_reason);
}

void LoginWindow::FillLoginMessage(const Message& data)
{
	if (data.flag == 0) {
		ui_login_->id->setText(QString::fromStdString(data.account));
	}
	else if (data.flag == 1) {
		ui_login_->id->setText(QString::fromStdString(data.account));
		ui_login_->password->setText(QString::fromStdString(data.password));
		ui_login_->remember_password->setChecked(true);
	}
	else {
		ui_login_->id->setText(QString::fromStdString(data.account));
		ui_login_->password->setText(QString::fromStdString(data.password));
		ui_login_->remember_password->setChecked(true);
		ui_login_->auto_login->setChecked(true);
		Login();
	}
}

QJsonObject LoginWindow::AcquireSaveMessage()
{
	QString user_id = ui_login_->id->text();
	QString password = ui_login_->password->text();
	QJsonObject save_info;
	bool result = ui_login_->auto_login->isChecked();
	if (!result) {
		result = ui_login_->remember_password->isChecked();
		if (result) {
			save_info["flag"] = 1;
		}
		else {
			save_info["flag"] = 0;
		}
	}
	else {
		save_info["flag"] = 2;
	}
	save_info["account"] = user_id;
	save_info["password"] = password;

	return save_info;
}

void LoginWindow::Login()
{
	QString user_id = ui_login_->id->text();
	QString password = ui_login_->password->text();
	// 构造要发送的登录的信息
	QJsonObject login_info, login_data;
	login_data["user_id"] = user_id;
	login_data["password"] = password;
	login_info["data"] = login_data;

	Q_EMIT SendLoginInfo(login_info);
}

void LoginWindow::JudgeAutoLogin(int state)
{
	// 选中自动登录时，默认选择记住密码
	if (state == Qt::Checked) {
		ui_login_->remember_password->setChecked(true);
	}
}

#include "RegisterWindow.h"
#include <QJsonObject>
#include <QMessageBox>
#include "Types.h"

RegisterWindow::RegisterWindow(OwlLiveWindow* parent)
	: OwlLiveWindow(parent), ui_register_(new Ui::Register)
{
	this->setAttribute(Qt::WA_DeleteOnClose);  // 确保关闭窗口时清理资源
	ui_register_->setupUi(this);
	ui_register_->result->hide();  // 注册时隐藏注册结果界面

	this->show();

	// 连接所有与注册界面操作有关的信号与槽
	connect(ui_register_->login, SIGNAL(clicked()), this, SIGNAL(RegisterBackToLogin()));
	connect(ui_register_->register_button, SIGNAL(clicked()), this, SLOT(Register()));
}

RegisterWindow::~RegisterWindow()
{
	if (ui_register_) {
		delete ui_register_;
		ui_register_ = nullptr;
	}
}

void RegisterWindow::RegisterSuccess(const QString user_id)
{
	ui_register_->result_status->setText("注册成功！");
	ui_register_->result_id->setText("账号:  " + user_id);
	ui_register_->result->show();  // 注册时隐藏注册结果界面
	qDebug() << user_id;
}

void RegisterWindow::RegisterFailed(const QString fail_reason)
{
	QMessageBox::warning(0, "ERROR", "Register Failed!\n" + fail_reason);
	qDebug() << fail_reason;
}

void RegisterWindow::Register()
{
	QString user_name = ui_register_->name->text();
	QString password = ui_register_->password->text();
	// 构造要发送的注册的信息
	QJsonObject register_info, register_data;
	register_data["user_name"] = user_name;
	register_data["password"] = password;
    register_data["identification"] = ClientIdentification::Teacher;
	register_info["data"] = register_data;

	Q_EMIT SendRegisterInfo(register_info);
}

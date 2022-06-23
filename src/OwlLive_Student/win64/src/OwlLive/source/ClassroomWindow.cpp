#include "ClassroomWindow.h"
#include "OwlPlay.h"
#include "OwlChat.h"


ClassroomWindow::ClassroomWindow(OwlLiveWindow* parent)
	: OwlLiveWindow(parent), ui_classroom_(new Ui::Classroom)
{
	ui_classroom_->OwlChatWidget = owl_chat_;
	ui_classroom_->OwlPlayWidget = owl_play_;
	this->setAttribute(Qt::WA_DeleteOnClose);  // 确保关闭窗口时清理资源
	ui_classroom_->setupUi(this);

	// 初始化播放器和聊天框，并绑定到教室窗口中
	owl_play_ = new OwlPlay(ui_classroom_->OwlPlayWidget);
	owl_chat_ = new OwlChat(ui_classroom_->OwlChatWidget);

	this->show();

	// 连接教室界面与聊天框的信号与信号
	connect(owl_chat_, &OwlChat::SendClientRequestInfo, this, &ClassroomWindow::SendChatMessage);
	// 连接教室界面与聊天框的信号与槽
	connect(this, &ClassroomWindow::ForwardChatMessage, owl_chat_, &OwlChat::ShowChatMessage);
}

ClassroomWindow::~ClassroomWindow()
{
	if (ui_classroom_) {
		delete ui_classroom_;
		ui_classroom_ = nullptr;
	}

	Q_EMIT ExitClassroom();
}

void ClassroomWindow::PullStream(const QString& stream_address)
{
	owl_play_->PullStream(stream_address);
}

void ClassroomWindow::UpdateChatInfo(const QJsonObject& data)
{
	if (owl_chat_) {
		owl_chat_->ShowChatMessage(data);
	}
}

void ClassroomWindow::InitClassMembers(const QJsonArray& data)
{
	if (owl_chat_) {
		owl_chat_->ShowMembers(data);
	}
}


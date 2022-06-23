#include "OwlChat.h"
#include <QListWidgetItem>
#include <QJsonObject>
#include <QJsonArray>
#include "Types.h"

OwlChat::OwlChat(QWidget* parent)
	: QWidget(parent), ui_chat_widget_(new Ui::ChatWidget)
{
	ui_chat_widget_->setupUi(this);
	ui_chat_widget_->info_stack->setCurrentIndex(0);

	// 连接界面信号与功能槽
	connect(ui_chat_widget_->chat_info, SIGNAL(clicked()), this, SLOT(SwitchChatWidget()));
	connect(ui_chat_widget_->member_info, SIGNAL(clicked()), this, SLOT(SwitchMemberListWidget()));
	connect(ui_chat_widget_->chat_send, SIGNAL(clicked()), this, SLOT(SendChatMessage()));
}

OwlChat::~OwlChat()
{
	if (ui_chat_widget_) {
		delete ui_chat_widget_;
		ui_chat_widget_ = nullptr;
	}
}


void OwlChat::dealMessage(QString name, ChatMessage* messageW, QListWidgetItem* item, QString text, QString time, ChatMessage::User_Type type)
{
	// 设置消息宽度 使用listwidget宽度
	messageW->setFixedWidth(ui_chat_widget_->chat_show->width());

	QSize size = messageW->fontRect(text);
	item->setSizeHint(size);
	messageW->setText(name, text, time, size, type);
	ui_chat_widget_->chat_show->setItemWidget(item, messageW);
}

void OwlChat::dealMessageTime(QString curMsgTime, QString name)
{
	bool isShowTime = false;
	if (ui_chat_widget_->chat_show->count() > 0) {
		QListWidgetItem* lastItem = ui_chat_widget_->chat_show->item(ui_chat_widget_->chat_show->count() - 1);
		ChatMessage* messageW = (ChatMessage*)ui_chat_widget_->chat_show->itemWidget(lastItem);
		int lastTime = messageW->time().toInt();
		int curTime = curMsgTime.toInt();
		qDebug() << "curTime lastTime:" << curTime - lastTime;
		isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
	}
	else {
		isShowTime = true;
	}
	if (isShowTime) {
		ChatMessage* messageTime = new ChatMessage(ui_chat_widget_->chat_show->parentWidget());
		QListWidgetItem* itemTime = new QListWidgetItem(ui_chat_widget_->chat_show);

		// 设置时间的标识的宽度，用listwidget的宽度
		QSize size = QSize(ui_chat_widget_->chat_show->width(), 40);

		messageTime->resize(size);
		itemTime->setSizeHint(size);
		messageTime->setText(curMsgTime, name, curMsgTime, size, ChatMessage::User_Time);
		ui_chat_widget_->chat_show->setItemWidget(itemTime, messageTime);
	}
}

void OwlChat::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);

	ui_chat_widget_->chat_input->resize(ui_chat_widget_->chat_input_widget->width() - 20, ui_chat_widget_->chat_input_widget->height() - 20);
	ui_chat_widget_->chat_input->move(10, 10);

	ui_chat_widget_->chat_send->move(ui_chat_widget_->chat_input->width() + ui_chat_widget_->chat_input->x() - ui_chat_widget_->chat_send->width() - 10,
		ui_chat_widget_->chat_input->height() + ui_chat_widget_->chat_input->y() - ui_chat_widget_->chat_send->height() - 10);


	for (int i = 0; i < ui_chat_widget_->chat_show->count(); i++) {
		ChatMessage* messageW = (ChatMessage*)ui_chat_widget_->chat_show->itemWidget(ui_chat_widget_->chat_show->item(i));
		QListWidgetItem* item = ui_chat_widget_->chat_show->item(i);

		dealMessage(messageW->name(), messageW, item, messageW->text(), messageW->time(), messageW->userType());
	}
}

void OwlChat::ShowChatMessage(const QJsonObject& data)
{
	QString uname = data.value("name").toString();
	QString msg = data.value("message").toString();
	qDebug() << "接收的消息：--" << uname << msg;
	QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳

	dealMessageTime(time, uname);

	ChatMessage* messageW = new ChatMessage(ui_chat_widget_->chat_show);
	QListWidgetItem* item = new QListWidgetItem(ui_chat_widget_->chat_show);

	dealMessage(uname, messageW, item, msg, time, ChatMessage::User_She);

	ui_chat_widget_->chat_show->setCurrentRow(ui_chat_widget_->chat_show->count() - 1);
}


void OwlChat::SendChatMessage()
{
	QString messgae = ui_chat_widget_->chat_input->toPlainText();
	ui_chat_widget_->chat_input->setText("");
	QJsonObject info;
	info["message"] = messgae;

	Q_EMIT SendClientRequestInfo(info);
}

void OwlChat::ShowMembers(const QJsonArray& members_info)
{
	ui_chat_widget_->member_show->clear();  // 清除成员列表 

	for (auto it : members_info)
	{
		QString name = it.toObject().value("name").toString();
		int identification = it.toObject().value("identification").toInt();
		class_members_ = new QWidget();
		ui_class_members_ = new Ui::ClassMembers;
		ui_class_members_->setupUi(class_members_);

		QListWidgetItem* item = new QListWidgetItem(ui_chat_widget_->member_show);
		item->setSizeHint(QSize(350, 60));
		if (identification == ClientIdentification::Student) {
			ui_class_members_->student_name->setText(name);
			ui_chat_widget_->member_show->setItemWidget(item, ui_class_members_->student_info);

		}
		else if (identification == ClientIdentification::Teacher) {
			ui_class_members_->teacher_name_->setText(name);
			ui_chat_widget_->member_show->setItemWidget(item, ui_class_members_->teacher_info);

		}
	}
}

void OwlChat::SwitchChatWidget()
{
	ui_chat_widget_->info_stack->setCurrentIndex(0);
}

void OwlChat::SwitchMemberListWidget()
{
	ui_chat_widget_->info_stack->setCurrentIndex(1);
}


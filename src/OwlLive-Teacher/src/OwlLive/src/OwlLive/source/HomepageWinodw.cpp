#include "HomepageWinodw.h"
#include "HomepageControl.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

HomepageWindow::HomepageWindow(OwlLiveWindow* parent)
	: OwlLiveWindow(parent), ui_homepage_(new Ui::Homepage)
{
	this->setAttribute(Qt::WA_DeleteOnClose);  // 确保关闭窗口时清理资源
	ui_homepage_->setupUi(this);
	// 将网格布局加入到课程列表中
	ui_homepage_->course_list->widget()->setLayout(ui_homepage_->course_layout);
	this->show();
}

HomepageWindow::~HomepageWindow()
{
	if (ui_homepage_) {
		delete ui_homepage_;
		ui_homepage_ = nullptr;
	}
}

void HomepageWindow::UpdateCourseInfo(const QJsonArray& data)
{

	qDebug() << "更新！！！";

	// 清空之前的课程信息列表
	while (ui_homepage_->course_layout->count() != 0)
	{
		QLayoutItem* item = ui_homepage_->course_layout->takeAt(0);
		if (item)
		{
			delete item->widget();
			delete item;
		}
	}

	// 更新已选的课程
	QString id, name, status;
	int status_flag, row, colum, course_num = 0;
	for (int i = 0; i < data.count(); ++i) {
		id = data.at(i)["id"].toString();
		name = data.at(i)["name"].toString();
		status_flag = data.at(i)["status"].toInt();
		// 根据课程状态的标记，设置界面中显示的课程状态信息
		if (status_flag == CourseStatus::NotInClass)
			status = "上课中";
		else if (status_flag == CourseStatus::InClass)
			status = "未上课";

		row = course_num / 2;  // 课程列表的行数
		colum = course_num % 2;  // 课程列表的列数

		// 创建课程信息卡片并加入到首页中
		QWidget* course_card = new QWidget();
		course_card->setAttribute(Qt::WA_DeleteOnClose);

		course_cards_[id] = course_card;
		Ui::CourseCard* ui_course_card = new Ui::CourseCard();
		ui_course_cards_[id] = ui_course_card;
		ui_course_cards_[id]->setupUi(course_cards_[id]);

		ui_course_cards_[id]->course_name->setText(name);
		ui_course_cards_[id]->course_status->setText(status);
		ui_homepage_->course_layout->addWidget(ui_course_cards_[id]->course_box, row, colum);
		// 增加课程数量
		++course_num;

		// 连接所有与课程信息卡操作有关的信号与槽
		connect(ui_course_cards_[id]->enter_class, &QPushButton::clicked,
			this, [this, id]() { JoinClassroom(id); });  // 用lambda表达式传递课程id
		connect(ui_course_cards_[id]->pushButton, &QPushButton::clicked,
			this, [this, id]() { DeselectionCourse(id); });
		connect(ui_course_cards_[id]->course_info, &QPushButton::clicked,
			this, [this, id]() { QueryCourseDetails(id); });
	}

	// 创建加入课程的卡片并加入到首页中
	join_course_card_ = new QWidget();
	join_course_card_->setAttribute(Qt::WA_DeleteOnClose);
	ui_join_course_card_ = new Ui::JoinCourseCard();
	ui_join_course_card_->setupUi(join_course_card_);

	row = course_num / 2;  // 课程列表的行数
	colum = course_num % 2;  // 课程列表的列数
	ui_homepage_->course_layout->addWidget(ui_join_course_card_->join_course, row, colum);
	++course_num;
	connect(ui_join_course_card_->join_course, &QPushButton::clicked, this, &HomepageWindow::OpenJoinCourseForm);
}

void HomepageWindow::CloseJoinCourseForm()
{
	if (join_course_form_) {
		join_course_form_->close();
		delete join_course_card_;
	}
}

void HomepageWindow::InitCourseDetails(const QJsonObject& data)
{
	// 创建课程详情组件
	course_details_ = new QWidget();
	course_details_->setAttribute(Qt::WA_DeleteOnClose);
	ui_course_details_ = new Ui::CourseDetails;
	ui_course_details_->setupUi(course_details_);

	// 解析并填写信息
	QString course_id = data["course_id"].toString();
	QString teacher_name = data["teacher_name"].toString();
	ui_course_details_->course_id->setText(course_id);
	ui_course_details_->teacher_name->setText(teacher_name);


}
void HomepageWindow::JoinClassroom(const QString course_id)
{
	// 容错
	if (!ui_homepage_ || !ui_course_cards_[course_id]) return;
	if (ui_course_cards_[course_id]->course_status->text() != "上课中") {
		QMessageBox::information(0, "ERROR", "该课程未处于上课状态");
		return;
	}

	// 向服务器传递请求上课的信息
	QJsonObject join_class_data;
	join_class_data["course_id"] = course_id;

	Q_EMIT InitClassroom(join_class_data);
	qDebug() << course_id;
}

void HomepageWindow::DeselectionCourse(const QString course_id)
{
	// 向服务器传递请求退选课程的信息
	QJsonObject deselection_course_data;
	deselection_course_data["course_id"] = course_id;

	Q_EMIT SendDeselectionCourse(deselection_course_data);
}

void HomepageWindow::QueryCourseDetails(const QString course_id)
{
	// 向服务器传递请求查询课程详细信息
	QJsonObject data;
	data["course_id"] = course_id;

	Q_EMIT SendQueryCourseDetails(data);
}

void HomepageWindow::OpenJoinCourseForm()
{
	join_course_form_ = new QWidget();
	ui_join_course_form_ = new Ui::JoinCourseForm();
	ui_join_course_form_->setupUi(join_course_form_);
	join_course_form_->show();

	// 连接与加入课程表单有关的信号与槽
	connect(ui_join_course_form_->confirm, &QPushButton::clicked, this, &HomepageWindow::JoinCourse);
}

void HomepageWindow::JoinCourse()
{
	if (ui_join_course_form_) {
		QJsonObject data;
		QString course_id = ui_join_course_form_->course_id_input->text();
		data["course_id"] = course_id;

		Q_EMIT SendJoinCourse(data);
	}
}



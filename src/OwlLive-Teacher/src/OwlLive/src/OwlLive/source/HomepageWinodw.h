#pragma once
#include "OwlLiveWindow.h"
#include "ui_Homepage.h"
#include "ui_CourseCard.h"
#include "ui_JoinCourseCard.h"
#include "ui_JoinCourseForm.h"
#include "ui_CourseDetails.h"
class HomepageControl;
class QJsonArray;

class HomepageWindow : public OwlLiveWindow
{
	Q_OBJECT

public:
	HomepageWindow(OwlLiveWindow* parent = Q_NULLPTR);
	~HomepageWindow();

	// 更新页面的课程概要信息
	void UpdateCourseInfo(const QJsonArray& data);

	// 关闭加入课程表单界面
	void CloseJoinCourseForm();

	// 初始化课程详情界面
	void InitCourseDetails(const QJsonObject& data);
signals:
	// 初始化教室的信号
	void InitClassroom(const QJsonObject& data);

	// 发送请求加入课程的信号
	void SendJoinCourse(const QJsonObject& data);

	// 发送请求退选课程的信号
	void SendDeselectionCourse(const QJsonObject& data);

	// 发送查询课程详情的信号
	void SendQueryCourseDetails(const QJsonObject& data);
private slots:
	// 加入到教室的槽
	void JoinClassroom(const QString course_id);

	// 退选课程的槽
	void DeselectionCourse(const QString course_id);

	// 查询课程详情的槽
	void QueryCourseDetails(const QString course_id);

	// 打开加入课程表单的槽
	void OpenJoinCourseForm();

	// 加入课程的槽
	void JoinCourse();

private:
	Ui::Homepage* ui_homepage_ = nullptr;  // 主页UI
	Ui::JoinCourseCard* ui_join_course_card_ = nullptr;  // 课程卡片UI
	Ui::JoinCourseForm* ui_join_course_form_ = nullptr;  // 加入课程表单UI 
	Ui::CourseDetails* ui_course_details_ = nullptr;  // 课程详情UI
	QWidget* join_course_card_ = nullptr;  // 加入课程卡片实体
	QWidget* join_course_form_ = nullptr;  // 加入课程的表单实体
	QWidget* course_details_ = nullptr;  // 课程详情的实体
	QHash<QString, QWidget*> course_cards_;  // 课程列表中的所有课程信息实体，key为课程id
	QHash<QString, Ui::CourseCard*> ui_course_cards_;  // 课程列表中的所有课程信息UI，key为课程id
};


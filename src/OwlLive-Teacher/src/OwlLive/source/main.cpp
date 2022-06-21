#include "WindowsControl.h"
#include <QtWidgets/QApplication>
#include "RemoteAttendClassControl.h"

#include "InfoControl.h"
#include <QJsonObject>
#include <QJsonArray>
int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	//InfoControl* info_control = new InfoControl();
	//info_control->Open();
	//info_control->ConnectServer();

	// TEST:注册 
	//info_control->Register();
	// TEST:登录
	//QString user_id = "2";
	//QString password = "123";
	//info_control->Login(user_id, password);
	// TEST:显示学生的信息和所选的课程的信息
	//QJsonObject student_info;
	//student_info = info_control->getStudentInfo();
	//qDebug() << student_info;
	//QJsonArray course_info;
	//course_info = info_control->getCoursesInfo();
	//qDebug() << course_info;

	// 由远程上课控制类负责启动软件
	RemoteAttendClassControl remote_attend_class_control;
	// TEST:与界面耦合后逻辑测试
	remote_attend_class_control.Test();

	/*w.setWindowFlags(w.windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);*/


	return a.exec();
}

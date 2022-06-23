#include "GetInfoControl.h"
#include "Student.h"
#include <QJsonObject>
#include <QJsonArray>
#include "Types.h"

GetInfoControl::GetInfoControl()
	: student_(&Student::GetStudent())
{

}

GetInfoControl::~GetInfoControl()
{
}

void GetInfoControl::ParseQueryType(const QJsonObject& data)
{
	int type = data["type"].toInt();
	switch (type) {
	case GetInfo::GetCoursesAbstract:
		ParseGetCoursesAbstract(data);
		break;
	case GetInfo::GetClassInfo:
		ParseGetClassInfo(data);
		break;
	default:
		break;
	}
}

QJsonArray GetInfoControl::getCoursesAbstract()
{
	// TODO:换接口
	return student_->getCoursesAbstract();
}

QJsonObject GetInfoControl::getClassInfo(const QString& course_id, const QString& class_id)
{
	// TODO:换接口，通过课程id和课堂id读取课堂信息
	QJsonObject a;
	return a;
}

void GetInfoControl::ParseGetCoursesAbstract(const QJsonObject& data)
{
	QJsonObject info, info_data;
	info["courses_abstract"] = getCoursesAbstract();
	data["data"] = info;
	Q_EMIT QueryResult(data);  // 返回查询的结果
}

void GetInfoControl::ParseGetClassInfo(const QJsonObject& data)
{
	QJsonObject info, info_data;
	info["to"] = data["from"].toObject();  // 将数据请求者标识为数据返回目标
	info["type"] = data["type"].toObject();  // 标识数据请求类型

	// 解析获取缓存需要的数据
	info_data = data["data"].toObject();
	QString course_id = info_data["course_id"].toString();
	QString class_id = info_data["class_id"].toString();

	// 获取数据并存入发送的信息中
	info_data = getClassInfo(course_id, class_id);
	data["data"] = info_data;
	Q_EMIT QueryResult(data);  // 返回查询的结果
}

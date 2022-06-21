#include "Student.h"
#include <QJsonObject>
#include <QJsonArray>
#include "Course.h"

QJsonObject Student::getInfo() const
{
	QJsonObject student_info;
	student_info["id"] = user_id_;
	student_info["name"] = name_;
	QJsonArray course_array = getCoursesInfo();  // 构建课程信息数组
	student_info["courses_info"] = course_array;  // 将课程信息数组加入学生信息中

	return student_info;
}

QJsonArray Student::getCoursesInfo() const
{
	QJsonArray course_array;  // 构建课程信息数组
	for (auto course : courses_) {
		QJsonObject course_object, course_info;
		course_info = course->getInfo();
		course_object["id"] = course_info["id"];
		course_object["name"] = course_info["name"];
		course_object["classes_info"] = course_info["classes_info"];
		course_array.append(course_object);
	}

	return course_array;
}

QJsonArray Student::getCoursesAbstract() const
{
	QJsonArray course_array;  // 构建课程信息数组
	for (auto course : courses_) {
		QJsonObject course_object;
		course_object = course->getAbstract();
		course_array.append(course_object);
	}

	return course_array;
}

QJsonObject Student::getClassesInfo(QString class_id) const
{
	QJsonObject class_info;
	for (auto course : courses_) {
		if (course->ClassIsExit(class_id)) {
			class_info = course->getClassesInfo(class_id);
		}
	}
	return class_info;
}

void Student::AddCourse(const QJsonObject& data)
{
	// 解析学生的课程id
	QString course_id = data["id"].toString();
	// 存入学生已选的课程
	Course* course = new Course();
	courses_[course_id] = course;
	// 存入课程相应的课程信息
	courses_[course_id]->setInfo(data);
}

void Student::setClass(const QJsonObject& data)
{
	QString course_id = data["id"].toString();
	QJsonArray class_info = data["class_info"].toArray();
	QJsonObject object;
	for (auto member : class_info) {
		object = member.toObject();
		courses_[course_id]->setClass(object);
	}

}

void Student::DeleteCourse(const QJsonObject& data)
{
	QString course_id = data["id"].toString();
	for (auto i = courses_.begin(); i != courses_.end(); i++) {
		if (i.key() == course_id) {
			i.value()->~Course();
			break;
		}
	}
}

QString Student::getUserId()
{
	return user_id_;
}

void Student::setUserId(const QString& id)
{
	user_id_ = id;
}

QString Student::getName()
{
	return name_;
}

void Student::setName(const QString& name)
{
	name_ = name;
}

void Student::setInfo(const QJsonObject& data)
{
	user_id_ = data["user_id"].toString();
	name_ = data["name"].toString();
	QJsonArray course_array;  // 解析课程信息数组
	course_array = data["courses_info"].toArray();
	for (auto c : course_array) {
		QJsonObject course_info;
		course_info = c.toObject();
		// 解析学生的课程id
		QString course_id = course_info["id"].toString();
		// 存入学生已选的课程
		Course* course = new Course();
		courses_[course_id] = course;
		// 存入课程相应的课程信息
		courses_[course_id]->setInfo(course_info);
	}
}

Student& Student::GetStudent()
{
	static Student student;

	return student;
}
#pragma once
#include <QObject>
class Student;

class SetInfoControl :QObject
{
public:
	SetInfoControl();
	~SetInfoControl();

	// 解析想要改变的信息类型
	void ParseChangeType(const QJsonObject& data);
private:
	// 设置学生信息
	void SetStudentInfo(const QJsonObject& data);

	// 添加课堂信息
	void AddClassInfo(const QJsonObject& data);
private:
	Student* student_ = nullptr;  // 学生相关的信息类
};

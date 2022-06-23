/*********************************************************************************
  *Date:  2022.06.19
  *Description:  存放信息控制类，
  *				 主要用于将缓存信息存入实体类，从而避免每次都向服务端索取信息。
**********************************************************************************/
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

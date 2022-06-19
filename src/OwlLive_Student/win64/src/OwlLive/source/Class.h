/*********************************************************************************
  *Date:  2022.06.19
  *Description:  课堂实体类，
  *				 主要用于存取课堂的信息，暂时未使用。
**********************************************************************************/
#pragma once
#include <QString>
#include <QHash>
#include "Types.h"
class QJsonObject;
class QJsonArray;

class Class
{
public:
	// 获取课堂信息
	QJsonObject getInfo() const;

	// 获取课程成员概要信息
	QJsonArray getClassMemberAbstract() const;

	// 添加一个课堂成员概要信息
	void AddAMemberAbstract(QJsonObject& data);

	// 删除一个课堂成员概要信息
	void DeleteAMemberAbstract(QJsonObject& data);

	// 获取课堂id
	QString getId();

	// 设置课堂id
	void setId(const QString& id);

	// 获取推流地址
	QString getStreamAddress();

	// 设置推流地址
	void setStreamAddress(const QString& address);

	// 设置课堂信息
	void setInfo(const QJsonObject& data);

	Class() = default;
	~Class();
private:
	QHash<QString, InfoStruct::ClassMemberAbstract*> members_abstract_;  // 课堂成员简要信息列表，key为加入课堂成员的id
	QString id_ = "";  // 课堂号
	QString stream_address_ = "";  // 课堂推流地址
};


#include "Class.h"
#include <QJsonObject>
#include <QJsonArray>

QJsonObject Class::getInfo() const
{
	QJsonObject class_info;
	class_info["id"] = id_;
	class_info["stream_address_"] = stream_address_;
	class_info["members_abstract"] = getClassMemberAbstract();
	return class_info;
}

QJsonArray Class::getClassMemberAbstract() const
{
	QJsonObject object;
	QJsonArray members;
	for (auto member : members_abstract_) {
		object["id"] = member->id;
		object["identification"] = member->identification;
		object["name"] = member->name;
		members.append(object);
	}
	return members;
}

void Class::AddAMemberAbstract(QJsonObject& data)
{
	QString member_id = data["id"].toString();
	// 存入课堂成员概要信息
	InfoStruct::ClassMemberAbstract* tmp_member = new InfoStruct::ClassMemberAbstract();
	members_abstract_[member_id] = tmp_member;
	// 存入成员概要信息的数据
	members_abstract_[member_id]->id = member_id;
	members_abstract_[member_id]->identification = data["identification"].toString();
	members_abstract_[member_id]->name = data["name"].toString();
}

void Class::DeleteAMemberAbstract(QJsonObject& data)
{
	QString member_id = data["id"].toString();
	for (auto i = members_abstract_.begin(); i != members_abstract_.end(); i++) {
		if (member_id == i.value()->id) {
			delete i.value();
			members_abstract_.erase(i);
			break;
		}
	}
}

QString Class::getStreamAddress()
{
	return stream_address_;
}

void Class::setStreamAddress(const QString& address)
{
	stream_address_ = address;
}

QString Class::getId()
{
	return id_;
}

void Class::setId(const QString& id)
{
	id_ = id;
}

void Class::setInfo(const QJsonObject& data)
{
	id_ = data["id"].toString();
	stream_address_ = data["stream_address"].toString();

	QJsonArray members_array;  // 解析课堂信息数组
	members_array = data["member_abstract"].toArray();
	for (auto member : members_array) {
		QJsonObject member_abstract = member.toObject();
		// 解析课堂成员id
		QString member_id = member_abstract["id"].toString();
		// 存入课堂成员概要信息
		InfoStruct::ClassMemberAbstract* tmp_member = new InfoStruct::ClassMemberAbstract();
		members_abstract_[member_id] = tmp_member;
		// 存入成员概要信息的数据
		members_abstract_[member_id]->id = member_id;
		members_abstract_[member_id]->identification = member_abstract["identification"].toString();
		members_abstract_[member_id]->name = member_abstract["name"].toString();
	}
}

Class::~Class()
{
	for (auto member : members_abstract_) {
		delete member;
	}
}


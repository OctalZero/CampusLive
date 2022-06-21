#pragma once
#include <QObject>
class QJsonObject;
class GetInfoControl;
class SetInfoControl;

class InfoControl : public QObject
{
	Q_OBJECT
public:
	InfoControl();
	~InfoControl();

signals:
	// 返回查询信息的结果
	void ReturnQueryResults(const QJsonObject& query_results);

public slots:
	// 解析对缓存信息的操作
	void ParseOperationType(const QJsonObject& data);

private:
	GetInfoControl* get_info_control_;  // 获取学生信息类
	SetInfoControl* set_info_control_ = nullptr;  // 设置学生信息类
};


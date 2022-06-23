#include "InfoControl.h"
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include "GetInfoControl.h"
#include "SetInfoControl.h"
#include "Types.h"

InfoControl::InfoControl()
	: get_info_control_(new GetInfoControl()), set_info_control_(new SetInfoControl())
{
	// 连接与信息控制类有关的所有信号与信号
	connect(get_info_control_, &GetInfoControl::QueryResult, this, &InfoControl::ReturnQueryResults);
}

InfoControl::~InfoControl()
{
	if (get_info_control_) {
		delete  get_info_control_;
		get_info_control_ = nullptr;
	}
	if (set_info_control_) {
		delete set_info_control_;
		set_info_control_ = nullptr;
	}
}

void InfoControl::ParseOperationType(const QJsonObject& data)
{
	int to = data["to"].toInt();  // 标识信息传向哪里
	switch (to) {
	case MessageForwordControl::GetInfo:
		get_info_control_->ParseQueryType(data);
		break;
	case MessageForwordControl::SetInfo:
		set_info_control_->ParseChangeType(data);
		break;
	}
}








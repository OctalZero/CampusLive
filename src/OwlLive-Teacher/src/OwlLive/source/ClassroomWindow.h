#pragma once
#include "OwlLiveWindow.h"
#include "ui_Classroom.h"
#include "ui_LiveConfig.h"
#include <mutex>
class OwlPlay;
class OwlChat;
class PushStream;
class SDLPlay;

class ClassroomWindow : public OwlLiveWindow
{
	Q_OBJECT

public:
	ClassroomWindow(OwlLiveWindow* parent = Q_NULLPTR);
	~ClassroomWindow();

	// 更新聊天框的聊天信息
	void UpdateChatInfo(const QJsonObject& data);

	// 初始化课堂成员列表的成员信息
	void InitClassMembers(const QJsonArray& data);

    // 配置直播参数
    void ConfigLive();

    // 循环向界面输入视频帧
    void InputVideoFrame();

    // 打开推流
    void OpenPushStream(const QJsonObject& data);

    // 开始推流
    void InitPushStream(std::string& stream_address, int& select);

signals:
	// 发送客户端的请求信息的信号
	void SendClientRequestInfo(const QJsonObject& data);

	// 转发聊天信息到聊天框
	void ForwardChatMessage(const QJsonObject& data);

    // 发送聊天信息
    void SendChatMessage(const QJsonObject& data);

    // 退出课堂
    void ExitClassroom();

    // 发送确认推流源
    void ConfirmSource(int& source);

private slots:
    // 判断直播的配置信息
    void JudgeLiveConfig();
private:
    // 关闭直播配置窗口
    void CloseLiveConfigWindow();
private:
	Ui::Classroom* ui_classroom_ = nullptr;  // 教室UI
    SDLPlay *sdl_play_ = nullptr;
	OwlChat* owl_chat_ = nullptr;  // 聊天框类
    Ui::LiveConfig* ui_live_config_ = nullptr;  // 选择推流源UI
    QWidget* live_config_ = nullptr;  // 选择推流源界面实体
    std::mutex play_mutex_;  // 读视频帧的互斥量
    PushStream* push_stream_ = nullptr;
    bool exit_ = false;
};

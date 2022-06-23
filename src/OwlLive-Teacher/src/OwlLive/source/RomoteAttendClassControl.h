#pragma once

#include <QObject>
class InfoControl;
class WindowsControl;
class ClientSocket;

class RemoteAttendClassControl : public QObject
{
	Q_OBJECT
public:
	RemoteAttendClassControl();
	~RemoteAttendClassControl();

	// 启动软件
	void Start();

	// TEST:用于测试的函数
	void Test();

private:
	InfoControl* info_control_ = nullptr;  // 信息控制类
	WindowsControl* windows_control_ = nullptr;  // 界面控制类
	ClientSocket* client_socket_ = nullptr;  // 客户端通信套接字
};


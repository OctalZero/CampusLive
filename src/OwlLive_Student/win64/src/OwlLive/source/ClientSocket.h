/*********************************************************************************
  *Date:  2022.06.19
  *Description:  套接字类，
  *				 主要用于与服务端进行网络通信。
**********************************************************************************/
#pragma once
#include<QObject>
class QTcpSocket;
class QJsonObject;

class ClientSocket : public QObject
{
	Q_OBJECT
public:
	// 连接到服务器，处于连接状态时将重连
	void ConnectToHost();

	// 单例模式，获取单个实例
	static ClientSocket& GetClientSocket();
	ClientSocket(const ClientSocket&) = delete;
	ClientSocket& operator=(const ClientSocket&) = delete;
	~ClientSocket();
signals:
	// 转发来自服务端消息的信号
	void ForwardServerMessage(const QJsonObject& data);

public slots:
	// Socket对发送消息进行封装的槽
	void SendClientMessage(const QJsonObject& message);
private slots:
	// 接收服务端新的消息并进行转发的槽
	void ReadServerInfo();

	// 成功连接到服务器的槽
	void Connected();

	// 与服务器断开连接的槽
	void Disconnected();

	// 处理服务器多次连接不上的问题
	void ConnectFailed();
private:
	ClientSocket();

private:
	QTcpSocket* tcp_socket_ = nullptr;  // TCP套接字
	QString server_ip_ = "";  // 服务器的ip
	quint16 server_port_ = 0; // 服务器的端口
};


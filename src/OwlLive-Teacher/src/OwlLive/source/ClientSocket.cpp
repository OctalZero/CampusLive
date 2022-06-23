#include "ClientSocket.h"
#include <QTcpSocket>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <string>
#include "Types.h"
#include "simpleini/SimpleIni.h"

//const char* ConfigFile = "../src/OwlLive/config/config.ini";  // 配置文件的相对路径

void ClientSocket::ConnectToHost()
{
	if (tcp_socket_->isOpen()) {
		tcp_socket_->abort();
	}
	tcp_socket_->connectToHost(server_ip_, server_port_);
	tcp_socket_->waitForConnected(1000);
}

void ClientSocket::SendClientMessage(const QJsonObject& message)
{
	// 容错，确保连接上服务器，多次连接不上则发送连接失败信号
	for (int i = 0; i < 3; i++) {
		if (!tcp_socket_->isOpen()) {
			ConnectToHost();
		}
		else {
			break;
		}
		if (i == 2) {
			ConnectFailed();
			return;
		}
	}
	qDebug() << "SendMessage!";
	QJsonDocument json_document;
	json_document.setObject(message);

	tcp_socket_->write(json_document.toJson(QJsonDocument::Compact));
}


void ClientSocket::ReadServerInfo()
{
	QByteArray read_data = tcp_socket_->readAll();

	// 将接收到的Json数据存入JsonDocument中
	QJsonParseError json_error;
	QJsonDocument json_doucment = QJsonDocument::fromJson(read_data, &json_error);

	// 解析发生错误
	if (json_error.error != QJsonParseError::NoError) {
		qDebug() << "QJsonParseError!";
	}
	// 解析为空
	if (json_doucment.isNull()) {
		qDebug() << "json_doucment is Null!";
	}
	// 解析未发生错误
	if (!json_doucment.isNull() && (json_error.error == QJsonParseError::NoError))
		if (json_doucment.isObject()) {
			QJsonObject server_info = json_doucment.object();
			qDebug() << "server_info：" << server_info;
			Q_EMIT ForwardServerMessage(server_info);
		}
}

void ClientSocket::ConnectFailed()
{
	//QJsonObject data;
	//data["type"] = ServerInfo::ConnectFailed;

	//Q_EMIT ForwardServerMessage(data);
	qDebug() << "连接失败！";
}

ClientSocket::ClientSocket()
	: tcp_socket_(new QTcpSocket)
{
//	CSimpleIniA ini;
//	int res = ini.LoadFile(ConfigFile);
//	if (res < 0)
//		qDebug() << "读取配置文件失败！";
//	server_ip_ = QString::fromStdString(ini.GetValue("SERVER", "ip"));
//	server_port_ = ini.GetLongValue("SERVER", "port");
	qDebug() << "server_ip_" << server_ip_;
	qDebug() << "server_port_" << server_port_;

    server_ip_ = "192.168.43.245";
//    server_ip_ = "127.0.0.1";
    server_port_ = 8888;

	// 连接与客户端套接字有关的信号与槽
	connect(tcp_socket_, &QTcpSocket::readyRead, this, &ClientSocket::ReadServerInfo);
	connect(tcp_socket_, &QTcpSocket::connected, this, &ClientSocket::Connected);
	connect(tcp_socket_, &QTcpSocket::disconnected, this, &ClientSocket::Disconnected);

}

// 单例模式，获取单个实例
ClientSocket& ClientSocket::GetClientSocket()
{
	static ClientSocket socket;	 // 静态变量，多次访问只会创建一次

	return socket;
}

ClientSocket::~ClientSocket()
{
	if (tcp_socket_) {
		delete tcp_socket_;
		tcp_socket_ = nullptr;
	}
}

// 成功连接到服务器的槽
void ClientSocket::Connected() {
	qDebug() << "ConnectSever";
}

// 与服务器断开连接的槽
void ClientSocket::Disconnected() {
	tcp_socket_->abort();
	qDebug() << "DisConnectServer";
}


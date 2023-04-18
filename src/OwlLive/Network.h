#ifndef NETWORK_H
#define NETWORK_H
#include <QObject>
#include <winsock2.h>
#include <windows.h>
#include <boost/asio.hpp>
#include <QThread>
#include <QString>
#include <string>

using boost::asio::ip::tcp;

class Network : public QThread {
  Q_OBJECT
 public:
  // 连接到服务器
  void ConnectToHost();
  // 读取包长度
  void ReadLength();
  // 读取包数据
  void ReadData();
  // 处理线程
  void run() override;

  // 单例模式，获取单个实例
  static Network& GetNetwork();
  Network(const Network&) = delete;
  Network& operator=(const Network&) = delete;
  ~Network();

 private:
  Network();
  // 异步连接的回调函数
  void HandleConnect(const boost::system::error_code& error);
  // 异步读取包长度的回调函数
  void HandleReadLength(const boost::system::error_code& error);
  // 异步读取包数据的回调函数
  void HandleReadData(const boost::system::error_code& error);
  // 异步发送包的回调函数
  void HandleSend(const boost::system::error_code& error);
 signals:
  void ServerMessage(const QString& message);
 public slots:
  // 发送消息
  void Send(const QString& message);


 private:
  bool exit_ = false;  // 线程退出表示
  boost::asio::io_service io_service_;
  tcp::socket* tcp_socket_ = nullptr;  // tcp套接字
  uint32_t len_ = 0;  // 读取到数据的长度
  std::string data_ = "";  // 读取到包的数据
  std::string server_ip_ = "";  // 服务器的ip
  std::string server_port_ = ""; // 服务器的端口
};

#endif // NETWORK_H

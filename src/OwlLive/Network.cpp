#include "Network.h"
#include <string>
#include <iostream>
#include <boost/bind/bind.hpp>
#include "simpleini/SimpleIni.h"
#include <QDebug>
const char* ConfigFile = "../../config/config.ini";  // 配置文件的相对路径

void Network::ConnectToHost() {
  tcp::resolver resolver(io_service_);
  tcp::resolver::query query(server_ip_, server_port_);
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  boost::asio::async_connect(*tcp_socket_, endpoint_iterator,
                             boost::bind(&Network::HandleConnect, this, boost::asio::placeholders::error));
  std::cout << "ConnectToHost()" << std::endl;
}

void Network::ReadLength() {
  boost::asio::async_read(*tcp_socket_, boost::asio::buffer(&len_, sizeof(len_)),
                          boost::bind(&Network::HandleReadLength, this, boost::asio::placeholders::error));
  std::cout << "ReadLength()" << std::endl;

}

void Network::ReadData() {
  boost::asio::async_read(*tcp_socket_, boost::asio::buffer(data_),
                          boost::bind(&Network::HandleReadData, this, boost::asio::placeholders::error));
  std::cout << "ReadData()" << std::endl;
}

void Network::Send(const QString& message) {
  qDebug() << "Send():" << message ;
//  std::cout << "Send():" << message << std::endl;
////  message = "Hello, world!";
  std::string msg = message.toStdString();
  uint32_t len = htonl(msg.length());

  std::vector<boost::asio::const_buffer> buffers{
    boost::asio::buffer(&len, sizeof(len)),
    boost::asio::buffer(msg.c_str(), msg.size()),
  };

  boost::asio::async_write(*tcp_socket_, buffers,
                           boost::bind(&Network::HandleSend, this, boost::asio::placeholders::error));

}

void Network::run() {
  // 连接服务器
  ConnectToHost();

  std::cout << "io->run()" << std::endl;
  io_service_.run();
  std::cout << "io->over()" << std::endl;
  exec();
  std::cout << "network->over()" << std::endl;
}

Network::Network() {
  tcp_socket_ = new tcp::socket(io_service_);
  CSimpleIniA ini;
  int res = ini.LoadFile(ConfigFile);
  if (res < 0)
    std::cout << "读取配置文件失败！" << std::endl;
  server_ip_ = ini.GetValue("SERVER", "ip");
  server_port_ = ini.GetValue("SERVER", "port");
  std::cout << "server_ip_" << server_ip_ << std::endl;
  std::cout << "server_port_" << server_port_ << std::endl;
}

void Network::HandleConnect(const boost::system::error_code& error) {
  if (!error) {
    std::cout << "Connected to server." << std::endl;
    ReadLength();
  } else {
    std::cerr << "Connect failed: " << error.message() << std::endl;
  }
}

void Network::HandleReadLength(const boost::system::error_code& error) {
  if (!error) {
    uint32_t len = ntohl(len_);
    std::cout << "len:" << len << std::endl;
    data_.resize(len);
    ReadData();
  } else {
    std::cerr << "Read length failed: " << error.message() << std::endl;
//    ReadLength();
  }
}

void Network::HandleReadData(const boost::system::error_code& error) {
  if (!error) {
    std::cout << "Received message: " << data_ << std::endl;
    ReadLength();
    emit ServerMessage(QString::fromStdString(data_));
  } else {
    std::cerr << "Read data failed: " << error.message() << std::endl;
  }
}

void Network::HandleSend(const boost::system::error_code& error) {
  if (!error) {
    std::cout << "Message sent." << std::endl;
  } else {
    std::cerr << "Write failed: " << error.message() << std::endl;
  }
}

// 单例模式，获取单个实例
Network& Network::GetNetwork() {
  static Network network;   // 静态变量，多次访问只会创建一次

  return network;
}

Network::~Network() {
  if (tcp_socket_) {
    delete tcp_socket_;
    tcp_socket_ = nullptr;
  }
  exit_ = true;
}

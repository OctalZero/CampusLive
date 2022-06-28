/*****************************************************************************
  *Date:  2022.06.13
  *Description:  用户类，
                 主要包含TCP套接字
                 发送和接收客户端的消息
*****************************************************************************/
#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QHash>

class User:public QObject
{
    Q_OBJECT
public:
    User(QObject *parent = nullptr, QTcpSocket *tcp_socket = nullptr);

    // 获取用户id
    QString getUserId() const;

    // 获取用户姓名
    QString getUserName() const;

    // 获取用户身份信息
    int getUserIdentification() const;

    // 发送消息到client
    void SendMessage(const int type, const int to,const QJsonObject &data);

    // 处理用户请求主界面
    void HandleUserInitHomepage();

    // 处理用户刷新课程状态
    void HandleUserRfreshCourseStatus();

    ~User();
signals:
    // 与服务器建立连接
    void ConnectedServer();

    // 与服务器断开连接
    void DisConnectedServer();

    // 用户加入课堂
    void UserJoinClassServer(const QString &course_id);

    // 教师加入课堂
    void TeacherJoinClassServer(const QString &course_id);

    // 将聊天信息发送到服务器
    void ClassroomChatServer(const QString &course_id,const QJsonObject &data);

    // 用户退出课堂
    void UserExitClassroomServer(const QString &course_id);

    // 教师关闭课堂
    void UserCloseClassroomServer(const QString &course_id);

    // 教师删除课程
    void UserDeleteCourseServer(const QString &course_id);
private slots:
    // 处理客户端连接信号的槽函数
    void ClientConnected();

    // 处理客户端断开连接信号的槽函数
    void ClientDisconnected();

    // 处理接受客户端数据的槽函数
    void ClientReadyRead();

private:
    // 处理登陆界面
    void HandleMessageLogin(const int &type,const QJsonObject &data);

    // 处理注册界面
    void HandleMessageRegister(const int &type,const QJsonObject &data);

    // 处理主界面
    void HandleMessageHomepage(const int &type,const QJsonObject &data);

    // 处理课堂界面
    void HandleMessageClassroom(const int &type,QJsonObject &data);

    // 处理用户登陆
    void HandleUserLogin(const QJsonObject &data);

    // 处理用户注册
    void HandleUserRegister(const QJsonObject &data);

    // 处理用户在线
    void HandleUserOnLine(const QJsonObject &data);

    // 处理教师创建课程
    void HandleUserCreateCourse(const QJsonObject &data);

    // 处理学生加入课程
    void HandleUserJoinCourse(const QJsonObject &data);

    // 处理用户加入课堂
    void HandleUserJoinClass(const QJsonObject &data);

    // 处理学生退选课程
    void HandleUserDeselectionCourse(const QJsonObject &data);

    // 处理教师删除课程
    void HandleUserDeleteCourse(const QJsonObject &data);

    // 处理用户查询课程信息
    void HandleUserQueryCourseDetails(const QJsonObject &data);

private:

    QTcpSocket *tcp_socket_;// tcp套接字

    QString id_="";// 用户id

    QString name_ = "";//用户名

    int identification_ = 0;

    int i = 0;
};

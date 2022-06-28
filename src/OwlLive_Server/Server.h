/*****************************************************************************
  *Date:  2022.06.13
  *Description:  服务器类，
                 主要监听TCP套接字并为其创建User
                 对用户进行整体性的管理（比如课堂中用户信息的传递）
*****************************************************************************/
#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QMap>
#include <map>
#include <vector>
class User;
class Server:public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject* parent = nullptr);

    ~Server();

    // 打开服务器，并监听port端口
    int Open(unsigned short port);

    // 关闭服务器
    void Close();

private slots:
    // 监测到有新连接（为TCP创建User类）
    void MonitorNewConnection();

    // 监测到用户请求建立连接
    void Connected();

    // 检测到用户请求断开连接
    void DisConnected();

    // 用户加入课堂
    void JoinTheClass(const QString &course_id);

    // 教师加入课堂
    void TeacherJoinTheClass(const QString &course_id);

    // 用户退出课堂
    void DropOutOfClass(const QString &course_id);

    // 用户进行课堂聊天
    void ClassroomChat(const QString &course_id,const QJsonObject &json);

    // 教师关闭课堂
    void CloseClassroom(const QString &course_id);

    // 教师删除课程
    void DeleteCourse(const QString &course_id);

private:
    QTcpServer *server_;// tcp服务器

    QVector<User *> TcpList_;// 保存当前建立连接的用户

    QMap<QString,QVector<User *>> ClassroomVillage_;// 课堂成员（课程号和相应课程的成员列表）
};

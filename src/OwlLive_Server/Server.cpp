#include "Server.h"
#include "Database.h"
#include "User.h"
#include "Types.h"
#include <QJsonObject>
#include <QJsonArray>

Server::Server(QObject* parent):QObject(parent)
{
    server_ = new QTcpServer(this);

    connect(server_,&QTcpServer::newConnection,this,&Server::MonitorNewConnection);
}

Server::~Server()
{

}

// 打开服务器，并监听port端口
int Server::Open(unsigned short port)
{
    Database::instance().Open("ds");
    if(server_->listen(QHostAddress::Any,port))
        return port;
    return 0;
}

// 关闭服务器
void Server::Close()
{
    Database::instance().Close();
    server_->close();
}


// 监测到有新连接
void Server::MonitorNewConnection()
{
    User *m_tcp = new User(this,server_->nextPendingConnection());
    connect(m_tcp,&User::ConnectedServer,this,&Server::Connected);
    connect(m_tcp,&User::DisConnectedServer,this,&Server::DisConnected);
    connect(m_tcp,&User::UserJoinClassServer,this,&Server::JoinTheClass);
    connect(m_tcp,&User::TeacherJoinClassServer,this,&Server::TeacherJoinTheClass);
    connect(m_tcp,&User::ClassroomChatServer,this,&Server::ClassroomChat);
    connect(m_tcp,&User::UserExitClassroomServer,this,&Server::DropOutOfClass);
    connect(m_tcp,&User::UserCloseClassroomServer,this,&Server::CloseClassroom);
    connect(m_tcp,&User::UserDeleteCourseServer,this,&Server::DeleteCourse);
    qDebug()<<"服务器监听到新连接（待连接到服务器）";
}

// 监测到用户请求建立连接
void Server::Connected()
{
    User *client = (User *)this->sender();
    if (NULL == client) return;
    TcpList_.push_back(client);
    qDebug()<<" 成功连接服务器（已成功登陆）";
}

// 检测到用户请求断开连接
void Server::DisConnected()
{
    User *client = (User *)this->sender();
    if (NULL == client) return;

    for(int i = 0; i < TcpList_.size(); i++){
        if(client == TcpList_[i]){
            TcpList_.remove(i);
        }
    }

    client->~User();// 执行析构函数

    qDebug()<<" 已成功断开服务器";
}

// 用户加入课堂
void Server::JoinTheClass(const QString &course_id)
{
    User *client = (User *)this->sender();
    if(NULL == client)return;

    qDebug()<<"用户course_id="+course_id+" 加入课堂";

    ClassroomVillage_[course_id].push_back(client);

    // 下面将课堂信息发送至新加入成员
    QJsonObject courseinfo;
    courseinfo = Database::instance().JoinClass(client->getUserId(),course_id);

    QJsonObject classinfo;
    classinfo = Database::instance().getClassInfo(course_id);

    // 将课堂成员列表保存至JsonArray中
    QJsonArray classMemberArray;
    for(auto it:ClassroomVillage_[course_id])
    {
        if(!it) // 将异常断开的成员进行删除
        {
            ClassroomVillage_[course_id].removeOne(it);
            continue;
        }
        QJsonObject userinfo;
        userinfo["name"] = it->getUserName();
        userinfo["id"] = it->getUserId();
        userinfo["identification"] = it->getUserIdentification();
        classMemberArray.push_back(userinfo);
        //qDebug()<<"课堂成员:"<<it->getUserName();
    }
    classinfo["member_abstract"] = classMemberArray;
    courseinfo["class_info"] = classinfo;

    // 向新加入的成员更新课堂信息courseinfo
    client->SendMessage(Classroom::InitClassroomResult,MessageForwordControl::Classroom,courseinfo);

    // 向已经进入的成员发送更新课堂成员信息classinfo
    for(auto it:ClassroomVillage_[course_id])
    {
        if(it!=client)
            it->SendMessage(Classroom::UpdateClassMembers,MessageForwordControl::Classroom,classinfo);
    }
}

// 教师加入课堂
void Server::TeacherJoinTheClass(const QString &course_id)
{
    // 获取该课程的选课
    QJsonObject data=Database::instance().getCourseMember(course_id);

    QJsonArray memberarray = data["course_member"].toArray();
    QVector<QString> member;
    for(auto it:memberarray)
    {
        QString id=it.toObject()["user_id"].toString();
        member.push_back(id);
    }

    // 向在线选择该课程的人发送消息
    for(auto it:TcpList_)
    {
        QString id = it->getUserId();
        bool isInCourse = false;

        for(auto m:member)
        {
            if(id==m)isInCourse=true;
        }
        if(isInCourse)it->HandleUserInitHomepage();
    }
}

// 用户退出课堂
void Server::DropOutOfClass(const QString &course_id)
{
    User *client = (User *)this->sender();
    if(NULL==client)return;
    ClassroomVillage_[course_id].removeOne(client);

    QJsonObject json;
    // 将课堂成员列表保存至JsonArray中
    QJsonArray classMemberArray;
    for(auto it:ClassroomVillage_[course_id])
    {
        if(!it) // 将异常断开的成员进行删除
        {
            ClassroomVillage_[course_id].removeOne(it);
            continue;
        }
        QJsonObject userinfo;
        userinfo["name"] = it->getUserName();
        userinfo["id"] = it->getUserId();
        userinfo["identification"] = it->getUserIdentification();
        classMemberArray.push_back(userinfo);
    }
    json["member_abstract"] = classMemberArray;

    for(auto it:ClassroomVillage_[course_id])
    {
        it->SendMessage(Classroom::UpdateClassMembers,MessageForwordControl::Classroom,json);
    }
    qDebug()<<"学生退出课堂";
}

void Server::ClassroomChat(const QString &course_id,const QJsonObject &data)
{
    qDebug()<<"课程："<<course_id<<" 服务器转发消息";
    for(auto it:ClassroomVillage_[course_id])
    {
        qDebug()<<"服务器向"+it->getUserName()+"转发一个消息";
        it->SendMessage(Classroom::UpdateChatMessage,MessageForwordControl::Classroom,data);
    }
}

// 教师关闭课堂
// 1.向所有成员发送关闭通知
// 2.将该课程状态设置为未上课
// 3.将该课程的在线成员清空
// TODO:教师关闭课堂，需向在课堂学生发送关闭课堂，需要在线选课学生发送课程状态
void Server::CloseClassroom(const QString &course_id)
{
    QJsonObject resultjson;

    // 获取该课程的选课
    QJsonObject data=Database::instance().getCourseMember(course_id);

    QJsonArray memberarray = data["course_member"].toArray();

    QVector<QString> member; // 选取该课程的学生
    for(auto it:memberarray)
    {
        QString id=it.toObject()["user_id"].toString();
        member.push_back(id);
    }
    //====================

    // 先更新
    Database::instance().UpdateCourseStatus(course_id,CourseStatus::NotInClass);

    for(auto it:ClassroomVillage_[course_id])
    {
        qDebug()<<"关闭课堂发送";
        if(it)
        {
            if(it->getUserIdentification()==ClientIdentification::Teacher)
                it->HandleUserRfreshCourseStatus();
            else
                 it->SendMessage(Classroom::CloseClassroomResult,MessageForwordControl::Classroom,resultjson);
        }
    }

    ClassroomVillage_[course_id].clear();
}

// 教师删除课程 需要向所有在线的选这门课程的同学发送消息
void Server::DeleteCourse(const QString &course_id)
{
    // 获取该课程的选课
    QJsonObject data=Database::instance().getCourseMember(course_id);

    QJsonArray memberarray = data["course_member"].toArray();

    QVector<QString> member; // 选取该课程的学生
    for(auto it:memberarray)
    {
        QString id=it.toObject()["user_id"].toString();
        member.push_back(id);
    }

    QJsonObject teacherinfo = Database::instance().getCourseDetailsInfo(course_id);
    QString teacher_id = teacherinfo["teacher_id"].toString();
    qDebug()<<"teacher_info="<<teacherinfo;

    // 删除课程
    QJsonObject resjson = Database::instance().DeleteCourse(course_id);
    // 向在线选择该课程的人发送消息

    for(auto it:TcpList_)
    {
        QString id = it->getUserId();
        bool isHasCourse = false;
        if(id==teacher_id)
        {
            qDebug()<<"向教师发送"<<teacher_id;
            it->SendMessage(Homepage::DeleteCourseResult,MessageForwordControl::Homepage,resjson);
            continue;
        }

        for(auto m:member)
        {
            if(id==m)isHasCourse=true;
        }
        if(isHasCourse)it->HandleUserInitHomepage();
    }
}

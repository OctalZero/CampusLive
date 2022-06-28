#include <QSqlQuery>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QMetaMethod>
#include <QSemaphore>
#include <QJsonArray>
#include <QDateTime>
#include <QHash>
#include "User.h"
#include "Database.h"
#include "Types.h"

User::User(QObject *parent, QTcpSocket *tcp_socket):QObject(parent)
{
    id_ = "";

    if (tcp_socket == nullptr) tcp_socket_ = new QTcpSocket(this);
    tcp_socket_ = tcp_socket;

    // 将数据库信息加载到当前
    // id , course

    // 连接套接字的信号与处理的槽函数
    connect(tcp_socket_,&QTcpSocket::readyRead, this, &User::ClientReadyRead);
    connect(tcp_socket_,&QTcpSocket::connected,this,&User::ClientConnected);
    connect(tcp_socket_,&QTcpSocket::disconnected,this,&User::ClientDisconnected);

}

// 获取用户id
QString User::getUserId() const
{
    return id_;
}

QString User::getUserName() const
{
    return name_;
}

int User::getUserIdentification() const
{
    return identification_;
}

// 发送消息到client
void User::SendMessage(const int type, const int to,const QJsonObject &data)
{
    if(tcp_socket_->isOpen()){
        QJsonObject jsonobj;
        jsonobj["type"] = type;
        jsonobj["data"] = data;
        jsonobj["to"] = to;
        QJsonDocument document;
        document.setObject(jsonobj);
        qDebug()<<"发送到User "<<getUserId()<<" 的包："<<jsonobj;
        tcp_socket_->write(document.toJson(QJsonDocument::Compact));
    }
}

User::~User()
{

}

// 处理客户端连接信号的槽函数
void User::ClientConnected()
{
    Q_EMIT ConnectedServer();
}

// 处理客户端断开连接信号的槽函数
void User::ClientDisconnected()
{
    qDebug()<<"执行断开 id="<<id_;
    Database::instance().UpdateUserStatus(id_,ClientStatus::OffLine);
    Q_EMIT DisConnectedServer();
}

// 处理接受客户端数据的槽函数
void User::ClientReadyRead()
{
    if(i == 0){
        Q_EMIT ConnectedServer();
    }
    i = 1;
    QByteArray receive;
    receive = tcp_socket_->readAll();
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(receive,&jsonError);
    qDebug()<<"接收User的包:"<<receive;
    if(!document.isNull() && (jsonError.error == QJsonParseError::NoError)){
        if(document.isObject()){
            QJsonObject object = document.object();
            int type = object["type"].toInt();
            int to = object["to"].toInt();
            QJsonObject data = object["data"].toObject();
            switch (to) {
            case MessageForwordControl::Login:{ // 处理登陆界面
                HandleMessageLogin(type,data);
            }
                break;
            case MessageForwordControl::Register:{ // 处理注册界面
                HandleMessageRegister(type,data);
            }
                break;
            case MessageForwordControl::Homepage:{ // 处理主界面
                HandleMessageHomepage(type,data);
            }
                break;
            case MessageForwordControl::Classroom:{ // 处理教室界面
                HandleMessageClassroom(type,data);
            }
                break;
            default:
                break;
            }
        }

    }
}

void User::HandleMessageLogin(const int &type,const QJsonObject &data)
{
    switch (type) {
    case Login::Login:{
        qDebug()<<"======处理登陆界面======";
        HandleUserLogin(data);
    }
        break;
    }
}

void User::HandleMessageRegister(const int &type,const QJsonObject &data)
{
    switch (type) {
    case Register::Register:{
        qDebug()<<"======处理注册======";
        HandleUserRegister(data);
    }
        break;
    }
}

void User::HandleMessageHomepage(const int &type,const QJsonObject &data)
{
    switch (type) {
    case Homepage::InitHomepage:{
        qDebug()<<"======处理初始化主界面======";
        HandleUserInitHomepage();
    }
        break;
    case Homepage::AddCourse:{
        qDebug()<<"======处理学生加入课程======";
        HandleUserJoinCourse(data);
    }
        break;
    case Homepage::CreateCourse:{
        qDebug()<<"======处理教师创建课程======";
        HandleUserCreateCourse(data);
    }
        break;
    case Homepage::DeselectionCourse:{
        qDebug()<<"======处理学生退选课程======";
        HandleUserDeselectionCourse(data);
    }
        break;
    case Homepage::DeleteCourse:{
        qDebug()<<"======处理教师删除课程======";
        HandleUserDeleteCourse(data);
    }
        break;
    case Homepage::QueryCourseDetails:{
        qDebug()<<"======处理用户查询课程详细信息======";
        HandleUserQueryCourseDetails(data);
    }
        break;
    case Homepage::RfreshCourseStatus:{
        qDebug()<<"======处理用户刷新课程状态======";
        HandleUserRfreshCourseStatus();
    }
        break;
    }
}

void User::HandleMessageClassroom(const int &type,QJsonObject &data)
{
    switch (type) {
    case Classroom::InitClassroom:{
        qDebug()<<"======处理初始化课堂界面======";
        HandleUserJoinClass(data);
    }
        break;
    case Classroom::SendChatMessage:{
        qDebug()<<"======处理课堂发送消息======";
        const QString course_id = data["course_id"].toString();
        //qDebug()<<"course_id=="<<cid;
        data.insert("name",getUserName());
        Q_EMIT ClassroomChatServer(course_id,data);
    }
        break;
    case Classroom::ExitClassroom:{ // 退出课堂
        qDebug()<<"======处理退出课堂======";
        const QString course_id = data["course_id"].toString();
        Q_EMIT UserExitClassroomServer(course_id);
    }
        break;
    case Classroom::CloseClassroom:{
        qDebug()<<"======处理关闭课堂======";
        const QString course_id = data["course_id"].toString();
        Q_EMIT UserCloseClassroomServer(course_id);
    }
        break;
    }
}

// 处理用户登录
void User::HandleUserLogin(const QJsonObject& data)
{
    QString id = data["user_id"].toString();
    id_ = id;

    QString userPassword = data["password"].toString();
    int identification = data["identification"].toInt();
    QJsonObject json;
    json = Database::instance().Login(id, userPassword,identification);

    SendMessage(Login::LoginResult,MessageForwordControl::Login ,json);
}

// 处理用户注册
void User::HandleUserRegister(const QJsonObject& data)
{
    QString userName = data["user_name"].toString();
    QString userPassword = data["password"].toString();
    int identification = data["identification"].toInt();

    id_ = Database::instance().Register(userName, userPassword,identification);
    QJsonObject json;
    if (id_ != "") {// 注册成功
        json["user_id"] = id_;
        json["result"] = "success";
        json["reson"] = "";
    }else { // 如果已经注册
        json["result"] = "failed";
        json["reason"] = "The account has been logged in";
    }
    SendMessage(Register::RegisterResult,MessageForwordControl::Register, json);
}

// 处理用户请求主界面信息
void User::HandleUserInitHomepage()
{
    QJsonObject sendobj;
    QJsonObject userinfojson;
    userinfojson = Database::instance().getUserInfo(id_);

    // 将用户信息保存至User变量中
    name_ = userinfojson["name"].toString();
    id_ = userinfojson["user_id"].toString();
    identification_ = userinfojson["identification"].toInt();

    sendobj["user_info"] = userinfojson;

    SendMessage(Homepage::InitHomepageResult,MessageForwordControl::Homepage, sendobj);
}

// 处理用户在线
void User::HandleUserOnLine(const QJsonObject &data)
{

}

// 处理教师创建课程
void User::HandleUserCreateCourse(const QJsonObject &data)
{
    QString course_name = data["course_name"].toString();

    QJsonObject resultinfo;
    resultinfo = Database::instance().CreateCourse(getUserId(),course_name);

    SendMessage(Homepage::CreateCourseResult,MessageForwordControl::Homepage,resultinfo);
}

// 处理学生加入课程
void User::HandleUserJoinCourse(const QJsonObject &data)
{
    QString course_id = data["course_id"].toString();

    QJsonObject resultinfo;
    resultinfo = Database::instance().JoinCourse(getUserId(),course_id);
    SendMessage(Homepage::AddCourseResult,MessageForwordControl::Homepage,resultinfo);
}

// 处理用户加入课堂
void User::HandleUserJoinClass(const QJsonObject &data)
{
    QString course_id=data["course_id"].toString();
    if(course_id == "")
    {
        qDebug()<<"course_id为空";
        return;
    }

    Q_EMIT UserJoinClassServer(course_id);
    if(getUserIdentification()==ClientIdentification::Teacher)
    {
        //HandleUserInitHomepage();
        Q_EMIT TeacherJoinClassServer(course_id);
    }
}

// 处理学生退选课程
void User::HandleUserDeselectionCourse(const QJsonObject &data)
{
    QString course_id = data["course_id"].toString();

    QJsonObject resultjson;
    resultjson=Database::instance().DeselectionCourse(getUserId(),course_id);


    SendMessage(Homepage::DeselectionCourseResult,MessageForwordControl::Homepage,resultjson);
}

// 处理教师删除课程
void User::HandleUserDeleteCourse(const QJsonObject &data)
{
    QString course_id = data["course_id"].toString();

    //QJsonObject resultjson;
    //resultjson = Database::instance().DeleteCourse(course_id);

    Q_EMIT UserDeleteCourseServer(course_id);

    //SendMessage(Homepage::DeleteCourseResult,MessageForwordControl::Homepage,resultjson);
}

// 处理用户查询课程信息
void User::HandleUserQueryCourseDetails(const QJsonObject &data)
{
    QString course_id = data["course_id"].toString();

    QJsonObject resultjson;

    resultjson = Database::instance().getCourseDetailsInfo(course_id);
    resultjson["course_id"] = course_id;
    SendMessage(Homepage::QueryCourseDetailsResult,MessageForwordControl::Homepage,resultjson);
}

// 处理用户刷新课程状态
void User::HandleUserRfreshCourseStatus()
{
    QJsonObject json = Database::instance().getUserInfo(getUserId());

    QJsonArray coursearray = json["courses_info"].toArray();

    QJsonObject resjson;
    resjson["courses_info"] = coursearray;
    SendMessage(Homepage::RfreshCourseStatusResult,MessageForwordControl::Homepage,resjson);
}

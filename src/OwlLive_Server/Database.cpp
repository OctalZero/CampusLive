#include "Database.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QSqlQuery>
// ip
#include <stdio.h>
#include <ifaddrs.h>
#include <string.h>
#include <arpa/inet.h>

#include "Types.h"
// 打开数据库
bool Database::Open(const QString &databaseName)
{
    UserDb_ = QSqlDatabase::addDatabase("QMYSQL");
    // 以下设置依据具体的数据库配置
    UserDb_.setHostName("localhost");
    UserDb_.setPort(3306);
    UserDb_.setUserName("root");
    UserDb_.setPassword("123456"); // 修改过后的密码
    UserDb_.setDatabaseName("mariadb"); // 自己通过命令行创建

    if(!UserDb_.open())return false;

    //    UserDb_ = QSqlDatabase::addDatabase("QSQLITE");
    //    UserDb_.setDatabaseName(databaseName);
    //    UserDb_.setHostName("127.0.0.1");
    //    UserDb_.setUserName("root");
    //    UserDb_.setPassword("123456");
    //    if(!UserDb_.open())return false;

    QSqlQuery query;
    // 建用户表
    query.exec("CREATE TABLE USERINFO (uid varchar(20) PRIMARY KEY, uname varchar(20), "
               "upassword varchar(20),status INT,identification INT);");

    // 建课程表
    query.exec("CREATE TABLE COURSEINFO (cid varchar(20) PRIMARY KEY, cname varchar(20), tid varchar(20),status INT,address varchar(200));");


    // 建选课表
    query.exec("CREATE TABLE UC (uid varchar(20) , cid varchar(20), PRIMARY KEY(uid,cid),FOREIGN KEY(uid) REFERENCES USERINFO(uid),FOREIGN KEY(cid) REFERENCES COURSEINFO(cid));");


    IPAddress_ = GetIPAddress();
    qDebug()<<"获取的ip地址"<<IPAddress_;

    // 将所有网民的状态置为OffLine
    query.exec("SELECT * FROM USERINFO ORDER BY uid;");
    while (query.next()) {
        MaxUserId_ = std::max(MaxUserId_,query.value(0).toInt());
        UpdateUserStatus(query.value(0).toString(),ClientStatus::OffLine);
    }

    // 处理所有课程
    query.exec("SELECT * FROM COURSEINFO ORDER BY cid;");
    while (query.next()) {
        CourseNumber_++;

        const QString course_id = query.value(0).toString();

        // 更新课程状态
        UpdateCourseStatus(course_id,CourseStatus::NotInClass);

        const QString setaddress = "rtmp://"+IPAddress_+"/live/course"+course_id;
        // 更新课程推流地址
        UpdateIPAddress(course_id,setaddress);

        // 将课程成员信息加入CoursePersonnel_
        QString selectstr="SELECT uid FROM UC WHERE cid='"+course_id+"';";
        QSqlQuery querymember;
        querymember.exec(selectstr);
        while(querymember.next())
        {
            QString user_id = query.value(0).toString();
            CoursePersonnel_[course_id].push_back(user_id);
        }

    }

    // 删除课程向添加课程的成员
    // 打印数据库信息
    PrintDatabase();
    return true;
}

// 关闭数据库
void Database::Close()
{
    UserDb_.close();
}

// 用户注册
QString Database::Register(const QString &name, const QString &password,const int &identification)
{
//    QString str = "SELECT uid FROM USERINFO ";
//    str += QString("WHERE uname='");
//    str += name;
//    str += QString("';");

//    QSqlQuery query(str);
//    if(query.next()){// 如果已经注册了
//        return "";
//    }
    QSqlQuery query;

    int id = ++MaxUserId_;
    qDebug()<<"name="<<name;
    QString str1="INSERT INTO USERINFO (uid, uname, upassword, status, identification) VALUES('"+QString::number(id)+"', '"+name+"', '"+password+"', "+QString::number(ClientStatus::OffLine)+", "+QString::number(identification)+");";
    //qDebug()<<"注册："<<str1;

    if(query.exec(str1))qDebug()<<"注册成功====";
    return QString::number(id);
}

// 用户登陆
QJsonObject Database::Login(const QString &id, const QString &password,const int identification)
{
    QString str="SELECT uid,uname,upassword,status,identification FROM USERINFO WHERE uid="+id+";";

    QString userName="";
    QString uid="";
    QString upassword="";
    QString uidentification = "";
    int ustatus=-1;


    QSqlQuery query;
    query.exec(str);
    if(query.next()){
        uid = query.value(0).toString();
        userName = query.value("uname").toString();
        upassword = query.value("upassword").toString();
        ustatus = query.value("status").toInt();
        uidentification = query.value("identification").toInt();
    }
    qDebug()<<"uidentification="<<uidentification;
    QJsonObject json;
    json.insert("name",userName);
    if(uid=="")// 如果用户不存在
    {
        json.insert("result","failed");
        json.insert("reason","Account does not exist");
    }else if(password!=upassword)// 如果密码不匹配
    {
        json.insert("result","failed");
        json.insert("reason","Password error");
    }else if(ustatus == ClientStatus::OnLine)
    {
        json.insert("result","failed");
        json.insert("reason","The account has been logged in");
    }else if(uidentification!=identification){
        json.insert("result","failed");
        json.insert("reason","Incorrect identity");
    }else{ // 如果匹配成功
        json.insert("result","success");
        json.insert("reason","");
        UpdateUserStatus(uid,ClientStatus::OnLine);
    }
    return  json;
}

QJsonObject Database::getUserInfo(const QString &user_id)
{
    QString str = "SELECT uid,uname,identification FROM USERINFO WHERE uid="+user_id+";";
    QSqlQuery query;
    query.exec(str);
    QString userName="";
    QString uid="";
    int uidentification=-1;
    if(query.next()){
        uid = query.value(0).toString();
        userName = query.value("uname").toString();
        uidentification = query.value("identification").toInt();
    }
    QJsonObject json;
    json.insert("user_id",user_id);
    json.insert("name",userName);
    json.insert("identification",uidentification);

    QString str1;

    // 查询学生的课程
    QString str2="SELECT cid ,cname,tid,status,address FROM COURSEINFO WHERE cid IN ("
    "SELECT cid FROM UC WHERE uid="+user_id+" );";

    //查询老师的课程
    QString str3="SELECT cid ,cname,tid,status,address FROM COURSEINFO WHERE tid="+user_id+";";
    if(uidentification==ClientIdentification::Student)str1=str2;
    else str1=str3;

    QJsonArray course_info;
    query.exec(str1);

    QString m_cid;
    QString m_cname;
    QString m_tid;
    QString m_address;
    int m_status;
    while (query.next()) {
        QJsonObject course;

        m_cid = query.value(0).toString();
        m_cname = query.value("cname").toString();
        m_tid = query.value("tid").toString();
        m_status=query.value("status").toInt();
        m_address = query.value("address").toString();

        course["id"] = m_cid;
        course["name"] = m_cname;
        course["tid"] = m_tid;
        course["status"] = m_status;
        course["stream_address"] = m_address;

        course_info.push_back(course);
    }
    json["courses_info"] = course_info;
    return json;
}

// 获取课堂信息
// 返回classinfo{id}
QJsonObject Database::getClassInfo(const QString &course_id)
{
    QJsonObject classinfo;
    QString classid="";

    QSqlQuery query;
    QString classtr="SELECT MAX(id) FROM COURSE"+course_id+"CLASSINFO;";
    query.exec(classtr);
    if(query.next()){
        qDebug()<<"         课堂信息查询成功";
        classid=query.value("id").toString();
    }
    classinfo["id"] = classid;

    return classinfo;
}

// 获取课程详细信息（目前只要课程id和教师姓名）
QJsonObject Database::getCourseDetailsInfo(const QString &course_id)
{
    QString querystr="SELECT uname FROM USERINFO WHERE uid in(SELECT tid FROM COURSEINFO WHERE cid = '"+course_id+"');";

    QString querystr2="SELECT tid FROM COURSEINFO WHERE cid='"+course_id+"';";


    QSqlQuery query;
    query.exec(querystr);

    QString teacher_name;
    QString teacher_id;
    qDebug()<<querystr;
    if(query.next())
    {
        teacher_name = query.value(0).toString();
        qDebug()<<"         教师名字查询成功";
    }

    query.exec(querystr2);
    qDebug()<<querystr2;
    if(query.next())
    {
        qDebug()<<"         教师id查询成功";
        teacher_id = query.value(0).toString();
    }

    QJsonObject resultjson;
    resultjson["teacher_name"] = teacher_name;
    resultjson["teacher_id"] = teacher_id;
    return resultjson;
}

QJsonObject Database::getCourseMember(const QString &course_id)
{
    QString str="SELECT uid FROM UC WHERE cid='"+course_id+"';";
    QSqlQuery query;
    query.exec(str);

    QJsonObject json;
    QJsonArray CourseMember;
    QString user_id;
    while(query.next())
    {
        QJsonObject userinfo;
        user_id=query.value("uid").toString();
        userinfo["user_id"] = user_id;
        CourseMember.push_back(userinfo);
    }
    json["course_member"] = CourseMember;
    return json;
}

// 教师创建课程
//TODO:创建课程名不能为空
QJsonObject Database::CreateCourse(const QString &tid, const QString &cname)
{
    qDebug()<<"教师创建课程";
    int id = 1;
    QSqlQuery querys("SELECT MAX(cid) FROM COURSEINFO;");

    if(querys.next()){
        id = querys.value(0).toInt();
    }
    id++;

    QString m_address = "rtmp://"+IPAddress_+"/live/course"+QString::number(id);
    CourseNumber_++;

    qDebug()<<QString::number(id,10);
    qDebug()<<cname;
    qDebug()<<tid;
    qDebug()<<m_address;
    QSqlQuery query;

    QString str="INSERT INTO COURSEINFO (cid, cname, tid, status, address) VALUES('"+QString::number(id,10)+"', '"+cname+"', '"+tid+"', "+QString::number(CourseStatus::NotInClass)+", '"+m_address+"');";

    if(query.exec(str))qDebug()<<"创建课程成功";

    // 创建课堂表 COURSE10CLASSINFO
    QString createclass="CREATE TABLE COURSE"+QString::number(id)+"CLASSINFO (id varchar(20) PRIMARY KEY);";
    query.exec(createclass);

    QJsonObject resultinfo;
    resultinfo["result"] = "success";
    return resultinfo;
}

// 学生加入课程
// 错误：1.学生加入未有课程 2.学生重复加入课程
QJsonObject Database::JoinCourse(const QString &user_id, const QString &course_id)
{
    QSqlQuery query;
    qDebug()<<"====学生加入课程 Debug====";
    // 查询学生是否有该课程
    QString querystr = "SELECT uid,cid FROM UC WHERE uid="+user_id+" and cid="+course_id+";";
    query.exec(querystr);
    if(query.next()) // 如果学生已经选取该课程
    {
        QJsonObject resultInfo;
        resultInfo["result"] = "failed";
        resultInfo["reason"] = "This course has been selected";
        return resultInfo;
    }

    QString str="SELECT cid,cname,tid,status FROM COURSEINFO WHERE cid="+course_id+";";
    query.exec(str);

    QString m_cid="";
    QString m_cname="";
    QString m_tid="";
    int m_status=-1;
    if(query.next()){
        m_cid = query.value(0).toString();
        m_cname = query.value("cname").toString();
        m_tid = query.value("tid").toString();
        m_status = query.value("status").toInt();
    }
    qDebug()<<"course_id"<<course_id;
    qDebug()<<"m_cid = = "<<m_cid;
    if(m_cid!="")
    {
        QString str2="INSERT INTO UC(uid,cid)VALUES("+user_id+","+course_id+");";
        qDebug()<<"学生加入课程:"<<str2;
        query.exec(str2);
        CoursePersonnel_[course_id].push_back(user_id);
    }

    QJsonObject coursejson;
    if(m_cid==course_id) // 加入课程成功
    {
        coursejson["result"] = "success";
        coursejson["reason"] = "";
    }else{ // 加入课程失败
        coursejson["result"] = "failed";
        coursejson["reason"] = "No current course";
    }
    return coursejson;
}

// 教师创建课堂
QJsonObject Database::CreateClass(const QString &tid, const QString &cid)
{

}

// 学生加入课堂
// 返回courseinfo{id,name,stream_address}
QJsonObject Database::JoinClass(const QString &user_id, const QString &course_id)
{
    QJsonObject classinfo = getClassInfo(user_id);
    QSqlQuery query;
    QString str="SELECT cid,cname,tid,status,address FROM COURSEINFO WHERE cid="+course_id+";";

    QString openstr = "UPDATE COURSEINFO SET status="+QString::number(CourseStatus::InClass)+" WHERE cid='"+course_id+"';";
    query.exec(openstr);

    query.exec(str);
    QString m_cid;
    QString m_cname;
    //QString m_tid;
    QString m_address;
    int m_status;
    QJsonObject courseinfo;
    if (query.next()) {
        m_cid = query.value(0).toString();
        m_cname = query.value("cname").toString();
        //m_tid = query.value("tid").toString();
        //m_status=query.value("status").toInt();
        m_address = query.value("address").toString();

        courseinfo["id"] = m_cid;
        courseinfo["name"] = m_cname;
        //courseinfo["tid"] = m_tid;
        //courseinfo["status"] = m_status;
        courseinfo["stream_address"] = m_address;
    }
    courseinfo["class_info"] = classinfo;
    return courseinfo;
}

// 退选课程
QJsonObject Database::DeselectionCourse(const QString &user_id, const QString &course_id)
{
    QString deletestr="DELETE FROM UC WHERE uid="+user_id+" AND cid="+course_id+";";
    QSqlQuery query;
    if(query.exec(deletestr))
        qDebug()<<"学生退选课程成功";

    QJsonObject resjson;
    resjson["result"]="success";
    return resjson;
}

// 教师删除课程
// 注意:教师删除的课程，对应学生的课程也应该删除
QJsonObject Database::DeleteCourse(const QString &course_id)
{

    QSqlQuery query;

    // 注意：先删选课表，再删课程表
    QString deletestr2="DELETE FROM UC WHERE cid="+course_id+";";
    if(query.exec(deletestr2))
        qDebug()<<"教师删除课程，学生的相应课程删除成功";

    QString deletestr="DELETE FROM COURSEINFO WHERE cid="+course_id+";";
    if(query.exec(deletestr))
        qDebug()<<"教师删除课程成功";

    QString deletestr3="DROP TABLE COURSE"+course_id+"CLASSINFO;";
    if(query.exec(deletestr3))
        qDebug()<<"删除课堂表";

    QJsonObject resjson;
    resjson["result"]="success";

    CoursePersonnel_[course_id].clear();
    return resjson;
}

// 教师关闭课堂
void Database::CloseClass(const QString &course_id)
{

    QString closestr = "UPDATE COURSEINFO SET status="+QString::number(CourseStatus::NotInClass)+" WHERE cid='"+course_id+"';";
    QSqlQuery query;
    if(query.exec(closestr))
        qDebug()<<course_id<<"课堂关闭成功";
}

// 更新用户状态
void Database::UpdateUserStatus(const QString &id,const int &status)
{
    QString str="UPDATE USERINFO SET status='"+QString::number(status)+"' WHERE uid="+id+";";
    //qDebug()<<str;
    QSqlQuery query(str);
    if(query.exec())
        qDebug()<<"更新用户状态成功";

}

void Database::UpdateCourseStatus(const QString &id, const int &status)
{
    QString str="UPDATE COURSEINFO SET status='"+QString::number(status)+"' WHERE cid="+id+";";
    //qDebug()<<str;
    QSqlQuery query(str);
    if(query.exec())
        qDebug()<<"更新课程状态成功";
}


// 更新课程IP
void Database::UpdateIPAddress(const QString &course_id, const QString &IP)
{
    QString setstr = "UPDATE COURSEINFO SET address='"+IP+"' WHERE cid="+course_id+";";
    //qDebug()<<setstr;
    QSqlQuery query(setstr);
    query.exec();
}

// 获取本机ip地址
QString Database::GetIPAddress()
{
    struct ifaddrs * ifAddrStruct=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    QString resIp;

    while (ifAddrStruct!=NULL) {
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            //printf("%s IP Address4 %s\n", ifAddrStruct->ifa_name, addressBuffer);
            //printf("%s\n",addressBuffer);
            resIp=addressBuffer;

        } else if (ifAddrStruct->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            //printf("%s IP Address6 %s\n", ifAddrStruct->ifa_name, addressBuffer);
        }
        ifAddrStruct=ifAddrStruct->ifa_next;
    }
    return resIp;
}

void Database::PrintDatabase()
{
    QString str = "SELECT * FROM USERINFO ORDER BY uid;";
    QSqlQuery query;
    query.exec(str);

    qDebug()<<"===========打印 USERINFO 表所有信息:============";
    while(query.next()){
        QString m_id=query.value(0).toString();
        QString m_name=query.value("uname").toString();
        QString m_password=query.value("upassword").toString();
        int m_status=query.value("status").toInt();
        int m_identification=query.value("identification").toInt();
        qDebug()<<"id:"<<m_id<<" name"<<m_name<<" password"<<m_password<<" status"<<m_status<<" identification"<<m_identification;
    }
    qDebug()<<"=========================================";

    QString str2 = "SELECT * FROM COURSEINFO ORDER BY cid;";
    query.exec(str2);
    qDebug()<<"===========打印 COURSEINFO 表所有信息:============";
    while(query.next()){
        QString m_cid=query.value(0).toString();
        QString m_cname=query.value("cname").toString();
        QString m_tid=query.value("tid").toString();
        int m_status=query.value("status").toInt();
        QString m_address=query.value("address").toString();
        qDebug()<<"id:"<<m_cid<<" name"<<m_cname<<" tid"<<m_tid<<" status"<<m_status<<" address"<<m_address;
    }
    qDebug()<<"=========================================";

    QString str3 = "SELECT * FROM UC ORDER BY uid;";
    query.exec(str3);
    qDebug()<<"===========打印 UC 表所有信息:============";
    while(query.next()){
        QString m_uid=query.value(0).toString();
        QString m_cid=query.value("cid").toString();
        qDebug()<<"uid:"<<m_uid<<" cid"<<m_cid;
    }
    qDebug()<<"=========================================";
}

/*****************************************************************************
  *Date:  2022.06.13
  *Description:  数据库类，
                 使用单例模式，供User类和Server使用
                 主要功能对表进行增删改查
*****************************************************************************/
#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QHash>
class Database:public QObject
{
    Q_OBJECT
public:
    // 单例模式
    static Database &instance(){
        static Database instance;
        return instance;
    }

    // 打开数据库
    bool Open(const QString &databaseName);

    // 关闭数据库
    void Close();

    // 用户注册
    QString Register(const QString &name, const QString &password,const int &identification);

    // 用户登陆
    QJsonObject Login(const QString &id, const QString &password,const int identification);

    // 获取用户信息
    QJsonObject getUserInfo(const QString &user_id);

    // 获取课堂信息
    QJsonObject getClassInfo(const QString &cid);

    // 获取课程详细信息（目前只要课程id和教师姓名）
    QJsonObject getCourseDetailsInfo(const QString &course_id);

    // 获取课程成员信息
    QJsonObject getCourseMember(const QString &course_id);

    // 教师创建课程
    QJsonObject CreateCourse(const QString &tid,const QString &cname);

    // 学生加入课程
    QJsonObject JoinCourse(const QString &uid,const QString &cid);

    // 教师创建课堂
    QJsonObject CreateClass(const QString &tid,const QString &cid);

    // 学生加入课堂
    QJsonObject JoinClass(const QString &user_id,const QString &course_id);

    // 学生退选课程
    QJsonObject DeselectionCourse(const QString &user_id,const QString &course_id);

    // 教师删除课程
    QJsonObject DeleteCourse(const QString &course_id);

    // 教师关闭课堂
    void CloseClass(const QString &course_id);

    // 更新用户状态
    void UpdateUserStatus(const QString &id,const int &status);

    // 更新课程状态
    void UpdateCourseStatus(const QString &id,const int &status);

    // 更新课程IP
    void UpdateIPAddress(const QString &course_id,const QString &IP);

    // 获取本机ip地址
    QString GetIPAddress();

    // 打印数据库的所有信息
    void PrintDatabase();
private:
    Database()= default;

    int CourseNumber_ = 0;// 课堂数量

    int MaxUserId_ = 1; // 用户最大的id

    QSqlDatabase UserDb_;// sql数据库

    QString IPAddress_;

    QHash<QString,QVector<QString>> CoursePersonnel_; // 每个课程的人员
};

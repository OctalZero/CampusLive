/*********************************************************************************
  *Date:  2022.06.19
  *Description:  聊天室窗口类，
  *				 主要用于设置聊天室的UI。
**********************************************************************************/
#pragma once
#include <QWidget>

class QPaintEvent;
class QPainter;
class QLabel;
class QMovie;

class ChatMessage : public QWidget
{
	Q_OBJECT
public:
	explicit ChatMessage(QWidget* parent = nullptr);

	enum User_Type {
		User_System,//系统
		User_Me,    //自己
		User_She,   //用户
		User_Time,  //时间
	};

	// 只要是将外部文本设置
	void setText(QString name, QString text, QString time, QSize allSize, User_Type userType);

	// 获取文本的大小，如果有换行
	QSize getRealString(QString src);

	// 设置每个区域的具体位置
	QSize fontRect(QString str);

	inline QString name() { return m_name; }
	inline QString text() { return m_msg; }
	inline QString time() { return m_time; }
	inline User_Type userType() { return m_userType; }
protected:
	// 打印
	void paintEvent(QPaintEvent* event);
private:
	QString m_msg;
	QString m_time;
	QString m_curTime;
	QString m_name;

	QSize m_allSize;
	User_Type m_userType = User_System;

	int m_nameWidth;
	int m_kuangWidth;
	int m_textWidth;
	int m_spaceWid;
	int m_lineHeight;


	// 名字
	QRect m_nameLeftRect; // 显示名字左侧区域
	QRect m_iconLeftRect; // 显示左头像的区域
	QRect m_sanjiaoLeftRect; // 显示三角形左侧区域
	QRect m_kuangLeftRect; // 显示矩形左侧区域
	QRect m_textLeftRect; // 显示消息左侧区域
	QPixmap m_leftPixmap; // 提供他人头像的图片显示
};

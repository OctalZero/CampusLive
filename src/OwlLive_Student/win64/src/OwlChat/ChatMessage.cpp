#include "ChatMessage.h"
#include <QFontMetrics>
#include <QPaintEvent>
#include <QDateTime>
#include <QPainter>
#include <QMovie>
#include <QLabel>
#include <QDebug>
ChatMessage::ChatMessage(QWidget* parent) : QWidget(parent)
{
	QFont te_font = this->font();
	te_font.setFamily("MicrosoftYaHei");
	te_font.setPointSize(12);
	this->setFont(te_font);
	m_leftPixmap = QPixmap(":/classroom/assets/avatar.png");
}

void ChatMessage::setText(QString name, QString text, QString time, QSize allSize, ChatMessage::User_Type userType)
{
	m_msg = text;
	m_name = name;
	m_userType = userType;
	m_time = time;

	m_curTime = QDateTime::fromTime_t(time.toInt()).toString("hh:mm");
	m_allSize = allSize;

	this->update();
}

QSize ChatMessage::fontRect(QString str)
{
	m_msg = str;
	//m_name = str;
	// 名字
	int namex = 10;
	int namey = 5;
	int nameHei = 50;
	int nameW = 200;

	int minHei = 30;
	// 头像
	int iconWH = 40; // 头像宽高
	int iconSpaceW = 10; // 头像与边界的距离 x
	int iconRectW = 5; // 头像与消息框的距离
	int iconTMPH = 10; //  y

	int sanJiaoW = 6; //三角形的宽度
	int kuangTMP = 20; //  y

	int textSpaceRect = 12;// 文本与左右两边的距离

	// 矩形的宽度
	m_kuangWidth = this->width() - kuangTMP - 2 * (iconWH + iconSpaceW + iconRectW);

	// 文本宽度
	m_textWidth = m_kuangWidth - 2 * textSpaceRect;
	m_nameWidth = m_kuangWidth - 2 * textSpaceRect;

	//  剩余宽度
	m_spaceWid = this->width() - m_textWidth;
	// 图标
	m_iconLeftRect = QRect(iconSpaceW, iconTMPH + nameHei, iconWH, iconWH);

	QSize size_name = getRealString(m_name);
	nameW = size_name.width();
	QSize size = getRealString(m_msg); // 整个的size
	int hei = size.height() < minHei ? minHei : size.height();


	// 设置名字区域
	m_nameLeftRect.setRect(namex, namey, nameW, nameHei);

	// 设置三角形
	m_sanjiaoLeftRect = QRect(iconWH + iconSpaceW + iconRectW, m_lineHeight / 2 + 2 * nameHei, sanJiaoW, hei - m_lineHeight);

	// 设置矩形
	if (size.width() < (m_textWidth + m_spaceWid)) {
		m_kuangLeftRect.setRect(m_sanjiaoLeftRect.x() + m_sanjiaoLeftRect.width(), m_lineHeight / 4 * 3 + nameHei, size.width() - m_spaceWid + 2 * textSpaceRect + 5, hei - m_lineHeight);

	}
	else {
		m_kuangLeftRect.setRect(m_sanjiaoLeftRect.x() + m_sanjiaoLeftRect.width(), m_lineHeight / 4 * 3 + nameHei, m_kuangWidth + 5, hei - m_lineHeight);

	}

	// 设置消息区域
	m_textLeftRect.setRect(m_kuangLeftRect.x() + textSpaceRect, m_kuangLeftRect.y() + iconTMPH, m_kuangLeftRect.width() - 2 * textSpaceRect, m_kuangLeftRect.height() - 2 * iconTMPH);

	return QSize(size.width(), hei + nameHei);
}

QSize ChatMessage::getRealString(QString src)
{
	QFontMetricsF fm(this->font());
	m_lineHeight = fm.lineSpacing();
	int nCount = src.count("\n");
	int nMaxWidth = 0;
	if (nCount == 0) {
		nMaxWidth = fm.width(src);
		QString value = src;
		if (nMaxWidth > m_textWidth) {
			nMaxWidth = m_textWidth;
			int size = m_textWidth / fm.width(" ");
			int num = fm.width(value) / m_textWidth;
			int ttmp = num * fm.width(" ");
			num = (fm.width(value)) / m_textWidth;
			nCount += num;
			QString temp = "";
			for (int i = 0; i < num; i++) {
				temp += value.mid(i * size, (i + 1) * size) + "\n";
			}
			src.replace(value, temp);
		}
	}
	else {
		for (int i = 0; i < (nCount + 1); i++) {
			QString value = src.split("\n").at(i);
			nMaxWidth = fm.width(value) > nMaxWidth ? fm.width(value) : nMaxWidth;
			if (fm.width(value) > m_textWidth) {
				nMaxWidth = m_textWidth;
				int size = m_textWidth / fm.width(" ");
				int num = fm.width(value) / m_textWidth;
				num = ((i + num) * fm.width(" ") + fm.width(value)) / m_textWidth;
				nCount += num;
				QString temp = "";
				for (int i = 0; i < num; i++) {
					temp += value.mid(i * size, (i + 1) * size) + "\n";
				}
				src.replace(value, temp);
			}
		}
	}
	return QSize(nMaxWidth + m_spaceWid, (nCount + 1) * m_lineHeight + 2 * m_lineHeight);
}

void ChatMessage::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);//消锯齿
	painter.setPen(Qt::NoPen);
	painter.setBrush(QBrush(Qt::gray));

	if (m_userType == User_Type::User_She) { // 用户

		//头像
		painter.drawPixmap(m_iconLeftRect, m_leftPixmap);

		//框加边
		QColor col_KuangB(234, 234, 234);
		painter.setBrush(QBrush(col_KuangB));
		painter.drawRoundedRect(m_kuangLeftRect.x() - 1, m_kuangLeftRect.y() - 1, m_kuangLeftRect.width() + 2, m_kuangLeftRect.height() + 2, 4, 4);
		//框
		QColor col_Kuang(255, 255, 255);
		painter.setBrush(QBrush(col_Kuang));
		painter.drawRoundedRect(m_kuangLeftRect, 4, 4);

		//名字
		QPen penText_;
		penText_.setColor(Qt::black);
		painter.setPen(penText_);
		QTextOption option_(Qt::AlignLeft | Qt::AlignVCenter);
		option_.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
		painter.setFont(this->font());
		painter.drawText(m_nameLeftRect, m_name, option_);
		//内容
		QPen penText;
		penText.setColor(QColor(51, 51, 51));
		painter.setPen(penText);
		QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
		option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
		painter.setFont(this->font());
		painter.drawText(m_textLeftRect, m_msg, option);

	}
	else if (m_userType == User_Type::User_Time) { // 时间
		QPen penText;
		penText.setColor(QColor(153, 153, 153));
		painter.setPen(penText);
		QTextOption option(Qt::AlignCenter);
		option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
		QFont te_font = this->font();
		te_font.setFamily("MicrosoftYaHei");
		te_font.setPointSize(10);
		painter.setFont(te_font);
		painter.drawText(this->rect(), m_curTime, option);
	}
}

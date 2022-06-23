/*********************************************************************************
  *Date:  2022.06.19
  *Description:  系统主体窗口类，是其他将要显示给用户的界面的父类，
  *				 主要用于统一窗口的部分UI样式和一些界面操作。
**********************************************************************************/
#pragma once
#include <QWidget>

class OwlLiveWindow : public QWidget
{
	Q_OBJECT

public:
	OwlLiveWindow(QWidget* parent = Q_NULLPTR);
	~OwlLiveWindow();
};

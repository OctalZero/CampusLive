/*********************************************************************************
  *Date:  2022.04.23
  *Description:  滑动条类，继承于 QSlider，
  *				 主要重载了 QSlider 里的一些响应操作。
**********************************************************************************/
#pragma once
#include <QObject>
#include <QMouseEvent>
#include <QSlider>
class OwlSlider : public QSlider
{
	Q_OBJECT

public:
	void mousePressEvent(QMouseEvent* e) override;  // 鼠标按压操作的重写
	OwlSlider(QWidget* parent = nullptr);  // parent赋为nullptr，直接new时失败
	~OwlSlider() = default;
};
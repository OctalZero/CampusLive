#include "OwlSlider.h"

void OwlSlider::mousePressEvent(QMouseEvent* e)
{
	QSlider::mousePressEvent(e);
	double pos = static_cast<double>(e->pos().x()) / width();
	setValue(pos * this->maximum());
	// 重载后，保证原有事件也得到处理
	//QSlider::mousePressEvent(e);  // 存在问题：不能拖动到结尾位置，拖动到最后时松开事件没有响应
	QSlider::sliderReleased();
}

OwlSlider::OwlSlider(QWidget* parent)
	: QSlider(parent)
{
}
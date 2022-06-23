/*********************************************************************************
  *Date:  2022.04.23
  *Description:  播放器界面类，继承于 QWidget，
  *				 主要处理解播放器中UI事件的响应。
  *				 OwlLive中利用OwlPlay的部分功能来完成Classroom中的播放功能。
**********************************************************************************/
#pragma once
#include <QtWidgets/QWidget>
#include "ui_OwlPlay.h"
class OwlVideoWidget;
struct AVFrame;

class OwlPlay : public QWidget
{
	Q_OBJECT
public:
	OwlPlay(QWidget* parent = Q_NULLPTR);
	~OwlPlay();

    // 初始化 OpenGL
    virtual void Init(int width, int height);

    // 重绘 OpenGL 显示的图像
    virtual void Repaint(AVFrame* frame);

private:
	Ui_OwlPlayClass* ui_owl_play_ = nullptr;  // 教室界面UI
    OwlVideoWidget* video_widget_ = nullptr;  // 显示视频图像的类
};

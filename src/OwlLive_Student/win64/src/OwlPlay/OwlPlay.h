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
class AVFrame;

class OwlPlay : public QWidget
{
	Q_OBJECT

public:
	// 定时器，滑动条显示
	void timerEvent(QTimerEvent* e) override;

	// 窗口尺寸变化
	void resizeEvent(QResizeEvent* e) override;

	// 双击全屏，全屏的时候会卡顿
	void mouseDoubleClickEvent(QMouseEvent* e) override;

	// 切换暂停与播放
	void SetPause(bool is_pause);

	// 通过课堂直播流地址拉取直播流，并进行播放的槽
	void PullStream(const QString& stream_address);

	OwlPlay(QWidget* parent = Q_NULLPTR);
	~OwlPlay();
public slots:
	// 打开视频文件的槽
	void OpenVideo();

	// 通过UI中的按钮进行拉流的槽
	void Pull();

	// 切换播放和暂停的槽
	void PlayOrPause();

	// 按住滑动条的槽
	void SliderPress();

	// 松开滑动条，视频进度滑动到相应位置的槽
	void SliderRelease();
private:
	Ui_OwlPlayClass* ui_owl_play_ = nullptr;  // 教室界面UI
	bool is_slider_press = false; // 滑动条默认没被按住
};

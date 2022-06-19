#include "OwlPlay.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include "OwlDemuxThread.h"
#include "OwlVideoWidget.h"
extern "C" {
#include <libavutil/frame.h>
}

using namespace std;
static OwlDemuxThread demux_thread;
// 输入拉流地址
static QString g_pull_url = "http://39.134.65.162/PLTV/88888888/224/3221225611/index.m3u8";
//static QString g_pull_url = "rtmp://192.168.43.93/videotest/test";



void OwlPlay::timerEvent(QTimerEvent* e)
{
	if (is_slider_press)  return;
	long long total = demux_thread.total_ms_;  // 先保存下来，防止判断>0之后在其他进程被改
	if (total > 0) {
		double pos = static_cast<double>(demux_thread.pts_) / static_cast<double>(total);
		int play_pos = ui_owl_play_->play_pos->maximum() * pos;
		ui_owl_play_->play_pos->setValue(play_pos);
	}
}

void OwlPlay::resizeEvent(QResizeEvent* e)
{
	ui_owl_play_->play_pos->move(50, this->height() - 100);
	ui_owl_play_->play_pos->resize(this->width() - 100, ui_owl_play_->play_pos->height());
	ui_owl_play_->open->move(100, this->height() - 150);
	ui_owl_play_->is_play->move(ui_owl_play_->open->x() + ui_owl_play_->open->width() + 10, ui_owl_play_->open->y());
	ui_owl_play_->video->resize(this->size());
}

void OwlPlay::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (isFullScreen()) {
		this->showNormal();
	}
	else {
		this->showFullScreen();
	}
}

void OwlPlay::SetPause(bool is_pause)
{
	if (is_pause) {
		ui_owl_play_->is_play->setText("Play");
	}
	else {
		ui_owl_play_->is_play->setText("Pause");
	}
}

OwlPlay::OwlPlay(QWidget* parent)
	: QWidget(parent), ui_owl_play_(new Ui::OwlPlayClass)
{
	this->setAttribute(Qt::WA_DeleteOnClose);  // 确保关闭窗口时清理资源
	ui_owl_play_->setupUi(this);

	demux_thread.Start();  // 开启封装线程
	startTimer(40);  // 开启定时器，用于进度条更新

	// 连接将UI中的信号与执行逻辑的槽连接
	connect(ui_owl_play_->open, SIGNAL(clicked()), this, SLOT(OpenVideo()));
	connect(ui_owl_play_->is_play, SIGNAL(clicked()), this, SLOT(PlayOrPause()));
	connect(ui_owl_play_->play_pos, SIGNAL(sliderPressed()), this, SLOT(SliderPress()));
	connect(ui_owl_play_->play_pos, SIGNAL(sliderReleased()), this, SLOT(SliderRelease()));
	connect(ui_owl_play_->pull, SIGNAL(clicked()), this, SLOT(Pull()));

	// 隐藏组件
	ui_owl_play_->open->setVisible(false);
	ui_owl_play_->is_play->setVisible(false);
	ui_owl_play_->play_pos->setVisible(false);
	ui_owl_play_->pull->setVisible(false);
}

OwlPlay::~OwlPlay()
{
	if (ui_owl_play_) {
		delete ui_owl_play_;
		ui_owl_play_ = nullptr;
	}

	demux_thread.Close();
}

void OwlPlay::OpenVideo()
{
	// 选择文件
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件或填入拉流URL"));
	if (name.isEmpty())  return;
	this->setWindowTitle(name);
	if (!demux_thread.Open(name.toLocal8Bit(), ui_owl_play_->video)) {
		QMessageBox::warning(0, "ERROR", "打开视频失败!");
		return;
	}

	SetPause(demux_thread.is_pause_);
	return;
}

void OwlPlay::Pull()
{
	PullStream(g_pull_url);
}

void OwlPlay::PullStream(const QString& stream_address)
{
	// 隐藏组件
	//ui_owl_play_->open->setVisible(false);
	//ui_owl_play_->is_play->setVisible(false);
	//ui_owl_play_->play_pos->setVisible(false);
	//ui_owl_play_->pull->setVisible(false);

	qDebug() << stream_address.toLocal8Bit();
	if (!demux_thread.Open(stream_address.toLocal8Bit(), ui_owl_play_->video)) {
		QMessageBox::warning(0, "ERROR", "该课程暂时还未开课!");
		return;
	}

	SetPause(demux_thread.is_pause_);
	return;
}

void OwlPlay::PlayOrPause()
{
	bool is_pause = !demux_thread.is_pause_;
	SetPause(is_pause);
	demux_thread.SetPause(is_pause);
}

void OwlPlay::SliderPress()
{
	is_slider_press = true;
}

void OwlPlay::SliderRelease()
{
	is_slider_press = false;
	double pos = 0.0;
	pos = static_cast<double>(ui_owl_play_->play_pos->value()) / ui_owl_play_->play_pos->maximum();
	demux_thread.Seek(pos);
}

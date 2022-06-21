#include "OwlPlay.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include "OwlVideoWidget.h"
extern "C" {
#include <libavutil/frame.h>
}

using namespace std;
// 输入拉流地址
static QString g_pull_url = "http://39.134.65.162/PLTV/88888888/224/3221225611/index.m3u8";
//static QString g_pull_url = "rtmp://192.168.43.93/videotest/test";

OwlPlay::OwlPlay(QWidget* parent)
    : QWidget(parent), ui_owl_play_(new Ui::OwlPlayClass)
{
	this->setAttribute(Qt::WA_DeleteOnClose);  // 确保关闭窗口时清理资源
    ui_owl_play_->setupUi(this);
    video_widget_ = ui_owl_play_->video;
}

OwlPlay::~OwlPlay()
{
	if (ui_owl_play_) {
		delete ui_owl_play_;
		ui_owl_play_ = nullptr;
    }
}

void OwlPlay::Init(int width, int height)
{
    ui_owl_play_->video->Init(width, height);
}

void OwlPlay::Repaint(AVFrame *frame)
{
   ui_owl_play_->video->Repaint(frame);
}


#include "ClassroomWindow.h"
#include "OwlPlay.h"
#include "OwlChat.h"
#include "IPushStream.h"
#include <thread>
#include <QJsonObject>
#include <iostream>
#include <unistd.h>
#include "SDLPlay.h"
extern "C" {
#include <libavutil/frame.h>
}

ClassroomWindow::ClassroomWindow(OwlLiveWindow* parent)
    : OwlLiveWindow(parent), ui_classroom_(new Ui::Classroom)
{
    ui_classroom_->OwlChatWidget = owl_chat_;
    ui_classroom_->OwlPlayWidget = sdl_play_;
    this->setAttribute(Qt::WA_DeleteOnClose);  // 确保关闭窗口时清理资源
    ui_classroom_->setupUi(this);

    // 初始化播放器和聊天框，并绑定到教室窗口中
    push_stream_ = new PushStream;
    owl_chat_ = new OwlChat(ui_classroom_->OwlChatWidget);
    sdl_play_ = new SDLPlay(ui_classroom_->OwlPlayWidget);
    this->show();

    // 连接教室界面与聊天框的信号与信号
    connect(owl_chat_, &OwlChat::SendClientRequestInfo, this, &ClassroomWindow::SendChatMessage);
    // 连接教室界面与聊天框的信号与槽
    connect(this, &ClassroomWindow::ForwardChatMessage, owl_chat_, &OwlChat::ShowChatMessage);
}

ClassroomWindow::~ClassroomWindow()
{
    if (ui_classroom_) {
        delete ui_classroom_;
        ui_classroom_ = nullptr;
    }
    exit_ = true;
    push_stream_->ClosePushStream();
    qDebug()<<"Close pushe stream";
    if(push_stream_){
        delete push_stream_;
        push_stream_ = nullptr;
    }
    Q_EMIT ExitClassroom();
}

void ClassroomWindow::UpdateChatInfo(const QJsonObject& data)
{
    if (owl_chat_) {
        owl_chat_->ShowChatMessage(data);
    }
}

void ClassroomWindow::InitClassMembers(const QJsonArray& data)
{
    if (owl_chat_) {
        owl_chat_->ShowMembers(data);
    }
}

void ClassroomWindow::ConfigLive()
{
    ui_live_config_ = new Ui::LiveConfig();
    live_config_ = new QWidget();
    ui_live_config_->setupUi(live_config_);
    live_config_->show();
    qDebug()<<" show !!!!";
    connect(ui_live_config_->camera, &QCheckBox::clicked, this, [&]()->void{
        if (ui_live_config_->camera->isCheckable()) {
            ui_live_config_->screen->setChecked(false);
        }
    });
    connect(ui_live_config_->screen, &QCheckBox::clicked, this, [&]()->void{
        if (ui_live_config_->screen->isCheckable()) {
            ui_live_config_->camera->setChecked(false);
        }
    });

    connect(ui_live_config_->confirm, &QPushButton::clicked, this, &ClassroomWindow::JudgeLiveConfig);
}

void ClassroomWindow::InputVideoFrame()
{

    AVFrame *frame;
    if(push_stream_){
//        usleep(1000000);
        while(1){
            frame = push_stream_->ReadVideoFrame();
            sdl_play_->PresentFrame(frame->data[0],frame->width,frame->height);
            if(exit_)break;
            av_frame_free(&frame);
        }
        qDebug()<<"close read frame";
    }
}

void ClassroomWindow::OpenPushStream(const QJsonObject& data)
{
    qDebug()<<"  init stream   "<<data;
    QString stream_address = data["stream_address"].toString();
    std::string s = stream_address.toStdString();
    int select = data["source"].toInt();
    auto lambda_fun1 = [s,select,this]()->void{
        std::string stream_address = s;
        int sel = select;
        std::cout<<stream_address<<"  "<<sel<<std::endl;
        InitPushStream(stream_address, sel);
    };
    std::thread push_stream(lambda_fun1);

    push_stream.detach();


}

void ClassroomWindow::InitPushStream(std::string& stream_address, int& select)
{
    qDebug()<<"init push stream";
    std::cout<<stream_address<<" df "<<select<<std::endl;
    if(push_stream_)
        push_stream_->OpenPushStream(stream_address,select);
    auto lambda_fun = [&]()->void{
        InputVideoFrame();
    };
    std::thread input_video(lambda_fun);
    input_video.detach();
    //    qDebug()<<"open push stream fail";
}

void ClassroomWindow::JudgeLiveConfig()
{
    int device = 0;  // 读取设备：屏幕为0,摄像头为1
    if (ui_live_config_->screen->isChecked()) {
        device = 0;
    }
    else {
        device = 1;
    }
    qDebug()<<" source  "<<device;
    CloseLiveConfigWindow();
    Q_EMIT ConfirmSource(device);
}

void ClassroomWindow::CloseLiveConfigWindow()
{
    if (live_config_) {
        live_config_->close();
        live_config_ = nullptr;
    }
    if (ui_live_config_) {
        ui_live_config_ = nullptr;
    }

}


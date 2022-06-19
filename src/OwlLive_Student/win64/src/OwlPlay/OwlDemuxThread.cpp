#include "OwlDemuxThread.h"
#include <iostream>
#include "OwlDemux.h"
#include "OwlVideoThread.h"
#include "OwlAudioThread.h"
#include "OwlDecode.h"
using namespace std;

bool OwlDemuxThread::Open(const char* url, IVideoCall* video_call)
{
	// 容错
	if (url == nullptr || url[0] == '\0')  return false;
	mutex_.lock();
	if (!demux_)  demux_ = new OwlDemux();
	if (!video_thread_)  video_thread_ = new OwlVideoThread();
	if (!audio_thread_)  audio_thread_ = new OwlAudioThread();

	// 打开解封装
	bool re = demux_->Open(url);
	if (!re) {
		mutex_.unlock();
		cout << "demux_->Open(url) failed!" << endl;
		return false;
	}

	// 打开视频解码器和处理线程
	if (!video_thread_->Open(demux_->CopyVideoPara(),
		video_call, demux_->width_, demux_->height_)) {
		re = false;
		cout << "video_thread_->Open（）failed！" << endl;
	}

	// 打开音频解码器和处理线程
	if (!audio_thread_->Open(demux_->CopyAudioPara())) {
		re = false;
		cout << "audio_thread_->Open（）failed！" << endl;
	}
	total_ms_ = demux_->total_ms_;
	mutex_.unlock();
	cout << "OwlDemuxThread::Open：" << re << endl;
	return re;
}

void OwlDemuxThread::Start()
{
	mutex_.lock();
	if (!demux_)  demux_ = new OwlDemux();
	if (!video_thread_)  video_thread_ = new OwlVideoThread();
	if (!audio_thread_)  audio_thread_ = new OwlAudioThread();
	// 启动当前线程 
	QThread::start();
	if (video_thread_)  video_thread_->start();
	if (audio_thread_)	audio_thread_->start();
	mutex_.unlock();
}

void OwlDemuxThread::Close()
{
	is_exit_ = true;
	wait();
	if (video_thread_)  video_thread_->Close();
	if (audio_thread_)  audio_thread_->Close();
	mutex_.lock();
	delete video_thread_;
	delete audio_thread_;
	video_thread_ = nullptr;
	audio_thread_ = nullptr;
	mutex_.unlock();
}

void OwlDemuxThread::Clear()
{
	mutex_.lock();
	if (demux_)  demux_->Clear();
	if (video_thread_)  video_thread_->Clear();
	if (audio_thread_)  audio_thread_->Clear();
	mutex_.unlock();
}

void OwlDemuxThread::SetPause(bool is_pause)
{
	mutex_.lock();
	is_pause_ = is_pause;
	if (audio_thread_)  audio_thread_->SetPause(is_pause);
	if (video_thread_)  video_thread_->SetPause(is_pause);
	mutex_.unlock();
}

void OwlDemuxThread::Seek(double pos)
{

	mutex_.lock();
	bool status = is_pause_;
	mutex_.unlock();

	// 暂停
	SetPause(true);

	// 清理缓冲
	Clear();

	mutex_.lock();
	if (demux_) {
		demux_->Seek(pos);

		// 实际要显示的位置pts
		long long seek_pts = pos * demux_->total_ms_;
		// 暂停状态下也能刷新到seek到的位置的图像
		while (!is_exit_) {
			AVPacket* pkt = demux_->ReadVideo();
			if (!pkt)  break;
			// 如果解码到seek_pts
			if (video_thread_->ReaintPts(pkt, seek_pts)) {
				pts_ = seek_pts;
				break;
			}
		}
	}
	mutex_.unlock();

	// Seek时处于非暂停状态
	if (!status) {
		SetPause(false);
	}
}

void OwlDemuxThread::run()
{
	while (!is_exit_) {
		mutex_.lock();
		// 处理暂停解封装阻塞
		if (is_pause_) {
			mutex_.unlock();
			msleep(5);
			continue;
		}
		if (!demux_) {
			mutex_.unlock();
			msleep(5);
			continue;
		}

		// 音视频同步，没有考虑只有音频或只有视频的情况
		if (audio_thread_ && video_thread_) {
			pts_ = audio_thread_->pts_;
			video_thread_->syn_pts_ = audio_thread_->pts_;
		}

		//bool flush_flag_ = false;  // 判断是否进行过刷新，用于改变 DemuxThread 状态
		AVPacket* pkt = demux_->Read();
		if (!pkt) {
			// DemuxThread 状态由读取流中变为读取结束，并刷新解码缓冲区
			if (read_state_ == 1) {
				cout << "!!!!!!!!!Flush!!!!!!!!!" << endl;
				read_state_ = 2;
				if (audio_thread_ && video_thread_) {
					audio_thread_->is_flush_ = true;
					video_thread_->is_flush_ = true;
					audio_thread_->read_state_ = 2;
					video_thread_->read_state_ = 2;

					while (read_state_ == 2) {  // 阻塞等待音视频的 Decode 缓冲刷新结束
						if (audio_thread_->read_state_ == 3
							&& video_thread_->read_state_ == 3) {
							read_state_ = 3;
						}
						msleep(5);
					}
				}
			}
			else {  // 阻塞
				cout << "解封装阻塞" << endl;
				mutex_.unlock();
				msleep(5);
				continue;
			}

		}

		// 将读取流的状态重置为未读取流
		if (read_state_ == 3) {
			read_state_ = 0;
			if (audio_thread_ && video_thread_) {
				audio_thread_->read_state_ = 0;
				video_thread_->read_state_ = 0;
			}
		}

		// 判断数据是音频
		if (demux_->isAudio(pkt)) {
			if (audio_thread_) {
				read_state_ = 1;  // 改变当前流读取状态为流读取中
				cout << "Push Audio" << endl;
				audio_thread_->Push(pkt);
			}
		}
		else if (demux_->isVideo(pkt)) {  // 视频
			if (video_thread_) {
				read_state_ = 1;
				cout << "Push Vedio" << endl;
				video_thread_->Push(pkt);  // 隐患，Push中可能造成阻塞
			}
		}
		mutex_.unlock();
		msleep(1);  // 防止读取太快，音视频同步来不及做
	}
}

OwlDemuxThread::~OwlDemuxThread()
{
	// 防止线程宕掉
	is_exit_ = true;
	wait();
}
#include "OwlVideoThread.h"
#include <iostream>
#include "OwlDecode.h"
using namespace std;
bool OwlVideoThread::Open(AVCodecParameters* para, IVideoCall* video_call, int width, int height)
{
	if (!para)  return false;
	Clear();

	video_mutex_.lock();
	syn_pts_ = 0;
	// 初始化显示窗口
	video_call_ = video_call;  // 传递回调函数
	if (video_call_) {
		video_call_->Init(width, height);
	}
	video_mutex_.unlock();

	int re = true;
	// decode_的Open用完后会释放para，放在最后面释放
	if (!decode_->Open(para)) {
		cout << "video OwlDecode open failed!" << endl;
		re = false;
	}

	cout << "OwlVideoThread::Open：" << re << endl;

	return re;
}


void OwlVideoThread::SetPause(bool is_pause)
{
	video_mutex_.lock();
	is_pause_ = is_pause;
	video_mutex_.unlock();
}

void OwlVideoThread::run()
{
	while (!is_exit_) {
		video_mutex_.lock();

		// 处理暂停
		if (is_pause_) {
			video_mutex_.unlock();
			msleep(5);
			continue;
		}

		// 音视频同步, 不存在音频时不处理
		if (syn_pts_ > 0 && decode_->pts_ - syn_pts_ > 90
			&& abs(decode_->pts_ - syn_pts_) < 1000) {  // 确保音频滞后90ms以内，重新推流后不进行阻塞
			cout << "音视频同步阻塞" << endl;
			video_mutex_.unlock();
			msleep(1);
			continue;
		}


		if (is_flush_) {
			FlushDecodeBuffer();
		}

		AVPacket* packet = Pop();

		bool re = decode_->SendPacket(packet);
		if (!re) {
			cout << "视频解码阻塞" << endl;
			video_mutex_.unlock();
			msleep(1);
			continue;
		}
		// 一次Send，多次Receive
		while (!is_exit_) {
			AVFrame* frame = decode_->ReceiveFrame();
			// 停止刷新 Decode 缓冲区，更新读取流的状态
			if (decode_->eof_) {
				is_flush_ = false;
				read_state_ = 3;
			}
			if (!frame)  break;
			cout << "video pts = " << decode_->pts_ << endl;

			// 显示视频
			if (video_call_) {
				video_call_->Repaint(frame);
			}

		}
		video_mutex_.unlock();
	}
}

bool OwlVideoThread::ReaintPts(AVPacket* pkt, long long seek_pts)
{
	video_mutex_.lock();

	bool re = decode_->SendPacket(pkt);
	if (!re) {
		video_mutex_.unlock();
		return true;  // 表示结束解码
	}
	AVFrame* frame = decode_->ReceiveFrame();
	if (!frame) {
		video_mutex_.unlock();
		return false;  // 没读到，继续处理下一帧
	}
	// 到达位置
	if (decode_->pts_ >= seek_pts) {
		if (video_call_) {
			video_call_->Repaint(frame);
		}
		video_mutex_.unlock();
		return true;
	}
	OwlFreeFrame(&frame);

	video_mutex_.unlock();
	return false;
}
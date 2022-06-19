#include "OwlDecodeThread.h"
#include "OwlDecode.h"

#include <iostream>
using namespace std;

void OwlDecodeThread::Push(AVPacket* pkt)
{
	if (read_state_ != 2 && !pkt)  return;

	// 阻塞，数据不能丢
	while (!is_exit_) {
		mutex_.lock();
		if (packets_.size() < max_list_) {
			packets_.push_back(pkt);
			mutex_.unlock();
			break;
		}
		cout << "Push阻塞" << endl;
		mutex_.unlock();
		msleep(1);
	}
}

void OwlDecodeThread::Clear()
{
	mutex_.lock();
	decode_->Clear();
	// 清理缓冲队列
	while (!packets_.empty()) {
		AVPacket* packet = packets_.front();
		OwlFreePacket(&packet);
		packets_.pop_front();
	}
	mutex_.unlock();
}

void OwlDecodeThread::Close()
{
	Clear();
	// 等待线程退出
	is_exit_ = true;
	wait();
	// 线程退出后再清理decode_，在线程中可能访问到decode_
	decode_->Close();
	mutex_.lock();
	delete decode_;
	decode_ = nullptr;
	mutex_.unlock();
}

AVPacket* OwlDecodeThread::Pop()
{
	mutex_.lock();
	if (packets_.empty()) {
		mutex_.unlock();
		return nullptr;
	}
	AVPacket* packet = packets_.front();
	packets_.pop_front();
	mutex_.unlock();
	cout << "Pop" << endl;

	return packet;
}

void OwlDecodeThread::FlushDecodeBuffer()
{
	decode_->read_state_ = read_state_;
	AVPacket* empty_pkt = nullptr;
	decode_->AllocEmptyPacket(empty_pkt);
	if (is_flush_) {
		Push(empty_pkt);
	}
	else {
		decode_->FreeEmptyPacket(empty_pkt);
	}
}

OwlDecodeThread::OwlDecodeThread()
{
	// 打开解码器
	if (!decode_)  decode_ = new OwlDecode();
}

OwlDecodeThread::~OwlDecodeThread()
{
	// 等待线程退出，不然线程还在运行，但其他空间已经释放了
	is_exit_ = true;
	wait();
}
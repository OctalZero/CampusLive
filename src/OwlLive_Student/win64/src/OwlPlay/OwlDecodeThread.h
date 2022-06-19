/*********************************************************************************
  *Date:  2022.04.23
  *Description:  解码的线程类，继承于 QThread 类，
  *				 主要处理解码所需要的逻辑，相当于解码控制器。
**********************************************************************************/
#pragma once
#include <list>
#include <mutex>
#include <QThread>
struct AVPacket;
class OwlDecode;

class OwlDecodeThread : public QThread
{
public:
	// 放入packet到缓冲队列
	virtual void Push(AVPacket* pkt);

	// 清理队列
	virtual void Clear();

	// 清理资源，停止线程
	virtual void Close();

	// 取出一帧数据，并出栈，如果没有返回 nullptr 
	virtual AVPacket* Pop();

	// 流读取结束后，刷新 Decode 缓冲区
	virtual void FlushDecodeBuffer();

	OwlDecodeThread();
	virtual ~OwlDecodeThread();
public:
	int max_list_ = 100;  // 最大队列，缓冲约2s
	bool is_exit_ = false;  // 判断线程是否退出
	bool is_pause_ = false;  // 是否暂停
	// 读取流的状态：0->还未读取流; 1->读取流中; 2->读取流结束; 3->刷新完 Decode 缓冲区
	int read_state_ = 0;
	bool is_flush_ = false;  // 是否刷新 Decode 缓冲
protected:
	std::mutex mutex_;  // 解码线程锁
	// 生产者消费者模式，由调用者生产packet扔到该生产队列中，再在线程中消费packet
	std::list<AVPacket*> packets_;
	OwlDecode* decode_ = nullptr;  // 解码器
};


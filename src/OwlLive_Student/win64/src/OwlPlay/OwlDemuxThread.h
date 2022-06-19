
/*********************************************************************************
  *Date:  2022.04.23
  *Description:  解封装的线程类，继承于 QThread，
  *				 主要处理解封装所需要的逻辑，相当于解封装控制器，
  *				 包含解封装后的各种参数信息，在此类中将信息传给 OwlVideoThread 和
  *				 OwlAudioThread。
**********************************************************************************/
#pragma once
#include <QThread>
#include <mutex>
#include "IVideoCall.h"
class OwlDemux;
class OwlVideoThread;
class OwlAudioThread;

class OwlDemuxThread : public QThread
{
public:
	// 创建对象并打开
	virtual bool Open(const char* url, IVideoCall* video_call);

	// 启动所有线程
	virtual void Start();

	// 关闭线程，清理资源
	virtual void Close();

	// 清理缓冲
	virtual void Clear();

	// 设置暂停
	void SetPause(bool is_pause);

	// 控制视频进度
	virtual void Seek(double pos);

	// 处理线程
	void run() override;

	OwlDemuxThread() = default;
	~OwlDemuxThread();
public:
	bool is_exit_ = false;  // 是否退出
	long long pts_ = 0;  // 当前播放到的pts
	long long total_ms_ = 0;  // 视频总时长，进度条使用
	bool is_pause_ = false;  // 是否暂停
	// 读取流的状态：0->还未读取流; 1->读取流中; 2->读取流结束; 3->刷新完 Decode 缓冲区
	int read_state_ = 0;
protected:
	std::mutex mutex_;  // 解封装线程锁
	OwlDemux* demux_ = nullptr;  // 解封装
	OwlVideoThread* video_thread_ = nullptr;  // 视频线程
	OwlAudioThread* audio_thread_ = nullptr;  // 音频线程
};


/*********************************************************************************
  *Date:  2022.04.23
  *Description:  解码和播放音频的线程类，继承于 OwlDecodeThread 类
  *				 主要处理音频播放逻辑，相当于音频控制器。
**********************************************************************************/
#pragma once
#include <QThread>
#include <mutex>
#include "OwlDecodeThread.h"
class OwlAudioPlay;
class OwlResample;
struct AVCodecParameters;

class OwlAudioThread : public OwlDecodeThread
{
public:
	// 打开，不管成功与否都清理
	virtual bool Open(AVCodecParameters* para);

	// 停止线程，清理资源
	virtual void Close() override;

	// 清理缓冲，音频的IO设备也有缓冲要清理，需要重写清理函数
	virtual void Clear() override;

	// 设置暂停
	void SetPause(bool is_pause);

	// 从缓冲队列取出packet
	void run() override;

	OwlAudioThread();
	~OwlAudioThread() = default;
public:
	long long pts_ = 0;  // 当前音频播放的pts
protected:
	std::mutex audio_mutex_;  // 音频线程锁
	OwlAudioPlay* audio_play_ = nullptr;  // 音频播放
	OwlResample* resample_ = nullptr;  // 重采样  
};


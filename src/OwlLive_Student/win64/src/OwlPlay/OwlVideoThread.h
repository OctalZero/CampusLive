/*********************************************************************************
  *Date:  2022.04.23
  *Description:  解码和显示视频的线程类，继承于 OwlDecodeThread，
  *				 主要处理视频播放逻辑，相当于视频控制器。
**********************************************************************************/
#pragma once
#include "IVideoCall.h"
#include "OwlDecodeThread.h"
struct AVCodecParameters;

class OwlVideoThread : public OwlDecodeThread
{
public:
	// 打开，不管成功与否都清理
	virtual bool Open(AVCodecParameters* para, IVideoCall* video_call, int width, int height);

	// 设置暂停
	void SetPause(bool is_pause);

	// 从缓冲队列取出packet
	void run() override;

	// 解码pts，如果接收到的解码数据pts >= seek_pts，就return true，并且显示画面
	virtual bool ReaintPts(AVPacket* pkt, long long seek_pts);

	OwlVideoThread() = default;
	virtual ~OwlVideoThread() = default;
public:
	long long syn_pts_ = 0;  // 同步时间，由外部传入
protected:
	std::mutex video_mutex_;  // 视频线程锁
	IVideoCall* video_call_ = nullptr;  // 显示视频图像的接口
};
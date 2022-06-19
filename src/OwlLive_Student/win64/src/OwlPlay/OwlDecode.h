/*********************************************************************************
  *Date:  2022.04.23
  *Description:  解码的类，
				 将所有需要和 FFmpeg 解码部分相关的内容放入了此类,
				 避免其他类和 FFmpeg 耦合。
**********************************************************************************/
#pragma once
#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
extern void OwlFreePacket(AVPacket** pkt);  // 暴露释放函数给OwlDecodeThread，避免引入FFmpeg库
extern void OwlFreeFrame(AVFrame** frame);  // 暴露释放函数给OwlDecodeThread，避免引入FFmpeg库

class OwlDecode
{
public:
	// 打开解码器,不管成功与否都释放para空间
	virtual bool Open(AVCodecParameters* para);

	// 发送到解码线程，不管成功与否都释放pkt空间（对象和媒体内容）
	virtual bool SendPacket(AVPacket* pkt);

	// 获取解码数据，一次send可能需要多次Receive，最后获取缓冲中的数据Send NULL再Receive多次
	// 每次复制一份，由调用者释放 av_frame_free
	virtual AVFrame* ReceiveFrame();

	// 分配一个空的 AVPacket
	void AllocEmptyPacket(AVPacket* pkt);

	// 释放一个空的 AVPacket
	void FreeEmptyPacket(AVPacket* pkt);

	// 关闭
	virtual void Close();

	// 清理缓存
	virtual void Clear();

	OwlDecode() = default;
	~OwlDecode() = default;
public:
	long long pts_ = 0;  // 当前解码到的pts
	bool eof_ = false;  // 流读取结束的标记
	// 读取流的状态：0->还未读取流; 1->读取流中; 2->读取流结束; 3->刷新完 Decode 缓冲区
	int read_state_ = 0;
protected:
	AVCodecContext* codec_context_ = nullptr; // 解码上下文
	std::mutex mutex_; // 解码锁
};


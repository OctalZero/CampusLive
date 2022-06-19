/*********************************************************************************
  *Date:  2022.04.23
  *Description:  解封装的类，
  *				 将所有需要和 FFmpeg 解封装部分相关的内容放入了此类,
				 避免其他类和 FFmpeg 耦合。。
**********************************************************************************/
#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class OwlDemux
{
public:
	// 打开媒体文件，或者流媒体 rtmp\http\rtsp
	virtual bool Open(const char* url);

	// 解封装，获取AVPacket，空间需要调用者释放，释放ACPacket对象和数据空间 av_packet_free
	virtual AVPacket* Read();

	// 只读视频，音频丢弃，空间释放
	virtual AVPacket* ReadVideo();

	// 判断是否为音频
	virtual bool isAudio(AVPacket* pkt);

	// 判断是否为视频
	virtual bool isVideo(AVPacket* pkt);

	// 获取视频参数，返回空间需要清理 avcodec_parameters_free
	virtual AVCodecParameters* CopyVideoPara();

	// 获取音频参数，返回空间需要清理 avcodec_parameters_free
	virtual AVCodecParameters* CopyAudioPara();

	// seek 位置 pos 0.0 ~ 1.0
	virtual bool Seek(double pos);

	// 清空读取缓存
	virtual void Clear();

	// 关闭
	virtual void Close();

	OwlDemux();
	~OwlDemux() = default;
public:
	int total_ms_ = 0;  // 媒体总时长（毫秒）
	int width_ = 0;  // 材质宽
	int height_ = 0;  // 材质高
	int sample_rate_ = 0;  // 音频采样率
	int channels_ = 0;  // 音频通道数
protected:
	std::mutex mutex_;  // 解封装锁 
	AVFormatContext* ic = NULL;  // 解封装上下文
	int video_stream_ = 0;  // 视频索引，读取时区分音视频
	int audio_stream_ = 1;  // 音频索引，读取时区分音视频
};


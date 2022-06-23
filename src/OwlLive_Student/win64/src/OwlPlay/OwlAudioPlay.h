/*********************************************************************************
  *Date:  2022.04.23
  *Description:  播放音频的接口类，
  *				 使用工厂模式，可具体换成不同的音频接口，这里使用的是Qt的Audio接口。
**********************************************************************************/
#pragma once
class OwlAudioPlay
{
public:
	// 打开音频播放，定义为纯虚函数，在子类中实现
	virtual bool Open() = 0;

	// 关闭音频
	virtual void Close() = 0;

	// 清理音频缓存
	virtual void Clear() = 0;

	// 返回缓冲中还没有播放的时间（毫秒ms） 
	virtual long long GetNoPlayMs() = 0;

	// 播放音频
	virtual bool Write(const unsigned char* data, int datasize) = 0;

	// 判断音频缓冲区是否有足够空间来写
	virtual int GetFree() = 0;

	// 设置暂停
	virtual void SetPause(bool is_pause) = 0;

	// 单例模式
	static OwlAudioPlay& GetAudioPlay();
	OwlAudioPlay(const OwlAudioPlay&) = delete;
	OwlAudioPlay& operator=(const OwlAudioPlay&) = delete;
	OwlAudioPlay() = default;
	virtual ~OwlAudioPlay() = default;
public:
	int sample_rate_ = 44100;  // 采样率，重采样后修改
	int sample_size_ = 16;  // 位数，重采样后修改
	int channels_ = 2;  // 通道数，重采样后修改
};
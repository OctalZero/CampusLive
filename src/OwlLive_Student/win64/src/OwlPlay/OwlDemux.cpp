#include "OwlDemux.h"
#include <iostream>
using namespace std;
extern "C" {
#include "libavformat/avformat.h"
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")

// 计算分数的值
static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

bool OwlDemux::Open(const char* url)
{
	Close();
	// 参数设置
	AVDictionary* opts = nullptr;
	// 设置rtsp流以tcp协议打开
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);

	// 网络延时时间
	av_dict_set(&opts, "max_delay", "5000", 0);


	mutex_.lock();
	int re = avformat_open_input(
		&ic,
		url,
		NULL,  // 自动选择解封装器
		&opts  // 参数设置，比如rtsp的延时时间
	);
	if (re != 0)
	{
		mutex_.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed! :" << buf << endl;

		return false;
	}

	// 获取流信息
	re = avformat_find_stream_info(ic, NULL);
	if (re != 0)
	{
		mutex_.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed! :" << buf << endl;

		return false;
	}
	cout << "open " << url << " success! " << endl;

	//总时长 毫秒
	total_ms_ = ic->duration / (AV_TIME_BASE / 1000);
	cout << "total_ms_ = " << total_ms_ << endl;

	//打印视频流详细信息
	av_dump_format(ic, 0, url, 0);

	// 获取视频流，还有一种通过遍历所有流的方法获取
	video_stream_ = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	AVStream* as = ic->streams[video_stream_];
	// 存储宽高
	width_ = as->codecpar->width;
	height_ = as->codecpar->height;

	cout << "=================================================" << endl;
	cout << video_stream_ << "视频信息" << endl;
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "width=" << as->codecpar->width << endl;
	cout << "height=" << as->codecpar->height << endl;
	// 帧率 fps 分数转换
	cout << "video fps = " << r2d(as->avg_frame_rate) << endl;

	// 获取音频流
	audio_stream_ = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	as = ic->streams[audio_stream_];
	sample_rate_ = as->codecpar->sample_rate;
	channels_ = as->codecpar->channels;
	cout << "=================================================" << endl;
	cout << audio_stream_ << "音频信息" << endl;
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "sample_rate = " << as->codecpar->sample_rate << endl;
	cout << "channels = " << as->codecpar->channels << endl;
	//  一帧数据 单通道样本数 
	cout << "frame_size = " << as->codecpar->frame_size << endl;
	mutex_.unlock();

	return true;
}

AVPacket* OwlDemux::Read()
{
	mutex_.lock();
	if (!ic) {  // 容错
		mutex_.unlock();
		return nullptr;
	}
	AVPacket* pkt = av_packet_alloc();  // 分配对象空间
	// 读取一帧，并分配数据空间
	int re = av_read_frame(ic, pkt);
	if (re != 0) {  // 返回0为成功
		mutex_.unlock();
		av_packet_free(&pkt);  // 释放对象
		return nullptr;
	}
	// pts、dts转换为毫秒, 方便做同步
	pkt->pts = pkt->pts * (r2d(ic->streams[pkt->stream_index]->time_base) * 1000);
	pkt->dts = pkt->dts * (r2d(ic->streams[pkt->stream_index]->time_base) * 1000);
	mutex_.unlock();
	//cout << pkt->pts << " " << flush;

	return pkt;
}

AVPacket* OwlDemux::ReadVideo()
{
	mutex_.lock();
	if (!ic) {  // 容错
		mutex_.unlock();
		return nullptr;
	}
	mutex_.unlock();

	AVPacket* pkt = nullptr;
	// 限定20帧，防止阻塞
	for (int i = 0; i < 20; ++i) {
		pkt = Read();
		if (!pkt)  break;
		if (pkt->stream_index == video_stream_) {
			break;
		}
		av_packet_free(&pkt);
	}

	return pkt;
}

bool OwlDemux::isAudio(AVPacket* pkt)
{
	if (!pkt) return false;
	if (pkt->stream_index == audio_stream_) {
		return true;
	}
	return false;
}

bool OwlDemux::isVideo(AVPacket* pkt)
{
	if (!pkt) return false;
	if (pkt->stream_index == video_stream_) {
		return true;
	}
	return false;
}

AVCodecParameters* OwlDemux::CopyVideoPara()
{
	mutex_.lock();
	if (!ic) {
		mutex_.unlock();
		return nullptr;
	}
	AVCodecParameters* para = avcodec_parameters_alloc();
	avcodec_parameters_copy(para, ic->streams[video_stream_]->codecpar);

	mutex_.unlock();

	return para;
}

AVCodecParameters* OwlDemux::CopyAudioPara()
{
	mutex_.lock();
	if (!ic) {
		mutex_.unlock();
		return nullptr;
	}
	AVCodecParameters* para = avcodec_parameters_alloc();
	avcodec_parameters_copy(para, ic->streams[audio_stream_]->codecpar);

	mutex_.unlock();

	return para;
}

bool OwlDemux::Seek(double pos)
{
	mutex_.lock();
	if (!ic) {
		mutex_.unlock();
		return false;
	}
	// 清理读取缓冲，防止粘包现象
	avformat_flush(ic);

	long long seek_pos = 0;
	// 其中一种跳转方法，做容错的话还可以考虑另外两种方法，防止duration读不到
	// 跳转音频
	seek_pos = ic->streams[audio_stream_]->duration * pos;
	int re = av_seek_frame(ic, audio_stream_, seek_pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	mutex_.unlock();
	if (re < 0) {  //返回>=0为成功
		return false;
	}

	return true;
}

void OwlDemux::Clear()
{
	mutex_.lock();
	if (!ic) {
		mutex_.unlock();
		return;
	}
	// 清理读取缓冲，防止粘包现象
	avformat_flush(ic);
	mutex_.unlock();
}

void OwlDemux::Close()
{
	mutex_.lock();
	if (!ic) {
		mutex_.unlock();
		return;
	}
	avformat_close_input(&ic);
	// 重置媒体总时长（毫秒）
	total_ms_ = 0;
	mutex_.unlock();
}

OwlDemux::OwlDemux()
{
	static bool is_first = true;
	static std::mutex demux_mutex;
	demux_mutex.lock();
	if (is_first) {
		// 初始化封装库
		//av_register_all();

		// 初始化网络库 （可以打开rtsp rtmp http 协议的流媒体视频）
		avformat_network_init();
		is_first = false;
	}
	demux_mutex.unlock();
}

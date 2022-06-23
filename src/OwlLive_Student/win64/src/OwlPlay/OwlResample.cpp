#include "OwlResample.h"
#include "OwlResample.h"
#include <iostream>
using namespace std;
extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "swresample.lib")

bool OwlResample::Open(AVCodecParameters* para, bool is_clear_para)
{
	if (!para)  return false;
	mutex_.lock();

	//音频重采样 申请 SwrContext 结构体
	if (!resample_context_) {
		resample_context_ = swr_alloc();
	}
	// 如果resample为NULL会自动分配空间
	resample_context_ = swr_alloc_set_opts(resample_context_,
		av_get_default_channel_layout(out_channel_),	//输出格式
		(AVSampleFormat)out_format_,		//输出样本格式 1 AV_SAMPLE_FMT_S16
		out_sample_rate_,					//输出采样率
		av_get_default_channel_layout(para->channels),//输入格式
		(AVSampleFormat)para->format,
		para->sample_rate,
		0, 0
	);
	// 用完释放参数指针,默认不释放，防止多次释放
	if (is_clear_para) {
		avcodec_parameters_free(&para);
	}

	// 初始化 SwrContext
	int re = swr_init(resample_context_);
	mutex_.unlock();

	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "swr_init  failed! :" << buf << endl;
		return false;
	}
	//unsigned char* pcm = nullptr;

	return true;
}


void OwlResample::Close()
{
	mutex_.lock();
	if (resample_context_) {
		swr_free(&resample_context_);
	}
	mutex_.unlock();
}

int OwlResample::Resample(AVFrame* in_data, unsigned char* out_data)
{
	if (!in_data)  return 0;
	if (!out_data) {
		av_frame_free(&in_data);
		return 0;
	}

	uint8_t* data[2] = { 0 };
	data[0] = out_data;
	// 开始重采样转换，获取每个通道输出的样本数
	int re = swr_convert(resample_context_,
		data, in_data->nb_samples,		//输出
		(const uint8_t**)in_data->data, in_data->nb_samples	//输入
	);
	if (re <= 0)  return re;
	out_sample_size_ = av_get_bytes_per_sample((AVSampleFormat)out_format_);
	int out_size = re * out_channel_ * out_sample_size_;
	av_frame_free(&in_data);

	return out_size;
}


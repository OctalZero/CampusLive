#include "OwlDecode.h"
#include <iostream>
using namespace std;
extern "C" {
#include <libavcodec/avcodec.h>
}

void OwlFreePacket(AVPacket** pkt)
{
	if (!pkt || !(*pkt))  return;  // 容错
	av_packet_free(pkt);
}

void OwlFreeFrame(AVFrame** frame)
{
	if (!frame || !(*frame))  return;  // 容错
	av_frame_free(frame);
}

bool OwlDecode::Open(AVCodecParameters* para)
{
	if (!para)  return false;
	Close();

	// 解码器打开，找到解码器
	AVCodec* codec = avcodec_find_decoder(para->codec_id);
	if (!codec)
	{
		avcodec_parameters_free(&para);
		cout << "can't find the codec id " << para->codec_id << endl;
		return false;
	}
	cout << "find the AVCodec " << para->codec_id << endl;

	mutex_.lock();
	// 创建解码器上下文
	codec_context_ = avcodec_alloc_context3(codec);

	// 配置解码器上下文参数
	avcodec_parameters_to_context(codec_context_, para);
	avcodec_parameters_free(&para);  // 用完后及时释放参数指针

	// 八线程解码
	codec_context_->thread_count = 8;

	// 打开解码器上下文
	int re = avcodec_open2(codec_context_, 0, 0);
	if (re != 0)
	{
		avcodec_free_context(&codec_context_);
		mutex_.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "avcodec_open2  failed! :" << buf << endl;
		return false;
	}
	mutex_.unlock();
	cout << "avcodec_open2 success!" << endl;

	return true;
}

bool OwlDecode::SendPacket(AVPacket* pkt)
{
	// 容错处理
	if (read_state_ != 2 && (!pkt || pkt->size <= 0 || !pkt->data))  return false;
	mutex_.lock();
	if (!codec_context_) {
		mutex_.unlock();
		return false;
	}
	int re = avcodec_send_packet(codec_context_, pkt);
	mutex_.unlock();
	av_packet_free(&pkt);  // 只有用 av_packet_alloc 分配的才能这样释放
	if (re != 0) return false;

	return true;
}

AVFrame* OwlDecode::ReceiveFrame()
{
	mutex_.lock();
	if (!codec_context_) {
		mutex_.unlock();
		return nullptr;
	}
	AVFrame* frame = av_frame_alloc();
	int re = avcodec_receive_frame(codec_context_, frame);
	mutex_.unlock();

	if (re == AVERROR_EOF) {  // 判断解码器缓冲区是否刷新完
		cout << "!!!!!!!!!!!!!EOF!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		eof_ = true;
		read_state_ = 3;
	}
	if (re != 0) {
		av_frame_free(&frame);
		return nullptr;
	}

	//cout << "[" << frame->linesize[0] << "]" << " " << flush;
	pts_ = frame->pts;
	return frame;
}

void OwlDecode::AllocEmptyPacket(AVPacket* pkt)
{
	pkt = av_packet_alloc();
	pkt->data = nullptr;
	pkt->size = 0;
}

void OwlDecode::FreeEmptyPacket(AVPacket* pkt)
{
	av_packet_free(&pkt);
}

void OwlDecode::Close()
{
	mutex_.lock();
	if (codec_context_) {
		//avcodec_close(codec_context_);  // 该函数只清空的数据，未将指针置空
		avcodec_free_context(&codec_context_);
	}
	pts_ = 0;
	mutex_.unlock();
}

void OwlDecode::Clear()
{
	mutex_.lock();
	// 清理解码缓冲
	if (codec_context_) {
		avcodec_flush_buffers(codec_context_);
	}
	mutex_.unlock();
}

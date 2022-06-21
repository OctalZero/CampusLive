#ifndef PUSHSTREAM_H
#define PUSHSTREAM_H

#include <string>

extern "C"{
    #include <libavutil/frame.h>
    #include <libavcodec/packet.h>
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}

struct FrameData  // 存放待编码的数据帧
{
    AVFrame *frame;
    struct FrameData *next;
};

struct PacketData  // 存放发送包的链表
{
    AVPacket *packet;
    struct PacketData *next;
};

typedef struct OutputStream
{
    AVStream *stream = nullptr;  // 输出流
    AVCodecContext *encodec_context = nullptr;  // 编码上下文

    int64_t next_pts = 0;  // 记录帧的pts
    int samples_count = 0;  // 音频读取的数量

    AVFrame *convert_frame = nullptr;  //存放转换后编码前的帧
    AVFrame *temporary_frame = nullptr;  // 存放待转换的临时帧

    int t = 0;
    int tincr = 0;
    int tincr2 = 0;


    struct SwsContext *sws_context = nullptr;  // 图片格式转换
    struct SwrContext *swr_context = nullptr;  // 音频格式转换
} OutputStream;

class PushStream
{
public:
    PushStream();
    ~PushStream();
    void Open();
    void Close();

    struct FrameData *CreateFrameHead(struct FrameData *head);  // 创建存放待编码帧的链表的头
    void AddFrameNode(struct FrameData *head, AVFrame *frame);  // 添加一帧到链表中
    void DeleteFrameNode(struct FrameData *head, AVFrame *frame);  // 从链表中删除一帧
    int CountFrameNode(struct FrameData *head);  // 统计链表中帧的数量
    void ClearFrameNode(struct FrameData *head);  // 清空链表中的数据

    struct PacketData *CreatePacketHead(struct PacketData *head);  // 创建存放编码后的包链表头
    void AddPacketNode(struct PacketData *head, AVPacket *packet);  // 向链表中添加一个数据包
    void DeletePacketNode(struct PacketData *head, AVPacket *packet);  // 从链表中删除一个数据包
    int CountPacketNode(struct PacketData *head);  // 统计链表中包的数量
    void ClearPacketNode(struct PacketData *head);  // 清空链表中的数据包

    void InitRecordEquipment();  // 注册设备
    int InterruptCallback();  // 超时回调函数

    int OpenVideoDevice(std::string inputurl, std::string format);  // 打开视频输入设备
    int OpenAudioDevice(std::string inputurl, std::string format);  // 打开音频输入设备

    void InitDecodeContext(AVStream *input_stream);  // 初始解码上下文

    AVPacket *ReadInputPacket(AVFormatContext *incontext);  // 从输入设备中读取一个原始数据包

    int DecodeVideo(AVPacket *packet, AVCodecContext *decode_context, int width, int height);  // 解码视频帧
    int DecodeAudio(AVPacket *packet, AVCodecContext *decode_context);  // 解码音频帧

    void RecycleReadVideoData();  // 不断从视频输入设备中读取视频数据
    void RecycleReadAudioData();  // 不断从音频输入设备中读取音频数据

    void AddStream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id);  // 添加一个输出流
    AVFrame *AllocPictureMemory(enum AVPixelFormat pix_format, int width, int height);  // 分配存储一张图片的内存
    AVFrame *AllocAudioFrame(enum AVSampleFormat sample_format, uint64_t channel_layout, int sample_rate, int nb_sample);  // 分配存储一帧音频数据的内存
    void OpenVideoOutput(AVCodec *codec, OutputStream *ost, AVDictionary *dictory);  // 打开视频输出流
    void OpenAudioOutput(AVCodec *codec, OutputStream *ost, AVDictionary *dictory);  // 打开音频输出流
    void OpenOutput(std::string file_name);  // 打开输出环境
    AVFrame *GetAudioFrame(AVFrame *frame);  // 从链表中获取一帧音频数据

    AVFrame *ReadVideoFrame();
    AVFrame *GetVideoFrame(OutputStream *ost);  // 从链表中获取一帧视频数据
    AVPacket *GetSendPacket(PacketData *head);  // 从链表中获取要发送的音视频数据包
    int EncodeAudio();  // 编码音频
    int EncodeVideo();  // 编码视频
    int WritePacket(AVFormatContext *format_context, AVStream *st, AVPacket *pkt);  // 向输出流中写入一帧音视频数据
    void CloseStream(OutputStream *ost);  // 清理输出环境
    int SendPacket();  // 选择发送的数据

    void OpenPushStream(std::string address, int select);  // 开始推流
    void ClosePushStream();  // 结束推流


private:
    const int kvideo_width_ = 1280;
    const int kvideo_height_ = 720;
    std::string rtmp_server_addr_ = "rtmp://localhost/videotest/test";
    int stream_frame_rate_ = 30;
    int close_push_stream_ = 0;
    const int ktimeout_ = 3000000;
    pthread_mutex_t play_mutex_;
    pthread_mutex_t video_mutex_;
    pthread_mutex_t audio_mutex_;
    pthread_mutex_t video_mutex_packet_;
    pthread_mutex_t audio_mutex_packet_;
//    pthread_cond_t frame_cond_;
//    pthread_cond_t packet_cond_;
    AVFormatContext *video_input_context_ = nullptr;
    AVFormatContext *audio_input_context_ = nullptr;
    AVFormatContext *output_format_context_ = nullptr;
    AVCodecContext *video_encodec_context_ = nullptr;
    AVCodecContext *audio_encodec_context_ = nullptr;
    int64_t last_read_packet_time_;
    int64_t start_time_;
    volatile int64_t time_interval_;
    OutputStream video_stream_ = {0};
    OutputStream audio_stream_ = {0};
    int have_video_ = 0;
    int have_audio_ = 0;
    struct FrameData *play_frame_head_ = nullptr;
    struct FrameData *audio_frame_head_ = nullptr;
    struct FrameData *video_frame_head_ = nullptr;
    struct PacketData *audio_packet_head_ = nullptr;
    struct PacketData *video_packet_head_ = nullptr;

    uint8_t *out_buffer = nullptr;
};

#endif // PUSHSTREAM_H

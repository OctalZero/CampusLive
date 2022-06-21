#include "IPushStream.h"
#include <thread>
#include <iostream>
#include <QDebug>


extern "C"{
#include <libavutil/avassert.h>
#include "libavutil/time.h"
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavdevice/avdevice.h>
#include "libavutil/imgutils.h"
}


using namespace std;

PushStream::PushStream()
{

}

PushStream::~PushStream()
{

}

void PushStream::Open()
{

}

void PushStream::Close()
{

}

FrameData *PushStream::CreateFrameHead(FrameData *head)
{
    if(head==NULL)
    {
//        printf("new header frame\n");
        head = (struct FrameData *)malloc(sizeof(struct FrameData));
        head->next=NULL;
    }
    return head;
}

void PushStream::AddFrameNode(FrameData *head, AVFrame *frame)
{
    struct FrameData *tmp=head;
    struct FrameData *new_node;
    /*找到链表尾部*/
    while(tmp->next)
    {
        tmp=tmp->next;
    }
    /*插入新的节点*/
    //    new_node = new AUDIO_DATA;
    new_node=(struct FrameData *)malloc(sizeof(struct FrameData));
    new_node->frame = av_frame_alloc();
    //    printf("src pts %ld,",frame->pts);
    av_frame_move_ref(new_node->frame,frame);
    //    printf(" dst pts %ld\n",new_node->frame->pts);
    new_node->next=NULL;
    /*将新节点接入到链表*/
    tmp->next=new_node;
}

void PushStream::DeleteFrameNode(FrameData *head, AVFrame *frame)
{
    struct FrameData *tmp=head;
    struct FrameData *p;
    /*找到链表中要删除的节点*/
    while(tmp->next)
    {
        p=tmp;
        tmp=tmp->next;
        if(tmp->frame->pts==frame->pts)
        {
//            printf("delete\n");
            p->next=tmp->next;
//            av_frame_unref(frame);
            if(frame){
                av_frame_free(&frame);
            }
            if(tmp){
                free(tmp);
                tmp = nullptr;
            }
            break;
        }
    }
}

int PushStream::CountFrameNode(FrameData *head)
{
    int cnt=0;
    struct FrameData *tmp=head;
    while(tmp->next)
    {
        tmp=tmp->next;
        cnt++;
    }
    return cnt;
}

void PushStream::ClearFrameNode(FrameData *head)
{
    struct FrameData *tmp = head;
    while (tmp->next) {
        if(tmp->next->frame){
            DeleteFrameNode(head,tmp->next->frame);
        }
        tmp = head;
    }
    free(head);
    head = nullptr;
}

PacketData *PushStream::CreatePacketHead(PacketData *head)
{
    if(head==NULL)
    {
//        printf("new header packet\n");
        head = (struct PacketData *)malloc(sizeof(struct PacketData));
        head->next=NULL;
    }
    return head;
}

void PushStream::AddPacketNode(PacketData *head, AVPacket *packet)
{
    struct PacketData *tmp=head;
    struct PacketData *new_node;
    /*找到链表尾部*/
    while(tmp->next)
    {
        tmp=tmp->next;
    }
    /*插入新的节点*/
    //    new_node = new AUDIO_DATA;
    new_node=(struct PacketData *)malloc(sizeof(struct PacketData));
    new_node->packet = av_packet_alloc();
//    printf("add packt pts %ld ",packet->pts);
    av_packet_move_ref(new_node->packet,packet);
    av_packet_unref(packet);
//    printf("add new packt pts %ld\n",new_node->packet->pts);
//    new_node->packet->pts = packet->pts;
    new_node->next=NULL;
    /*将新节点接入到链表*/
    tmp->next=new_node;
}

void PushStream::DeletePacketNode(PacketData *head, AVPacket *packet)
{
    struct PacketData *tmp=head;
    struct PacketData *p;
    /*找到链表中要删除的节点*/
    while(tmp->next)
    {
        p=tmp;
        tmp=tmp->next;
        if(tmp->packet->pts==packet->pts)
        {
            p->next=tmp->next;
//            av_packet_unref(packet);
            if(packet){
//                printf("free\n");
                av_packet_free(&packet);
            }
            if(tmp){
                free(tmp);
                tmp = nullptr;
            }
            break;
        }
    }
}

int PushStream::CountPacketNode(PacketData *head)
{
    int cnt=0;
    struct PacketData *tmp=head;
    while(tmp->next)
    {
        tmp=tmp->next;
        cnt++;
    }
    return cnt;
}

void PushStream::ClearPacketNode(PacketData *head)
{
    struct PacketData *tmp = head;
    while (tmp->next) {
        if(tmp->next->packet){
            DeletePacketNode(head,tmp->next->packet);
        }
        tmp = head;
    }
    free(head);
    head = nullptr;
}

void PushStream::InitRecordEquipment()
{
    avformat_network_init();
    avdevice_register_all();
    av_log_set_level(AV_LOG_WARNING);
}

int PushStream::InterruptCallback()
{
    if(av_gettime() - last_read_packet_time_ > ktimeout_)
    {
        return -1;
    }
    return 0;
}

int PushStream::OpenVideoDevice(std::string inputurl, std::string format)
{
    video_input_context_ = avformat_alloc_context();

    //阻塞超时的回调函数
    last_read_packet_time_ = av_gettime();
//    video_input_context_->interrupt_callback.callback = InterruptCallback;


    AVInputFormat *ifmt = av_find_input_format(format.data());// video4linux2 x11grab
    AVDictionary *format_opts = nullptr;
    av_dict_set_int(&format_opts,"rtbufsize",0, 0);
//    av_dict_set(&format_opts,"start_time_realtime",0,0);
    int ret = avformat_open_input(&video_input_context_, inputurl.c_str(),ifmt, &format_opts);
//    printf("video open time sec %ld, usec %ld\n", start.tv_sec,start.tv_usec);
    //    int ret = avformat_open_input(&video_input_context_, nullptr,inputvideo, &format_opts);
    if(ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Input video file open input failed\n");
        return ret;
    }
    ret = avformat_find_stream_info(video_input_context_, nullptr);
    if(ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Find input file stream inform failed\n");
    }else {
        av_log(NULL, AV_LOG_FATAL, "Open input file %s success\n", inputurl.c_str());
    }
    int videoindex = -1;
    for(int i=0; i<(int)video_input_context_->nb_streams; i++){//find video stream index
        if(video_input_context_->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
            videoindex=i;
            break;
        }
        if(i == (int)video_input_context_->nb_streams-1){
            printf("Couldn't find a video stream.\n");
            return -1;
        }
    }
    AVPacket * packet = av_packet_alloc();
    ret = av_read_frame(video_input_context_, packet);
    if(ret < 0)printf("read frame fail\n");
    printf("video input duration %ld\n",1000000/packet->duration);
    stream_frame_rate_ = 1000000/packet->duration;
    av_packet_free(&packet);
    av_dict_free(&format_opts);
    start_time_ = av_gettime();
    return videoindex;
}

int PushStream::OpenAudioDevice(std::string inputurl, std::string format)
{
    audio_input_context_ = avformat_alloc_context();


    //阻塞超时的回调函数
    last_read_packet_time_ = av_gettime();
//    audio_input_context_->interrupt_callback.callback = &PushStream::InterruptCallback;


    AVInputFormat *ifmt = av_find_input_format(format.data());// video4linux2 x11grab
    AVDictionary *format_opts = nullptr;
    av_dict_set_int(&format_opts,"rtbufsize",0, 0);
//    av_dict_set(&format_opts,"start_time_realtime",0,0);
    int ret = avformat_open_input(&audio_input_context_, inputurl.c_str(),ifmt, &format_opts);

//    printf("video open time sec %ld, usec %ld\n", start.tv_sec,start.tv_usec);
    //    int ret = avformat_open_input(&audio_input_context_, nullptr,inputvideo, &format_opts);
    if(ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Input audio file open input failed\n");
        return ret;
    }
    ret = avformat_find_stream_info(audio_input_context_, nullptr);
    if(ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Find input file stream inform failed\n");
    }else {
        av_log(NULL, AV_LOG_FATAL, "Open input file %s success\n", inputurl.c_str());
    }
    int audioindex = -1;
    for(int i=0; i<(int)audio_input_context_->nb_streams; i++){//find video stream index
        if(audio_input_context_->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            audioindex=i;
            break;
        }
        if(i == (int)audio_input_context_->nb_streams-1){
            printf("Couldn't find a audio stream.\n");
            return -1;
        }
    }

    av_dict_free(&format_opts);
    return audioindex;
}

void PushStream::InitDecodeContext(AVStream *input_stream)
{
    AVCodec* decodec = NULL;
    AVCodecContext* decodec_ctx = NULL;

    AVCodecParameters *codecParameters=input_stream->codecpar;
    decodec_ctx=avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(decodec_ctx,codecParameters);

    decodec=avcodec_find_decoder(decodec_ctx->codec_id);
    if(decodec==NULL){
        printf("Codec not found.\n");
        return;
    }
    if(avcodec_open2(decodec_ctx, decodec,NULL)<0){
        printf("Could not open codec.\n");
        return;
    }

    printf("iput width %d   height %d\n",input_stream->codecpar->width,input_stream->codecpar->height);

    if(codecParameters->codec_type == AVMEDIA_TYPE_VIDEO) video_encodec_context_ = decodec_ctx;
    if(codecParameters->codec_type == AVMEDIA_TYPE_AUDIO)audio_encodec_context_ = decodec_ctx;
}

AVPacket *PushStream::ReadInputPacket(AVFormatContext *incontext)
{
    AVPacket *packet = av_packet_alloc();
    //    printf("read packet timestamp %ld, %ld\n",timestamp.tv_sec,timestamp.tv_usec);
    int ret = av_read_frame(incontext, packet);
    if(ret == 0){
//        int64_t mid = packet->pts;
//        printf("in packet pts %ld,",packet->pts);
        packet->pts = av_gettime();
//        printf(" time pts %ld\n",packet->pts - mid);
        //        cout<<"packet\n";
        //        std::cout<<"packet   "<<packet<<std::endl;
        return packet;
    }else {
        return nullptr;
    }
}

int PushStream::DecodeVideo(AVPacket *packet, AVCodecContext *decode_context, int width, int height)
{
    AVPixelFormat outFormat = AV_PIX_FMT_YUV420P;
    int ret = 0;
    AVFrame *pdecodeFrame = av_frame_alloc();
    AVFrame *ptranscodedFrame = av_frame_alloc();
//    AVFrame *pFrameYUV = av_frame_alloc();
    SwsContext *psws = nullptr;

    static int count = 0;
    ptranscodedFrame->format = outFormat;
    ptranscodedFrame->width = kvideo_width_;
    ptranscodedFrame->height = kvideo_height_;
    do
    {
        //        ret = av_image_fill_arrays(ptranscodedFrame->data, ptranscodedFrame->linesize, pbuffer, outFormat, owidth, oheight, 1);
        ret = av_frame_get_buffer(ptranscodedFrame, 32);  //为音频或视频数据分配新的缓冲区。

        if (ret < 0) {
            fprintf(stderr, "Could not allocate frame data.\n");
            exit(1);
        }
        if((ret) < 0){ break; }

        while (AVERROR(EAGAIN) == (ret = avcodec_send_packet(decode_context, packet))) {}
        if((ret) < 0){ break; }

        while (0 <= (ret = avcodec_receive_frame(decode_context, pdecodeFrame))) {








            psws = sws_getCachedContext(psws, decode_context->width, decode_context->height, decode_context->pix_fmt, width, height, outFormat, SWS_BICUBIC, 0, 0, 0);
            if(!psws){
                break;
            }
            if (av_frame_make_writable(pdecodeFrame) < 0)printf("pdecodeFrame is don't write\n");
            if (av_frame_make_writable(ptranscodedFrame) < 0)printf("ptranscodedFrame is don't write\n");
            //            printf("sws scale ret %d\n",ret);
            ret = sws_scale(psws, pdecodeFrame->data, pdecodeFrame->linesize, 0, pdecodeFrame->height, ptranscodedFrame->data, ptranscodedFrame->linesize);
            //            printf("sws scale ret %d\n",ret);

            if((ret) < 0){ break; }
            //            fprintf(stdout, "%d, transcode frame %d : %d, %d into %d, %d\n", count, pdecodeFrame->pts, pdecoderCtx->width, pdecoderCtx->height, owidth, oheight);
            pthread_mutex_lock(&video_mutex_);   /*互斥锁上锁*/
            pthread_mutex_lock(&play_mutex_);
            //            yuyv_to_yuv420p(image_buffer[video_buffer.index],YUV420P_Buffer,kvideo_width_,kvideo_height_);
            //            gettimeofday( &videotime, NULL );
            //            memcpy(YUV420P_Buffer,pbuffer,size);
            //            printf("success add a video frame to list\n");
            ptranscodedFrame->pts = packet->pts;

//            printf("video packet pts %ld, frame pts %ld\n",packet->pts,ptranscodedFrame->pts);
            AVFrame *frame = av_frame_clone(pdecodeFrame);
            av_frame_copy(frame,pdecodeFrame);
            AddFrameNode(play_frame_head_,frame);
            AddFrameNode(video_frame_head_,ptranscodedFrame);
            pthread_mutex_unlock(&video_mutex_); /*互斥锁解锁*/
            pthread_mutex_unlock(&play_mutex_);
            //            pthread_cond_broadcast(&cond);/*广播方式唤醒休眠的线程*/
            //            fwrite(pbuffer, 1, size, fp);
            count++;
//            printf("count i %d\n",i++);
        }
    } while (false);

//    if(out_buffer){
//        delete out_buffer;
//        out_buffer = nullptr;
//    }

//    if(img_convert_ctx){
//        sws_freeContext(img_convert_ctx);
//    }

    if (psws) {
        sws_freeContext(psws);
    }

    if (pdecodeFrame) {
        av_frame_free(&pdecodeFrame);
    }

//    if(pFrameYUV){
//        av_frame_free(&pFrameYUV);
//    }

    if (ptranscodedFrame) {
//        printf("free\n");
        av_frame_free(&ptranscodedFrame);
    }


    return ret;
}

int PushStream::DecodeAudio(AVPacket *packet, AVCodecContext *decode_context)
{
    int ret = 0;
    AVFrame *pdecodeFrame = av_frame_alloc();
    do
    {


        while (AVERROR(EAGAIN) == (ret = avcodec_send_packet(decode_context, packet))) {}
        if((ret) < 0){ break; }


        while (0 <= (ret = avcodec_receive_frame(decode_context, pdecodeFrame))) {
            //            printf("add a audio frame to list\n");
            pthread_mutex_lock(&audio_mutex_);   /*互斥锁上锁*/
            //            yuyv_to_yuv420p(image_buffer[video_buffer.index],YUV420P_Buffer,kvideo_width_,kvideo_height_);
            //            gettimeofday( &videotime, NULL );
            //            memcpy(YUV420P_Buffer,pbuffer,size);
//            printf("audio frame pts %d, frame pts %d\n",pdecodeFrame->channels,pdecodeFrame->format);
            AddFrameNode(audio_frame_head_,pdecodeFrame);
            pthread_mutex_unlock(&audio_mutex_); /*互斥锁解锁*/
            //            pthread_cond_broadcast(&cond);/*广播方式唤醒休眠的线程*/
            //            fwrite(pbuffer, 1, size, fp);
        }
    } while (false);
    if (pdecodeFrame) {
        av_frame_free(&pdecodeFrame);
    }

    return ret;
}

void PushStream::RecycleReadVideoData()
{
    /*1. 循环读取摄像头采集的数据*/
    /*2. 申请存放JPG的数据空间*/
    //    kvideo_width_ = video_input_context_->streams[0]->codecpar->width;
    //    kvideo_height_ = video_input_context_->streams[0]->codecpar->height;
    AVCodecContext* decodec_ = NULL;
    decodec_ = video_encodec_context_;
    if(!decodec_)exit(0);
    //    video_input_context_->streams[index]->codec = decodec_;
    AVPacket *pkt = nullptr;
    while(close_push_stream_ != 1)
    {
        //        gettimeofday( &start, NULL );
        pkt = nullptr;
        pkt = ReadInputPacket(video_input_context_);
        //        printf("time of video   %ld  %ld\n",end.tv_sec,end.tv_usec);
        //        printf("read vedio pts %ld, duration %ld\n",pkt->pts,pkt->duration);
        //        std::cout<<pkt<<std::endl;
        if(pkt){
            //            printf("read success\n");
            DecodeVideo(pkt,decodec_,kvideo_width_,kvideo_height_);
        }
        av_packet_free(&pkt);
        /*(1)等待摄像头采集数据,如果没有数据就进行等待，直到有数据才继续执行*/


    }
    avcodec_free_context(&decodec_);
    avformat_close_input(&video_input_context_);
    avformat_free_context(video_input_context_);
    close_push_stream_ = 2;
    printf("cout sdslfsd            %d\n",close_push_stream_);
}

void PushStream::RecycleReadAudioData()
{
    printf("init read audio\n");
    /*1. 循环读取摄像头采集的数据*/

    /*2. 申请存放JPG的数据空间*/
    //    kvideo_width_ = video_input_context_->streams[0]->codecpar->width;
    //    kvideo_height_ = video_input_context_->streams[0]->codecpar->height;
    AVCodecContext* decodec_ = NULL;
    decodec_ = audio_encodec_context_;
    if(!decodec_)exit(0);
    //    video_input_context_->streams[index]->codec = decodec_;
    AVPacket *pkt = nullptr;
    while(close_push_stream_ != 2)
    {
        //        gettimeofday( &start, NULL );
//        printf("close stream %d\n",close_push_stream_);
        pkt = nullptr;
        pkt = ReadInputPacket(audio_input_context_);
//        printf("audio duration %ld\n",pkt->duration);
        //        printf("time of video   %ld  %ld\n",end.tv_sec,end.tv_usec);
        //        printf("read audio pts %ld, duration %ld\n",pkt->pts,pkt->duration);
        //        std::cout<<pkt<<std::endl;
        if(pkt){
            //            printf("read success\n");
            DecodeAudio(pkt,decodec_);
        }
        av_packet_free(&pkt);
        /*(1)等待摄像头采集数据,如果没有数据就进行等待，直到有数据才继续执行*/


    }
    avcodec_free_context(&decodec_);
    avformat_close_input(&audio_input_context_);
    avformat_free_context(audio_input_context_);
    close_push_stream_ = 3;
    printf("cout sdslfsd            %d\n",close_push_stream_);
}

void PushStream::AddStream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, AVCodecID codec_id)
{
    AVCodecContext *c;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }


    ost->stream = avformat_new_stream(oc, NULL);
    if (!ost->stream) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }
    ost->stream->id = oc->nb_streams-1;
    c = avcodec_alloc_context3(*codec);
    if (!c) {
        fprintf(stderr, "Could not alloc an encoding context\n");
        exit(1);
    }
    ost->encodec_context = c;

    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        //        av_opt_set(c->priv_data, "tune", "zerolatency", 0);

        c->sample_fmt  = (*codec)->sample_fmts ? (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
        c->bit_rate    = 64000;  //设置码率
        c->sample_rate = audio_encodec_context_->sample_rate;  //音频采样率
        c->channels= audio_encodec_context_->channels;
//        cout<<"audio channels  "<<c->channels<<endl;
        c->channel_layout = av_get_default_channel_layout(c->channels);// AV_CH_LAYOUT_MONO 单声道   AV_CH_LAYOUT_STEREO 立体声

        ost->stream->time_base = (AVRational){ 1, c->sample_rate };
        //        c->time_base = ost->stream->time_base;
        break;

    case AVMEDIA_TYPE_VIDEO:
        AVCodec *pic;
        pic = *codec;
        av_opt_set(c->priv_data, "tune", "zerolatency", 0);
        av_opt_set(c->priv_data, "preset", "superfast", 0);
        c->codec_id = codec_id;
        //码率：影响体积，与体积成正比：码率越大，体积越大；码率越小，体积越小。
        c->bit_rate = 4000000; //设置码率 码率会影响视频的清晰度 400kps
        /*分辨率必须是2的倍数。 */
        c->width    = kvideo_width_;
        c->height   = kvideo_height_;
        /*时基：这是基本的时间单位（以秒为单位）
         *表示其中的帧时间戳。 对于固定fps内容，
         *时基应为1 / framerate，时间戳增量应为
         *等于1。*/
        ost->stream->time_base = (AVRational){1,stream_frame_rate_};
        c->time_base       = ost->stream->time_base;
        c->gop_size      = 5; /* 最多每十二帧发射一帧I帧 */
        c->pix_fmt       = *pic->pix_fmts;
        c->max_b_frames = 0;  //不要B帧
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER | AV_CODEC_FLAG_LOW_DELAY;
        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO)
        {
            c->mb_decision = 2;
        }
        break;

    default:
        break;
    }
}

AVFrame *PushStream::AllocPictureMemory(AVPixelFormat pix_format, int width, int height)
{
    AVFrame *picture;
    picture = av_frame_alloc();
    picture->format = pix_format;
    picture->width  = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    av_frame_get_buffer(picture, 32);
    return picture;
}

AVFrame *PushStream::AllocAudioFrame(AVSampleFormat sample_format, uint64_t channel_layout, int sample_rate, int nb_sample)
{
    AVFrame *frame = av_frame_alloc();
    frame->format = sample_format;
    frame->channels = av_get_channel_layout_nb_channels(channel_layout);
    frame->channel_layout = channel_layout;
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_sample;
    if(nb_sample)
    {
        av_frame_get_buffer(frame, 0);
    }
    return frame;
}

void PushStream::OpenVideoOutput(AVCodec *codec, OutputStream *ost, AVDictionary *dictory)
{
    AVCodecContext *c = ost->encodec_context;
    AVDictionary *opt = NULL;
    av_dict_copy(&opt, dictory, 0);
    /* open the codec */
    avcodec_open2(c, codec, &opt);
    av_dict_free(&opt);
    /* allocate and init a re-usable frame */
    ost->convert_frame = AllocPictureMemory(c->pix_fmt, c->width, c->height);
    ost->temporary_frame = NULL;
    /* 将流参数复制到多路复用器 */
    avcodec_parameters_from_context(ost->stream->codecpar, c);
}

void PushStream::OpenAudioOutput(AVCodec *codec, OutputStream *ost, AVDictionary *dictory)
{
    AVCodecContext *c;
    int nb_samples;
    int ret;
    AVDictionary *opt = NULL;
    c = ost->encodec_context;
    av_dict_copy(&opt, dictory, 0);
    av_dict_set(&opt, "audio_buffer_size","30", 0);
    ret = avcodec_open2(c, codec, &opt);
    if(ret < 0){
        printf("open faile\n");
    }
    av_dict_free(&opt);


    /*下面3行代码是为了生成虚拟的声音设置的频率参数*/
    ost->t     = 0;
    ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
    ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

    //AAC编码这里就固定为1024
    nb_samples = c->frame_size;

    ost->convert_frame     = AllocAudioFrame(c->sample_fmt, c->channel_layout,
                                       c->sample_rate, nb_samples);
    ost->temporary_frame = AllocAudioFrame(audio_encodec_context_->sample_fmt, c->channel_layout,c->sample_rate, nb_samples);

    /* copy the stream parameters to the muxer */
    avcodec_parameters_from_context(ost->stream->codecpar, c);

    /* create resampler context */
    ost->swr_context = swr_alloc();

    if(!ost->swr_context){
        fprintf(stdout, "Could not allocate resampler context\n");
    }
    swr_alloc_set_opts(ost->swr_context,
                       c->channel_layout,
                       c->sample_fmt,
                       c->sample_rate,
                       c->channel_layout,
                       audio_encodec_context_->sample_fmt,
                       audio_encodec_context_->sample_rate,
                       0,
                       NULL);
    swr_init(ost->swr_context);
}

void PushStream::OpenOutput(std::string file_name)
{
    AVOutputFormat *fmt;
    AVCodec *audio_codec, *video_codec;
    int ret;
    AVDictionary *opt = NULL;

    /* 分配输出环境*/
    avformat_alloc_output_context2(&output_format_context_,NULL,"flv",file_name.data());
    fmt=output_format_context_->oformat;
    //指定编码器
    fmt->video_codec=AV_CODEC_ID_H264;
    fmt->audio_codec=AV_CODEC_ID_AAC;

    /*使用默认格式的编解码器添加音频和视频流，初始化编解码器。 */
    if(fmt->video_codec != AV_CODEC_ID_NONE)
    {
        AddStream(&video_stream_,output_format_context_,&video_codec,fmt->video_codec);
        have_video_ = 1;
    }
    if(fmt->audio_codec != AV_CODEC_ID_NONE)
    {
        AddStream(&audio_stream_, output_format_context_, &audio_codec, fmt->audio_codec);
        have_audio_ = 1;
    }

    /*现在已经设置了所有参数，可以打开音频视频编解码器，并分配必要的编码缓冲区。 */
    if (have_video_)
        OpenVideoOutput(video_codec, &video_stream_, opt);

    if (have_audio_)
        OpenAudioOutput(audio_codec, &audio_stream_, opt);

    av_dump_format(output_format_context_, 0, file_name.data(), 1);

    /* 打开输出文件（如果需要） */
    if(!(fmt->flags & AVFMT_NOFILE))
    {
        printf("open\n");
        ret = avio_open2(&output_format_context_->pb, file_name.data(), AVIO_FLAG_READ_WRITE,nullptr,nullptr);
        if (ret < 0)
        {
            fprintf(stderr, "无法打开输出文件: '%s':\n", file_name.data());
            exit(0);
        }
    }

    /* 编写流头（如果有）*/
    ret = avformat_write_header(output_format_context_,&opt);
    if(ret == AVSTREAM_INIT_IN_WRITE_HEADER){
        printf("write header success\n");
    }
}

AVFrame *PushStream::GetAudioFrame(AVFrame *frame)
{
    AVFrame *mframe = frame;
//        AllocAudioFrame(AV_SAMPLE_FMT_S16, ost->encodec_context->channel_layout,ost->encodec_context->sample_rate,128);
    if (av_frame_make_writable(mframe) < 0){
        printf("get audio frame\n");
//        exit(1);
    }
    /* 检查我们是否要生成更多帧，用于判断是否结束*/
//    if (av_compare_ts(ost->next_pts, ost->encodec_context->time_base,STREAM_DURATION, (AVRational){ 1, 1 }) >= 0)
//        return NULL;

#if 1
    //获取链表节点数量
    int cnt=0;
    int64_t start = av_gettime();
    while(cnt<=0)
    {
        cnt=CountFrameNode(audio_frame_head_);
        if(av_gettime() - start > ktimeout_)return nullptr;
        //        std::cout<<"cnt  "<<cnt<<std::endl;
    }

    pthread_mutex_lock(&audio_mutex_); /*互斥锁上锁*/

    //得到节点数据
    struct FrameData *tmp=audio_frame_head_;

    tmp=tmp->next;
    if(tmp==NULL)
    {
        printf("数据为NULL.\n");
        exit(0);
    }
    //    printf("audiotime sec %ld, usec %ld\n",tmp->time.tv_sec,tmp->time.tv_usec);
    //    printf("videotime sec %ld, usec %ld\n",videotime.tv_sec,videotime.tv_usec);
    //    printf("distance sec %ld, usec %ld\n",tmp->time.tv_sec - videotime.tv_sec, videotime.tv_usec - tmp->time.tv_usec);

    //1024*16*1
    av_frame_unref(frame);
    av_frame_move_ref(frame,tmp->frame);
//    cout << "frame1->buf[0] ref count = " << av_buffer_get_ref_count(tmp->frame->buf[0]) << endl;
//    av_frame_unref(tmp->frame);
    DeleteFrameNode(audio_frame_head_,tmp->frame);
//    av_frame_unref(tmp->frame);
    pthread_mutex_unlock(&audio_mutex_); /*互斥锁解锁*/
#endif

//    int64_t end = frame->pts;

//    frame->pts = ost->next_pts;
////        printf("frame %ld\n",frame->pts);
//    ost->next_pts  += frame->nb_samples;
//    AVRational s{1,1000000};
//    int64_t mid = av_rescale_q(frame->pts,audio_stream_.enc->time_base,s);
//    time_interval_ = end - start_time_ - mid;
//    printf("two time glag %ld , midden %ld, mid %ld\n",time_interval_,end-start_time_,mid);
    return frame;
}

AVFrame *PushStream::ReadVideoFrame()
{
    AVFrame *frame = av_frame_alloc();
//    if (av_frame_make_writable(frame) < 0){
//        std::cout<<"get video frame\n"<<std::endl;
////        printf("get video frame\n");
//        exit(1);
//    }

#if 1
//    std::cout<<"play get video frame\n"<<std::endl;
    //获取链表节点数量
    int cnt=0;
    while(cnt<=0)
    {
        cnt=CountFrameNode(play_frame_head_);
    }

    pthread_mutex_lock(&play_mutex_); /*互斥锁上锁*/

    //得到节点数据
    struct FrameData *tmp=play_frame_head_;

    tmp=tmp->next;
    if(tmp==NULL)
    {
        printf("数据为NULL.\n");
        exit(0);
    }

    //1024*16*1
//    av_frame_unref(frame);
    av_frame_move_ref(frame,tmp->frame);

    DeleteFrameNode(play_frame_head_,tmp->frame);
//    av_frame_unref(tmp->frame);
    pthread_mutex_unlock(&play_mutex_); /*互斥锁解锁*/
#endif

    SwsContext *img_convert_ctx = nullptr;
    if(out_buffer){
        delete out_buffer;
        out_buffer = nullptr;
    }
    AVFrame *pFrameYUV = av_frame_alloc();
    out_buffer=(uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, video_encodec_context_->width, video_encodec_context_->height,1));

    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P,video_encodec_context_->width, video_encodec_context_->height, 1);

    img_convert_ctx = sws_getContext(video_encodec_context_->width, video_encodec_context_->height, video_encodec_context_->pix_fmt, video_encodec_context_->width, video_encodec_context_->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    sws_scale(img_convert_ctx, (const uint8_t* const*)frame->data, frame->linesize, 0, video_encodec_context_->height,pFrameYUV->data, pFrameYUV->linesize);
    pFrameYUV->width = video_encodec_context_->width;
    pFrameYUV->height = video_encodec_context_->height;


    av_frame_free(&frame);
    sws_freeContext(img_convert_ctx);
    return pFrameYUV;
}

AVFrame *PushStream::GetVideoFrame(OutputStream *ost)
{
    AVFrame *frame = ost->convert_frame;
    if (av_frame_make_writable(frame) < 0){
        printf("get video frame\n");
        exit(1);
    }
    /* 检查我们是否要生成更多帧，用于判断是否结束*/
//    if (av_compare_ts(ost->next_pts, ost->encodec_context->time_base,STREAM_DURATION, (AVRational){ 1, 1 }) >= 0)
//        return NULL;

#if 1
    //获取链表节点数量
    int cnt=0;
    int64_t start = av_gettime();
    while(cnt<=0)
    {
        cnt=CountFrameNode(video_frame_head_);
        if(av_gettime() - start > ktimeout_)return nullptr;
        //        std::cout<<"cnt  "<<cnt<<std::endl;
    }

    pthread_mutex_lock(&video_mutex_); /*互斥锁上锁*/

    //得到节点数据
    struct FrameData *tmp=video_frame_head_;

    tmp=tmp->next;
    if(tmp==NULL)
    {
        printf("数据为NULL.\n");
        exit(0);
    }
    //    printf("audiotime sec %ld, usec %ld\n",tmp->time.tv_sec,tmp->time.tv_usec);
    //    printf("videotime sec %ld, usec %ld\n",videotime.tv_sec,videotime.tv_usec);
    //    printf("distance sec %ld, usec %ld\n",tmp->time.tv_sec - videotime.tv_sec, videotime.tv_usec - tmp->time.tv_usec);

    //1024*16*1
    av_frame_unref(frame);
    av_frame_move_ref(frame,tmp->frame);

//    av_frame_unref(tmp->frame);
//    av_frame_unref(tmp->frame);
    DeleteFrameNode(video_frame_head_,tmp->frame);
//    av_frame_unref(tmp->frame);
    pthread_mutex_unlock(&video_mutex_); /*互斥锁解锁*/
#endif

    int64_t end = frame->pts;
    frame->pts = ost->next_pts++;
    AVRational s{1,1000000};
    int64_t mid = av_rescale_q(frame->pts,video_stream_.encodec_context->time_base,s);
    if(mid < 0)printf("av_rescale_q faile\n");
    int64_t end1 = end - start_time_ - time_interval_;
//    cout<<end1<<endl;
    end1 = av_rescale_q(end1,s,video_stream_.encodec_context->time_base);

//    printf("video time_interval_ %ld \n",end - start_time_ - mid);
//    printf("end1 %ld, frame pts %ld\n",end1,frame->pts);
    frame->pts = end1;
    return frame;
}

AVPacket *PushStream::GetSendPacket(PacketData *head)
{
    AVPacket *packet = av_packet_alloc();

#if 1
    //获取链表节点数量
    int cnt=0;
    int64_t start = av_gettime();
    while(cnt<=0)
    {
        cnt=CountPacketNode(head);
        if(av_gettime() - start > ktimeout_)return nullptr;
        //        std::cout<<"cnt  "<<cnt<<std::endl;
    }

    if(head == video_packet_head_)
        pthread_mutex_lock(&video_mutex_packet_); /*互斥锁上锁*/
    else
        pthread_mutex_lock(&audio_mutex_packet_);

    //得到节点数据
    struct PacketData *tmp= head;

    tmp=tmp->next;
    if(tmp==NULL)
    {
        printf("数据为NULL.\n");
        exit(0);
    }
    //    printf("audiotime sec %ld, usec %ld\n",tmp->time.tv_sec,tmp->time.tv_usec);
    //    printf("videotime sec %ld, usec %ld\n",videotime.tv_sec,videotime.tv_usec);
    //    printf("distance sec %ld, usec %ld\n",tmp->time.tv_sec - videotime.tv_sec, videotime.tv_usec - tmp->time.tv_usec);

    //1024*16*1
//    printf("acquir video packet pts %ld\n",tmp->packet->pts);
    av_packet_move_ref(packet,tmp->packet);
//    av_packet_unref(tmp->packet);
//    packet->pts = tmp->packet->pts;


    DeletePacketNode(head,tmp->packet);
    if(head == video_packet_head_)
        pthread_mutex_unlock(&video_mutex_packet_); /*互斥锁解锁*/
    else
        pthread_mutex_unlock(&audio_mutex_packet_);
#endif

    return packet;
}

int PushStream::EncodeAudio()
{
    OutputStream *ost = &audio_stream_;
    AVCodecContext *c = nullptr;
    AVPacket *pkt = nullptr;
    AVFrame *frame = nullptr;
    int ret = 0;
    int dst_nb_samples = 0;

    pkt = av_packet_alloc();
    c = ost->encodec_context;
    AVFrame *mframe = av_frame_alloc();

    int sample_byte = av_get_bytes_per_sample(audio_encodec_context_->sample_fmt);
    frame = GetAudioFrame(mframe);
    int sample_len = frame->nb_samples*sample_byte*ost->encodec_context->channels;
    int64_t end = frame->pts;
    int dst_len = ost->convert_frame->nb_samples*sample_byte*ost->encodec_context->channels;
    char * audio_buffer = (char *)malloc(sample_len);
    char * tmp_buffer = (char *)malloc(dst_len);
    int shift = 0;
    int audiolen = 0;
    while (close_push_stream_ != 3) {
//        printf("sdfdslkf\n");
//        int size = snd_pcm_format_width((snd_pcm_format_t)ost->temporary_frame->format)/8;

//        printf("requir size %ld\n",ost->convert_frame->nb_samples*sizeof (int16_t)*ost->encodec_context->channels);
        if(sample_len != dst_len){
            if(sample_len < dst_len){
                int readylen = 0;
                if(audiolen){
                    memcpy(tmp_buffer,audio_buffer,audiolen);
                    readylen = audiolen;
                    audiolen = 0;
                }
                int i = 1;
                while(readylen < dst_len){
                    frame = GetAudioFrame(mframe);
                    if(!frame)break;
                    if(i){
                        end = frame->pts;
                        i = 0;
                    }
                    if(sample_len + readylen <= dst_len){
                        memcpy(tmp_buffer + readylen,frame->data[0],sample_len);
                        readylen += sample_len;
                    }else{
                        int len = dst_len - readylen;
                        audiolen = sample_len - len;
                        memcpy(tmp_buffer + readylen,frame->data[0],len);
                        memcpy(audio_buffer,frame->data[0] + len,audiolen);
                        readylen = dst_len;
                    }
                }
            }else{
                if(audiolen > 0 && audiolen < dst_len){
                    memcpy(tmp_buffer,audio_buffer + shift,audiolen);
                    frame = GetAudioFrame(mframe);
                    if(!frame)continue;
                    int len = dst_len - audiolen;
                    memcpy(tmp_buffer + audiolen,frame->data[0],len);
                    audiolen = sample_len - len;
                    memcpy(audio_buffer,frame->data[0] + len,audiolen);
                    shift = 0;
                }else if(audiolen >= dst_len){
                    if(audiolen ==  dst_len){
                        memcpy(tmp_buffer,audio_buffer + shift,audiolen);
                        audiolen = 0;
                    }else{
                        memcpy(tmp_buffer,audio_buffer + shift,ost->encodec_context->frame_size);
                        shift += dst_len;
                        audiolen -= dst_len;
                    }
                }else if(audiolen == 0){
                    audiolen = sample_len - dst_len;
                    memcpy(tmp_buffer,frame->data[0],dst_len);
                    memcpy(audio_buffer,frame->data[0],audiolen);
                    shift = 0;
                }

            }
            if(!frame)continue;
            memcpy(ost->temporary_frame->data[0],tmp_buffer,ost->convert_frame->nb_samples*sample_byte*ost->encodec_context->channels);
            frame = ost->temporary_frame;
        }else{
            frame = GetAudioFrame(mframe);
            if(!frame)continue;
            end = frame->pts;
        }

//        cout<<"size of input frame "<<frame->nb_samples*sizeof(int16_t)*ost->encodec_context->channels<<endl;
//        q = q + frame->nb_samples*sizeof (int16_t)*ost->encodec_context->channels;
//        av_frame_free(&frame);
//            printf("frame pts %ld, %ld\n",ost->temporary_frame->nb_samples*sizeof (int16_t)*ost->encodec_context->channels,q - (int16_t *)audio_buffer);
//            q = (int16_t *)ost->temporary_frame->data[0];
//            q = (int16_t*)audio_buffer;
        frame->pts = ost->next_pts;
    //        printf("frame %ld\n",frame->pts);
        ost->next_pts  += frame->nb_samples;
        AVRational s{1,1000000};
        int64_t mid = av_rescale_q(frame->pts,audio_stream_.encodec_context->time_base,s);
        time_interval_ = end - start_time_;
        time_interval_ = time_interval_ -  mid;



        if(frame)
        {
            /*使用重采样器将样本从本机格式转换为目标编解码器格式*/
            /*计算样本的目标数量*/
            dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_context, c->sample_rate) + frame->nb_samples,c->sample_rate, c->sample_rate, AV_ROUND_UP);
//            printf("swr init sample %d %d\n",dst_nb_samples,ost->temporary_frame->nb_samples);
            av_assert0(dst_nb_samples == ost->temporary_frame->nb_samples);
//            if(!av_frame_make_writable(frame))printf("frame is writable\n");
//            printf("swr init sample %d %d\n",dst_nb_samples,frame->nb_samples);
            /*转换为目标格式 */
            int ret = swr_convert(ost->swr_context,ost->convert_frame->data, 1024,(const uint8_t **)frame->data, frame->nb_samples);
            if(ret < 0)
                printf("swr_convert fail\n");
            frame = ost->convert_frame;
            //        printf("frame1 %ld\n",frame->pts);
            //        printf("samples count %d\n",ost->samples_count);

        }

        ost->convert_frame->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
        //        printf("frame2 %ld\n",ost->convert_frame->pts);
        ost->samples_count += 1024;
//        printf("swr nb_sameple %d \n",frame->nb_samples);
        ret = avcodec_send_frame(c,ost->convert_frame);

        if(ret != 0){
            //        printf("send audio frame fail\n");
            free(audio_buffer);
            free(tmp_buffer);
            av_frame_free(&mframe);
            av_packet_free(&pkt);
            return -1;
        }else{
            //            printf("send a audio frame to encode success\n");
        }

        //    avcodec_encode_audio2(c, pkt, frame, &got_packet);
        //    printf("frame pts %ld\n",pkt.pts);
        //    printf("audio pts %d\n",frame->pts);
        while(!avcodec_receive_packet(c,pkt))
        {
            //                                    printf(" audio \n");
            //        printf("frame1 %ld\n",ost->convert_frame->pts);
            //        printf("pkt pts %ld\n",pkt->pts);
            //        pkt->pts = ost->convert_frame->pts;
            pthread_mutex_lock(&audio_mutex_packet_); /*互斥锁上锁*/
//            printf("audio frame pts %ld , packet pts %ld ,dts %ld ,duration %ld, timebase %d , in %d\n",frame->pts,pkt->pts,pkt->dts,pkt->duration,audio_stream_.enc->time_base.den,video_encodec_context_->time_base.den);
            av_packet_rescale_ts(pkt, audio_stream_.encodec_context->time_base, audio_stream_.stream->time_base);
//            printf("pts %ld\n",pkt->pts);
//            WritePacket(oc,&audio_stream_.enc->time_base,audio_stream_.st,pkt);
            pkt->dts = pkt->pts;
            AddPacketNode(audio_packet_head_,pkt);
            pthread_mutex_unlock(&audio_mutex_packet_);
//            av_packet_unref(pkt);
            if(ret != 0)
                printf("audio ret %d\n",ret);
        }
        av_packet_unref(pkt);
//        cout << "frame1->buf[0] ref count = " << av_buffer_get_ref_count(frame->buf[0]) << endl;
//        av_frame_unref(frame);
//        av_frame_free(&frame);
    }
    free(audio_buffer);
    free(tmp_buffer);
    av_frame_free(&mframe);
    av_packet_free(&pkt);
    close_push_stream_ = 4;
    printf("cout sdslfsd            %d\n",close_push_stream_);
    return (frame) ? 0 : 1;
}

int PushStream::EncodeVideo()
{

    OutputStream *ost = &video_stream_;
    int ret = 0;
    AVCodecContext *c;
    AVFrame *frame;
    AVPacket *pkt = { 0 };
    c=ost->encodec_context;
    //        if(c)printf("encode video codec is open\n");
    pkt = av_packet_alloc();
    while (close_push_stream_ != 4) {


        //获取一帧数据
        frame = GetVideoFrame(ost);
        if(!frame)continue;

        //    av_init_packet(&pkt);

        /* 编码图像 */
        ret = avcodec_send_frame(c,frame);
        if(ret != 0){
            printf("send video frame fail\n");
            return -1;
        }else{
            //            printf("send a video frame to encode success\n");
        }
        //    ret=avcodec_encode_video2(c, pkt, frame, &got_packet);
        //        printf("video\n");
        if(!avcodec_receive_packet(c,pkt))
        {
            //                            printf(" video  \n");
            //        printf("video pts %ld\n",pkt->pts);
            pthread_mutex_lock(&video_mutex_packet_); /*互斥锁上锁*/
//            printf("video frame pts %ld, packet pts %ld\n",frame->pts,pkt->pts);
            av_packet_rescale_ts(pkt, video_stream_.encodec_context->time_base, video_stream_.stream->time_base);
//            WritePacket(oc,&video_stream_.enc->time_base,video_stream_.st,pkt);
//            printf("video pts %ld\n",pkt->pts);
            pkt->dts = pkt->pts;
            AddPacketNode(video_packet_head_,pkt);
            pthread_mutex_unlock(&video_mutex_packet_);

            if(ret != 0)
                printf("video ret %d\n",ret);
        }
        av_packet_unref(pkt);
//        av_frame_unref(frame);
//        av_frame_free(&frame);

    }
    av_packet_free(&pkt);
    close_push_stream_ = 5;
    printf("cout sdslfsd            %d\n",close_push_stream_);
    return 1;
}

int PushStream::WritePacket(AVFormatContext *format_context, AVStream *st, AVPacket *pkt)
{
    pkt->stream_index = st->index; //stream_index 用于标识该avpacket的类型（音频视频）
    //    printf("reindex %d\n",pkt->stream_index);

    /*将压缩的帧写入媒体文件*/
    return av_interleaved_write_frame(format_context, pkt);
}

void PushStream::CloseStream(OutputStream *ost)
{
    avcodec_free_context(&ost->encodec_context);
    av_frame_free(&ost->convert_frame);
    av_frame_free(&ost->temporary_frame);
//    av_free(ost->stream);
    sws_freeContext(ost->sws_context);
    swr_free(&ost->swr_context);
}

int PushStream::SendPacket()
{
    AVPacket *videoPacket = av_packet_alloc();
    AVPacket *audioPacket = av_packet_alloc();
    int encodevideo = 0, encodeaudio = 0;
    while (close_push_stream_ != 5) {
        if(!encodevideo){
//            av_packet_unref(videoPacket);
            videoPacket = GetSendPacket(video_packet_head_);
            if(!videoPacket)continue;
//            printf("return video pts %ld\n",videoPacket->pts);
            encodevideo = 1;
        }
        if(!encodeaudio){
//            av_packet_unref(audioPacket);
            audioPacket = GetSendPacket(audio_packet_head_);
            if(!audioPacket)continue;
            encodeaudio = 1;
        }
//        printf("audio pts %ld, video pts %ld\n",audioPacket->pts,videoPacket->pts);
        if(videoPacket->pts > audioPacket->pts){
            WritePacket(output_format_context_, audio_stream_.stream, audioPacket);
            av_packet_free(&audioPacket);
            encodeaudio = 0;
//            printf("send audio packet success\n");
        }else{
            WritePacket(output_format_context_, video_stream_.stream, videoPacket);
            av_packet_free(&videoPacket);
            encodevideo = 0;
//            printf("send video packet success\n");
        }

    }


    av_write_trailer(output_format_context_);


    close_push_stream_ = 6;
    return 0;
}

void PushStream::OpenPushStream(string address, int select)
{
    qDebug()<<"open push stream";
    rtmp_server_addr_ = address;
    play_frame_head_ = nullptr;
    audio_frame_head_ = nullptr;
    video_frame_head_ = nullptr;
    audio_packet_head_ = nullptr;
    video_packet_head_ = nullptr;

    //创建链表头
    play_frame_head_ = CreateFrameHead(play_frame_head_);
    audio_frame_head_ = CreateFrameHead(audio_frame_head_);
    video_frame_head_ = CreateFrameHead(video_frame_head_);
    audio_packet_head_ = CreatePacketHead(audio_packet_head_);
    video_packet_head_ = CreatePacketHead(video_packet_head_);

    /*初始化互斥锁*/
    pthread_mutex_init(&play_mutex_,NULL);
    pthread_mutex_init(&video_mutex_,NULL);
    pthread_mutex_init(&video_mutex_packet_,NULL);
    pthread_mutex_init(&audio_mutex_,NULL);
    pthread_mutex_init(&audio_mutex_packet_,NULL);

    /*初始化摄像头设备*/
    //    err=VideoDeviceInit("/dev/video0");// /dev/video0
    InitRecordEquipment();
    int audioindex = OpenAudioDevice("default","alsa");
    int videoindex = -1;
    // /dev/video0 :0.0+0,0   // video4linux2 x11grab
    if(select == 0){
        videoindex = OpenVideoDevice(":0.0+0,0","x11grab");
    }else{
        videoindex = OpenVideoDevice("/dev/video0","video4linux2");
    }



    InitDecodeContext(video_input_context_->streams[videoindex]);
    InitDecodeContext(audio_input_context_->streams[audioindex]);

    OpenOutput(const_cast<char *>(rtmp_server_addr_.c_str()));

    if(video_encodec_context_)printf("video codec open success\n");
    if(audio_encodec_context_)printf("audio codec open success\n");

//    usleep(1000000);
    auto lambda_fun = [&]()->void{
        RecycleReadAudioData();
    };
    auto lambda_fun1 = [&]()->void{
        RecycleReadVideoData();
    };
    auto lambda_fun2 = [&]()->void{
        EncodeAudio();
    };
    auto lambda_fun3 = [&]()->void{
        EncodeVideo();
    };
    auto lambda_fun4 = [&]()->void{
        SendPacket();
    };
    thread readvideo(lambda_fun);//创建线程，线程起点
    thread readaudio(lambda_fun1);
    thread encodevideo(lambda_fun2);
    thread encodeaudio(lambda_fun3);
    thread sendpacket(lambda_fun4);
    readvideo.detach();
    readaudio.detach();
    encodevideo.detach();
    encodeaudio.detach();
    sendpacket.detach();
}

void PushStream::ClosePushStream()
{
    printf("close\n");
    close_push_stream_ = 1;
//    usleep(1000000);
    while (close_push_stream_ != 6) {
//        printf("close\n");
    }
    printf("close success %d\n",close_push_stream_);
    pthread_mutex_destroy(&play_mutex_);
    pthread_mutex_destroy(&video_mutex_);
    pthread_mutex_destroy(&video_mutex_packet_);
    pthread_mutex_destroy(&audio_mutex_);
    pthread_mutex_destroy(&audio_mutex_packet_);
    if (have_video_)
        CloseStream(&video_stream_);
    if (have_audio_)
        CloseStream(&audio_stream_);

    if (!(output_format_context_->oformat->flags & AVFMT_NOFILE))
        avio_closep(&output_format_context_->pb);
    if(output_format_context_){
        avformat_free_context(output_format_context_);
    }
    printf("open clear node\n");
    if(audio_frame_head_)
        ClearFrameNode(audio_frame_head_);
    if(audio_frame_head_ == nullptr)printf("audio head is nullptr\n");
    if(video_frame_head_)
        ClearFrameNode(video_frame_head_);
    if(play_frame_head_)
        ClearFrameNode(play_frame_head_);
    if(audio_packet_head_)
        ClearPacketNode(audio_packet_head_);
    if(video_packet_head_)
        ClearPacketNode(video_packet_head_);
}

﻿// 视频输出类
#ifndef VIDEOOUTSDL_H
#define VIDEOOUTSDL_H
#include <SDL2/SDL.h>
#include "mediabase.h"
namespace LIVE {
//定义分辨率
// 窗口分辨率
#define WIN_WIDTH  320
#define WIN_HEIGHT  240
// YUV像素分辨率
#define YUV_WIDTH   320
#define YUV_HEIGHT  240
//定义YUV格式
#define YUV_FORMAT  SDL_PIXELFORMAT_IYUV

class VideoOutSDL {
 public:
  VideoOutSDL();
  virtual ~VideoOutSDL();
  virtual RET_CODE Init(const Properties& properties);
  virtual RET_CODE Output(uint8_t* video_buf, uint32_t size);
 private:
  // SDL
  SDL_Event event;                            // 事件
  SDL_Rect rect;                              // 矩形
  SDL_Window* win = NULL;                     // 窗口
  SDL_Renderer* renderer = NULL;              // 渲染
  SDL_Texture* texture = NULL;                // 纹理
  uint32_t pixformat = YUV_FORMAT;            // YUV420P，即是SDL_PIXELFORMAT_IYUV

  // 分辨率
  int video_width = YUV_WIDTH;
  int video_height = YUV_HEIGHT;
  int win_width = YUV_WIDTH;
  int win_height = YUV_WIDTH;
};
}


#endif // VIDEOOUTSDL_H

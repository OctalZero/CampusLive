#include "SDLPlay.h"
extern "C" {
#include "SDL2/SDL.h"
}
#pragma comment(lib, "SDL2.lib")

int SDLPlay::m_nRef = 0;
SDLPlay::SDLPlay(QWidget *parent)
    : QWidget(parent),
      m_pTexture(nullptr)
    //m_bEmpty(true)
{
    setUpdatesEnabled(false);
    SDL_Related_Init();
    this->resize(1280, 720); // TODO: 换成接口
    m_pWindow = SDL_CreateWindowFrom((void*)winId());
    //m_pWindow = SDL_CreateWindowFrom((void*)this->winId());

    m_pRender = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

SDLPlay::~SDLPlay()
{
    if (m_pWindow)
        SDL_DestroyWindow(m_pWindow);
    if (m_pRender)
        SDL_DestroyRenderer(m_pRender);
    if (m_pTexture)
        SDL_DestroyTexture(m_pTexture);
    SDL_Related_Uninit();
}

void SDLPlay::Clear()
{

}

void SDLPlay::PresentFrame(const unsigned char* pBuffer, int nImageWidth, int nImageHeight)
{
    if (!m_pRender) {
        printf("Render not Create\n");
    }
    else {
        int nTextureWidth = 0, nTextureHeight = 0;
        //首先查询当前纹理对象的宽高，如果不符合，那么需要重建纹理对象
        SDL_QueryTexture(m_pTexture, nullptr, nullptr, &nTextureWidth, &nTextureHeight);
        if (nTextureWidth != nImageWidth || nTextureHeight != nImageHeight) {
            if (m_pTexture)
                SDL_DestroyTexture(m_pTexture);
            //这里指定了渲染的数据格式，访问方式和宽高大小
            m_pTexture = SDL_CreateTexture(m_pRender, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
                                           nImageWidth, nImageHeight);
        }
    }

    if (!m_pTexture) {
        printf("YUV Texture Create Failed\n");
    }
    else {
        //用新数据刷新纹理
        SDL_UpdateTexture(m_pTexture, nullptr, pBuffer, nImageWidth);
        //清除当前渲染
        SDL_RenderClear(m_pRender);
        //拷贝纹理对象到渲染器中
        SDL_RenderCopy(m_pRender, m_pTexture, nullptr, nullptr);
        //最终渲染
        SDL_RenderPresent(m_pRender);
    }
}

void SDLPlay::SDL_Related_Init()
{
    if (0 == m_nRef++)
    {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    }
}

void SDLPlay::SDL_Related_Uninit()
{
    if (0 == --m_nRef)
    {
        SDL_Quit();
    }
}

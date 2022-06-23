#pragma once
#include <QWidget>
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Window;

class SDLPlay : public QWidget
{
    Q_OBJECT

public:
    SDLPlay(QWidget *parent = 0);
    ~SDLPlay();

    void Clear();

protected:
    //virtual void resizeEvent(QResizeEvent *event);

private:
    static void SDL_Related_Init();
    static void SDL_Related_Uninit();

public slots:
//根据传入数据流显示视频
    void PresentFrame(const unsigned char* pBuffer, int nImageWidth, int nImageHeight);
private:
    SDL_Renderer*        m_pRender;
    SDL_Texture*         m_pTexture;
    SDL_Window*          m_pWindow;

    static int            m_nRef;        //引用计数来确定SDL全局资源的创建和回收
};


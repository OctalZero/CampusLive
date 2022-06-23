/*********************************************************************************
  *Date:  2022.04.23
  *Description:  显示视频图像的类，多继承于 QOpenGLWidget，QOpenGLFunctions，IVideoCall
  *				 主要通过 OpenGL 处理视频图像的显示。
**********************************************************************************/
#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <mutex>
#include "IVideoCall.h"
struct AVFrame;

class OwlVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions, public IVideoCall
{
	Q_OBJECT

public:
	// 初始化
	virtual void Init(int width, int height) override;

	// 重绘图像，不管成功与否都释放frame空间
	virtual void Repaint(AVFrame* frame) override;

	OwlVideoWidget(QWidget* parent);
	~OwlVideoWidget() = default;
protected:
	// 初始化OpenGL
	void initializeGL() override;

	// 刷新显示
	void paintGL() override;

	// 窗口尺寸变化
	void resizeGL(int width, int height) override;
private:
	std::mutex mutex_;
	QGLShaderProgram program;  // shader程序
	GLuint unis[3] = { 0 };  // shader中yuv变量地址
	GLuint texs[3] = { 0 };  // OpenGL的 texture地址
	unsigned char* datas[3] = { 0 };  // 材质内存空间
	int width_ = 0;  // 视频图像宽度
	int height_ = 0;  // 视频图像长度

};
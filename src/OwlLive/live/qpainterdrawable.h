#ifndef QPAINTERDRAWABLE_H
#define QPAINTERDRAWABLE_H

#include <QMutex>
#include <QImage>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QTimer>
#include "mediabase.h"
#include "imagescale.h"
#include "framequeue.h"
#include "qevent.h"
using namespace LIVE;

class QPainterDrawable : public QQuickPaintedItem {
  Q_OBJECT

 public:
  explicit QPainterDrawable(QQuickItem* parent = nullptr);
  ~QPainterDrawable();
 public slots:
  int draw(const Frame* newVideoFrame, bool, bool);
 private:
  void paint(QPainter* painter) override;
  bool event(QEvent*) override;
 private:
  int x_ = 0; //  起始位置
  int y_ = 0;
  int video_width = 0;
  int video_height = 0;
  int img_width = 0;
  int img_height = 0;
  QImage img;
  VideoFrame dst_video_frame_;
  QMutex m_mutex;
  ImageScaler* img_scaler_ = NULL;
};

#endif // QPAINTERDRAWABLE_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QQuickWindow>
#include <QQuickStyle>
#include <QProcess>
#include "AppInfo.h"
#include "ChatController.h"
#include "live/qpainterdrawable.h"
#include "live/dlog.h"
#include "LiveController.h"
#include "LoginController.h"
#include "CourseController.h"
#include "DiscussController.h"
#include "PlaybackController.h"

// 解除SDL库中的宏定义
#undef main


int main(int argc, char* argv[]) {
  init_logger("live.log", S_INFO); // 初始化日志
  Client* client = &Client::GetClient();

//  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
//  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

  QQuickStyle::setStyle("Basic");
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;
  QIcon icon(":/assert/icon.png");
  app.setWindowIcon(icon);
  // 注册C++类为QML类型
  qmlRegisterType<QPainterDrawable>("QPainterDrawable", 1, 0, "QPainterDrawable");
  qmlRegisterType<ChatController>("Controller", 1, 0, "ChatController");
  qmlRegisterType<LiveController>("Controller", 1, 0, "LiveController");
  qmlRegisterType<LoginController>("Controller", 1, 0, "LoginController");
  qmlRegisterType<CourseController>("Controller", 1, 0, "CourseController");
  qmlRegisterType<DiscussController>("Controller", 1, 0, "DiscussController");
  qmlRegisterType<PlaybackController>("Controller", 1, 0, "PlaybackController");

  engine.rootContext()->setContextProperty("appInfo", new AppInfo());
  const QUrl url(QStringLiteral("qrc:/App.qml"));
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
  &app, [url](QObject * obj, const QUrl & objUrl) {
    if (!obj && url == objUrl)
      QCoreApplication::exit(-1);
  }, Qt::QueuedConnection);
  engine.load(url);

  // 查看缩放比例
//  double dpr = QGuiApplication::primaryScreen()->devicePixelRatio();
//  qDebug() << dpr;

  return app.exec();
}


# 使用的QT模块
QT += quick quickcontrols2 concurrent network multimedia
# 编译使用的C++标准
CONFIG += c++11
# 启用关于使用已弃用 Qt 函数和类的警告信息、禁用 Qt 发出的警告信息的输出
DEFINES += QT_DEPRECATED_WARNINGS QT_NO_WARNING_OUTPUT
# 定义FFmpeg相关宏
DEFINES += __STDC_CONSTANT_MACROS

# 添加项目头文件
HEADERS += \
    AppInfo.h \
    Client.h \
    CourseController.h \
    DiscussController.h \
    LiveController.h \
    Network.h \
    PlaybackController.h \
    live/librtmp/amf.h \
    live/librtmp/bytes.h \
    live/librtmp/dh.h \
    live/librtmp/dhgroups.h \
    live/librtmp/handshake.h \
    live/librtmp/http.h \
    live/librtmp/log.h \
    live/librtmp/rtmp.h \
    live/librtmp/rtmp_sys.h \
    live/aacdecoder.h \
    live/audiodecodeloop.h \
    live/audiooutsdl.h \
    live/avsync.h \
    live/avtimebase.h \
    live/commonlooper.h \
    live/dlog.h \
    live/framequeue.h \
    live/h264decoder.h \
    live/imagescale.h \
    live/looper.h \
    live/mediabase.h \
    live/naluloop.h \
    live/packetqueue.h \
    live/pullwork.h \
    live/qpainterdrawable.h \
    live/rtmpbase.h \
    live/rtmpplayer.h \
    live/semaphore.h \
    live/timeutil.h \
    live/videodecodeloop.h \
    live/videooutputloop.h \
    live/videooutsdl.h \
    live/sonic.h \
    ChatController.h \
    Logincontroller.h \
    stdafx.h \
    types.h

# 添加项目源文件
SOURCES += \
        AppInfo.cpp \
        Client.cpp \
        CourseController.cpp \
        DiscussController.cpp \
        LiveController.cpp \
        Network.cpp \
        PlaybackController.cpp \
        live/librtmp/amf.c \
        live/librtmp/hashswf.c \
        live/librtmp/log.c \
        live/librtmp/parseurl.c \
        live/librtmp/rtmp.c \
        ChatController.cpp \
        live/aacdecoder.cpp \
        live/audiodecodeloop.cpp \
        live/audiooutsdl.cpp \
        live/avsync.cpp \
        live/avtimebase.cpp \
        live/commonlooper.cpp \
        live/dlog.cpp \
        live/framequeue.cpp \
        live/h264decoder.cpp \
        live/imagescale.cpp \
        live/looper.cpp \
        live/mediabase.cpp \
        live/naluloop.cpp \
        live/packetqueue.cpp \
        live/pullwork.cpp \
        live/qpainterdrawable.cpp \
        live/rtmpbase.cpp \
        live/rtmpplayer.cpp \
        live/timeutil.cpp \
        live/videodecodeloop.cpp \
        live/videooutputloop.cpp \
        live/videooutsdl.cpp \
        live/sonic.cpp \
        Logincontroller.cpp \
        main.cpp
# 添加Qt资源系统
RESOURCES += qml/qml.qrc

# 指定可执行文件输出目录
CONFIG(debug,debug|release) {
    DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/../../bin/debug)
} else {
    DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/../../bin/release)
}

# 复制Windows下所需dll到可执行文件目录
win32 {
    contains(QT_ARCH, i386) {
        COPYDLL = $$absolute_path($${_PRO_FILE_PWD_}/../../3rdparty/Win_x86/*.dll) $$DESTDIR
        contains(QMAKE_CC, cl) {
            QMAKE_PRE_LINK += $$QMAKE_COPY $$replace(COPYDLL, /, $$QMAKE_DIR_SEP)
        } else {
            QMAKE_PRE_LINK += $$QMAKE_COPY $$replace(COPYDLL, /, $$QMAKE_DIR_SEP)
        }
    } else {
        COPYDLL = $$absolute_path($${_PRO_FILE_PWD_}/../../3rdparty/Win_x86/*.dll) $$DESTDIR
        contains(QMAKE_CC, cl) {
            QMAKE_PRE_LINK += $$QMAKE_COPY $$replace(COPYDLL, /, $$QMAKE_DIR_SEP)
        } else {
            QMAKE_PRE_LINK += $$QMAKE_COPY $$replace(COPYDLL, /, $$QMAKE_DIR_SEP)
        }
    }
}


# 第三方库头文件搜索路径
INCLUDEPATH += ../../include
# 第三方库文件搜索路径
LIBS += -L$$absolute_path($${_PRO_FILE_PWD_}/../../lib)
# 链接的库文件
LIBS += -lavcodec \
        -lavformat \
        -lavutil \
        -lswresample \
        -lavfilter \
        -lswresample \
        -lswscale \
        -lSDL2 \
        -lws2_32 \
        -lwinmm
#        -lboost_system \
#        -lboost_thread

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

mac: {
    QMAKE_INFO_PLIST = Info.plist
}


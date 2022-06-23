QT       += core gui
QT += multimedia opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../OwlChat/ChatMessage.cpp \
    ../../OwlChat/OwlChat.cpp \
    ../../OwlPlay/PushStream.cpp \
    ../../OwlPlay/SDLPlay.cpp \
    Class.cpp \
    ClassroomControl.cpp \
    ClassroomWindow.cpp \
    ClientSocket.cpp \
    Course.cpp \
    GetInfoControl.cpp \
    HomepageControl.cpp \
    HomepageWinodw.cpp \
    InfoControl.cpp \
    LoginControl.cpp \
    LoginWindow.cpp \
    OwlLiveWindow.cpp \
    Password.cpp \
    RegisterControl.cpp \
    RegisterWindow.cpp \
    RemoteAttendClassControl.cpp \
    SetInfoControl.cpp \
    Student.cpp \
    WindowsControl.cpp \
    main.cpp

HEADERS += \
    ../../OwlChat/ChatMessage.h \
    ../../OwlChat/OwlChat.h \
    ../../OwlPlay/IPushStream.h \
    ../../OwlPlay/SDLPlay.h \
    Class.h \
    ClassroomControl.h \
    ClassroomWindow.h \
    ClientSocket.h \
    Course.h \
    GetInfoControl.h \
    HomepageControl.h \
    HomepageWinodw.h \
    InfoControl.h \
    LoginControl.h \
    LoginWindow.h \
    OwlLiveWindow.h \
    Password.h \
    RegisterControl.h \
    RegisterWindow.h \
    RemoteAttendClassControl.h \
    SetInfoControl.h \
    Student.h \
    Types.h \
    WindowsControl.h

FORMS += \
    ../ui/Chat.ui \
    ../ui/ClassMembers.ui \
    ../ui/Classroom.ui \
    ../ui/CourseCard.ui \
    ../ui/CourseDetails.ui \
    ../ui/Homepage.ui \
    ../ui/JoinCourseCard.ui \
    ../ui/JoinCourseForm.ui \
    ../ui/LiveConfig.ui \
    ../ui/Login.ui \
    ../ui/OwlChat.ui \
    ../ui/OwlPlay.ui \
    ../ui/Register.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../qrc/assets/avatar.png \
    ../qrc/assets/logo.png \
    ../qrc/assets/student_icon.png \
    ../qrc/assets/teacher_icon.png \
    OwlLive-teacher.pro.user

INCLUDEPATH += ../../../include
INCLUDEPATH += ../../OwlPlay
INCLUDEPATH += ../../OwlChat

#LIBS += ../../../lib/libavformat.so

#LIBS += ../../../lib/libavdevice.so

#LIBS += ../../../lib/libavcodec.so

#LIBS += ../../../lib/libavutil.so

#LIBS += ../../../lib/libswscale.so

#LIBS += ../../../lib/libswresample.so


LIBS += /usr/lib/libavformat.so

LIBS += /usr/lib/libavdevice.so

LIBS += /usr/lib/libavcodec.so

LIBS += /usr/lib/libavutil.so

LIBS += /usr/lib/libswscale.so

LIBS += /usr/lib/libswresample.so

LIBS += /usr/lib/libSDL2.so

RESOURCES += \
    ui.qrc

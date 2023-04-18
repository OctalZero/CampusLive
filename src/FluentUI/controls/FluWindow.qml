﻿import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls  2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

ApplicationWindow {


    enum LaunchMode {
        Standard,
        SingleTask,
        SingleInstance
    }

    default property alias content: container.data
    property int launchMode: FluWindow.Standard
    property string route
    property var argument:({})
    property var pageRegister
    signal initArgument(var argument)

    id:window
    background: Rectangle{
        color: {
            if(active){
                return FluTheme.dark ? Qt.rgba(32/255,32/255,32/255,1) : Qt.rgba(238/255,244/255,249/255,1)
            }
            return FluTheme.dark ? Qt.rgba(32/255,32/255,32/255,1) : Qt.rgba(243/255,243/255,243/255,1)
        }
        Behavior on color{
            ColorAnimation {
                duration: 300
            }
        }
    }

    Item{
        id:container
        anchors.fill: parent
        anchors.margins: window.visibility === Window.Maximized && FluTheme.frameless ? 8/Screen.devicePixelRatio : 0
        clip: true
    }

    onActiveChanged: {
        if(active){
            helper.firstUpdate()
        }
    }

    onClosing:
        (event)=>{
            //销毁窗口，释放资源
            helper.destoryWindow()
        }

    FluInfoBar{
        id:infoBar
        root: window
    }

    WindowHelper{
        id:helper
    }

    Component.onCompleted: {
        helper.initWindow(window)
        initArgument(argument)
        window.x = (Screen.width - window.width)/2
        window.y = (Screen.desktopAvailableHeight - window.height)/2
    }

    function showSuccess(text,duration,moremsg){
        infoBar.showSuccess(text,duration,moremsg);
    }

    function showInfo(text,duration,moremsg){
        infoBar.showInfo(text,duration,moremsg);
    }

    function showWarning(text,duration,moremsg){
        infoBar.showWarning(text,duration,moremsg);
    }

    function showError(text,duration,moremsg){
        infoBar.showError(text,duration,moremsg);
    }

    function registerForPageResult(path){
        return helper.createRegister(window, path)
    }

    function onResult(data){
        if(pageRegister){
            pageRegister.onResult(data)
        }
    }

}

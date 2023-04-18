import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import FluentUI 1.0


FluScrollablePage{

    title:"视频播放器"
    leftPadding:10
    rightPadding:10
    bottomPadding:20

    onVisibleChanged: {
        if(visible){
            player.play()
        }else{
            player.pause()
        }
    }

    FluArea{
        Layout.fillWidth: true
        height: 320
        Layout.topMargin: 20
        paddings: 10
        ColumnLayout{
            anchors{
                verticalCenter: parent.verticalCenter
                left:parent.left
            }
            FluMediaPlayer{
                id:player
                source:"http://120.78.82.230:80/test.flv"
            }
        }
    }

    FluArea{
        Layout.fillWidth: true
        height: 68
        Layout.topMargin: 20
        paddings: 10

        FluButton{
            text:"跳转到视频播放器窗口"
            anchors.verticalCenter: parent.verticalCenter
            onClicked:{
                FluApp.navigate("/media",{source:"http://120.78.82.230:80/test.flv"})
            }
        }

    }

}


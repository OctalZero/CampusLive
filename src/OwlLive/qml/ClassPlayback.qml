import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import Controller 1.0

FluScrollablePage{
    title:"课堂回放"

    leftPadding:10
    rightPadding:0
    bottomPadding:20

    property var playbacks: []  // 课堂回放

    Component.onCompleted: {
        playback_controller.UpdatePlaybackList()
    }

    // 更新界面的回放列表
    function updateList() {
        for (var i = 0; i < playbacks.length; i++) {
            var playback = playbacks[i]
            addPlayback(playback["fileName"], playback["url"])
        }
    }

    PlaybackController {
        id: playback_controller
        onUpdatePlaybackSuccess: {
            playbacks = playback_controller.getPlaybacks()
            model_playback.clear()
            updateList()
        }
    }

    // 添加回放
    function addPlayback(fileName, url) {
        model_playback.append({"fileName": fileName, "url": url})
    }

    FluFilledButton {
        text: "测试"
        onClicked:  {
            addPlayback("file", "http://120.78.82.230:80/test.flv")
        }
    }

    Component {
        id: playback_card
        Item {
            id: playback_item
            width: parent.width
            height: 110
            FluArea {
                width: parent.width - 20
                height: 100

                ColumnLayout {
                    spacing: 17
                    anchors {
                        left: parent.left
                        top: parent.top
                        leftMargin: 20
                        topMargin: 10
                    }

                    FluText{
                        id: name
                        text: model.fileName
                        fontStyle: FluText.SubTitle
                    }

                    FluText{
                        text: model.url
                        fontStyle: FluText.SubTitle
                    }
                }

                FluFilledButton {
                    text:"观看回放"
                    anchors{
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        rightMargin: 20
                    }
                    onClicked: {
                        FluApp.navigate("/media",{source: model.url})
                    }

                }
            }
        }
    }

    ListModel {
        id: model_playback
    }

    ListView {
        id: model_list
        Layout.fillWidth: true
        implicitHeight: contentHeight
        model: model_playback
        interactive: false
        delegate: playback_card
    }
}

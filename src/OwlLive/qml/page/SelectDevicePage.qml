import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

FluWindow {
    id: window
    width: 350
    height: 500
    minimumWidth: 350
    minimumHeight: 500
    maximumWidth: 350
    maximumHeight: 500
    flags: Qt.WindowStaysOnTopHint

    title:"设备选择"

    property bool videoSelected: false
    property bool audioSelected: false
    property var videoDevices: []  // 视频设备
    property var audioDevices: []  // 音频设备
    property string videoName: ""
    property string audioName: ""

    onInitArgument:
        (argument)=>{
            videoDevices = argument.videoDevices
            audioDevices = argument.audioDevices
        }

    Component.onCompleted: {
        updateDevices()
    }

    // 更新设备列表
    function updateDevices() {
        var i, device
        for (i = 0; i < videoDevices.length; i++) {
            device = videoDevices[i]
            addVideo(device)
        }
        for (i = 0; i < audioDevices.length; i++) {
            device = audioDevices[i]
            addAudio(device)
        }
    }


    FluAppBar{
        id:appbar
        title:"设备选择"
        width:parent.width
    }

    // 添加视频设备
    function addVideo(name) {
        video_model.append({"name": name})
    }

    // 添加音频设备
    function addAudio(name) {
        audio_model.append({"name": name})
    }

    FluText {
        id: title
        text: "选择直播设备"
        fontStyle: FluText.Title
        anchors.top: appbar.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    FluFilledButton {
        id: sure
        text: "确认"
        anchors {
            bottom: parent.bottom
            bottomMargin: 20
            horizontalCenter: parent.horizontalCenter
        }
        onClicked: {
            if (videoSelected && audioSelected) {
                onResult({videoName:videoName, audioName: audioName})
                window.close()
            }
            else {
                showWarning("请将设备选择完整！")
                return
            }
        }
    }

    FluScrollablePage{
        leftPadding:10
        rightPadding:10
        topPadding:20

        anchors {
            left: parent.left
            top: title.bottom
            bottom: sure.top
            right: parent.right
        }

        FluText {
            text: "视频设备"
            fontStyle: FluText.SubTitle
            Layout.alignment: Qt.AlignHCenter
        }

        Component {
            id: video_device
            Item {
                width: parent.width
                height: 30
                FluFilledButton {
                    text: model.name
                    normalColor: "#CD5C5C"
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        videoName = text
                        showSuccess("视频设备:" + videoName)
                        videoSelected = true
                    }
                }
            }
        }

        ListModel {
            id: video_model
        }

        ListView {
            Layout.fillWidth: true
            implicitHeight: contentHeight
            model: video_model
            interactive: false
            delegate: video_device
        }

        FluText {
            text: "音频设备"
            fontStyle: FluText.SubTitle
            Layout.alignment: Qt.AlignHCenter
        }

        Component {
            id: audio_device
            Item {
                width: parent.width
                height: 30
                FluFilledButton {
                    text: model.name
                    anchors.horizontalCenter: parent.horizontalCenter
                    normalColor: "#CD5C5C"
                    onClicked: {
                        audioName = text
                        showSuccess("音频设备:" + audioName)
                        audioSelected = true
                    }
                }
            }
        }

        ListModel {
            id: audio_model
        }

        ListView {
            Layout.fillWidth: true
            implicitHeight: contentHeight
            model: audio_model
            interactive: false
            delegate: audio_device
        }
    }

}

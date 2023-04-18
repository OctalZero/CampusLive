﻿import QtQuick 2.15
import QtQuick.Controls  2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

Item{

    enum StatusMode  {
        Loading,
        Empty,
        Error,
        Success
    }
    default property alias content: container.data
    property int statusMode: FluStatusView.Loading
    signal errorClicked

    Item{
        id:container
        anchors.fill: parent
        visible: statusMode === FluStatusView.Success
    }

    FluArea{
        paddings: 0
        border.width: 0
        anchors.fill: container
        visible: opacity
        opacity: statusMode === FluStatusView.Loading
        Behavior on opacity {
            NumberAnimation  { duration: 150 }
        }
        ColumnLayout{
            anchors.centerIn: parent
            visible: statusMode === FluStatusView.Loading
            FluProgressRing{
                indeterminate: true
                Layout.alignment: Qt.AlignHCenter
            }
            FluText{
                text:"正在加载..."
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    FluArea{
        paddings: 0
        border.width: 0
        anchors.fill: container
        visible: opacity
        opacity: statusMode === FluStatusView.Empty
        Behavior on opacity {
            NumberAnimation  { duration: 150 }
        }
        ColumnLayout{
            anchors.centerIn: parent
            visible: statusMode === FluStatusView.Empty
            FluText{
                text:"空空如也"
                fontStyle: FluText.BodyStrong
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }

    FluArea{
        paddings: 0
        border.width: 0
        anchors.fill: container
        visible: opacity
        opacity: statusMode === FluStatusView.Error
        Behavior on opacity {
            NumberAnimation  { duration: 150 }
        }
        ColumnLayout{
            anchors.centerIn: parent
            FluText{
                text:"页面出错了..."
                fontStyle: FluText.BodyStrong
                Layout.alignment: Qt.AlignHCenter
            }
            FluFilledButton{
                id:btn_error
                Layout.alignment: Qt.AlignHCenter
                text:"重新加载"
                onClicked:{
                    errorClicked.call()
                }
            }
        }
    }


    function showSuccessView(){
        statusMode = FluStatusView.Success
    }
    function showLoadingView(){
        statusMode = FluStatusView.Loading
    }
    function showEmptyView(){
        statusMode = FluStatusView.Empty
    }
    function showErrorView(){
        statusMode = FluStatusView.Error
    }

}

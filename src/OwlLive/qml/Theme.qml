﻿import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0

FluScrollablePage{

    title:"主题"
    leftPadding:10
    rightPadding:10
    bottomPadding:20

    RowLayout{
        Layout.topMargin: 20
        Repeater{
            model: [FluColors.Yellow,FluColors.Orange,FluColors.Red,FluColors.Magenta,FluColors.Purple,FluColors.Blue,FluColors.Teal,FluColors.Green]
            delegate:  FluRectangle{
                width: 42
                height: 42
                radius: [4,4,4,4]
                color: mouse_item.containsMouse ? Qt.lighter(modelData.normal,1.1) : modelData.normal
                FluIcon {
                    anchors.centerIn: parent
                    iconSource: FluentIcons.AcceptMedium
                    iconSize: 15
                    visible: modelData === FluTheme.primaryColor
                    color: FluTheme.dark ? Qt.rgba(0,0,0,1) : Qt.rgba(1,1,1,1)
                }
                MouseArea{
                    id:mouse_item
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        FluTheme.primaryColor = modelData
                    }
                }
            }
        }
    }

    FluText{
        text:"原生文本渲染"
        Layout.topMargin: 20
    }
    FluToggleSwitch{
        selected: FluTheme.nativeText
        clickFunc:function(){
            FluTheme.nativeText = !FluTheme.nativeText
        }
    }
}

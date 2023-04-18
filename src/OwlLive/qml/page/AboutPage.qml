import QtQuick 2.15
import QtQuick.Controls  2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

FluWindow {

    id:window

    width: 500
    height: 600
    minimumWidth: 500
    minimumHeight: 600
    maximumWidth: 500
    maximumHeight: 600
    launchMode: FluWindow.SingleTask

    title:"关于"

    FluAppBar{
        id:appbar
        title:"关于"
        width:parent.width
    }

    ColumnLayout{
        anchors{
            top: appbar.bottom
            left: parent.left
            right: parent.right
        }

        RowLayout{
            Layout.topMargin: 20
            Layout.leftMargin: 15
            spacing: 14
            FluText{
                text:"OWL Live"
                fontStyle: FluText.Title
            }
            FluText{
                text:"v%1".arg(appInfo.version)
                fontStyle: FluText.Body
                Layout.alignment: Qt.AlignBottom
            }
        }

        RowLayout{
            spacing: 14
            Layout.topMargin: 20
            Layout.leftMargin: 15
            FluText{
                text:"介绍："
            }
            FluText{
                text:"本系统是一款远程教学系统。"
                Layout.alignment: Qt.AlignBottom
            }
        }

        RowLayout{
            spacing: 14
            Layout.topMargin: 20
            Layout.leftMargin: 15
            FluText{
                text:"GitHub："
            }
            FluTextButton{
                id:text_hublink
                text:"https://github.com/OctalZero/OwlLive"
                Layout.alignment: Qt.AlignBottom
                onClicked: {
                    Qt.openUrlExternally(text_hublink.text)
                }
            }
        }
    }
}

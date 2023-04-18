import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Controls  2.15
import FluentUI 1.0
import "qrc:///global/"

FluScrollablePage{

    title:"侧边栏"
    leftPadding:10
    rightPadding:10
    bottomPadding:20
    spacing: 0

    FluArea{
        Layout.fillWidth: true
        Layout.topMargin: 20
        height: 168
        paddings: 10

        ColumnLayout{
            spacing: 10
            anchors{
                verticalCenter: parent.verticalCenter
                left: parent.left
            }

            FluText{
                text:"侧边栏显示模式"
                fontStyle: FluText.BodyStrong
                Layout.bottomMargin: 4
            }

            Repeater{
                id:repeater
                model: [{title:"常在模式",mode:FluNavigationView.Open},{title:"迷你模式",mode:FluNavigationView.Compact},{title:"隐藏模式",mode:FluNavigationView.Minimal},{title:"自动模式",mode:FluNavigationView.Auto}]
                delegate:  FluRadioButton{
                    selected : MainEvent.displayMode===modelData.mode
                    text:modelData.title
                    onClicked:{
                       MainEvent.displayMode = modelData.mode
                    }
                }
            }
        }

    }
}

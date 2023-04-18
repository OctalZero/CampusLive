import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import Controller 1.0

FluWindow {

    id:window
    width: 300
    height: 300
    minimumWidth: 300
    minimumHeight: 300
    maximumWidth: 300
    maximumHeight: 300

    title:"缺勤名单"

    onInitArgument:
        (argument)=>{
             name.text = argument.absentMembersName
        }

    FluAppBar{
        id:appbar
        title:"缺勤名单"
        width:parent.width
    }

    ColumnLayout{
        anchors{
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        FluText {
            text: "缺勤学生名单"
            fontStyle: FluText.SubTitle
            Layout.alignment: Qt.AlignHCenter
        }

        FluMultilineTextBox{
            id: name
            Layout.topMargin: 20
            Layout.preferredWidth: 200
            text: "暂无缺勤成员"
            Layout.alignment: Qt.AlignHCenter
        }

        FluFilledButton{
            text:"确认"
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 30
            onClicked:{
                 window.close();
            }
        }

    }



}

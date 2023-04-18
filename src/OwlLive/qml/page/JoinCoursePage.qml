import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import Controller 1.0

FluWindow {

    id:window
    width: 400
    height: 400
    minimumWidth: 400
    minimumHeight: 400
    maximumWidth: 400
    maximumHeight: 400

    title:"加入课程"

    FluAppBar{
        id:appbar
        title:"加入课程"
        width:parent.width
    }

    ColumnLayout{
        anchors{
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }


        FluTextBox{
            id: id
            placeholderText: "请输入课程id"
            Layout.preferredWidth: 260
            Layout.alignment: Qt.AlignHCenter
        }

        FluFilledButton{
            text:"确认"
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 30
            onClicked:{
                if(id.text === ""){
                    showError("课程号不能为空")
                    return
                }
                onResult({courseId:id.text})
                window.close()
            }
        }

    }



}

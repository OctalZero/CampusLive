import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

FluWindow {
    id:window
    width: 400
    height: 400
    minimumWidth: 400
    minimumHeight: 400
    maximumWidth: 400
    maximumHeight: 400

    title:"创建课程"

    FluAppBar{
        id:appbar
        title:"创建课程"
        width:parent.width
    }

    ColumnLayout{
        anchors{
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }


        FluTextBox{
            id: name
            placeholderText: "请输入课程名"
            Layout.preferredWidth: 260
            Layout.alignment: Qt.AlignHCenter
        }

        FluFilledButton{
            text:"确认"
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 30
            onClicked:{
                if(name.text === ""){
                    showError("课程名不能为空")
                    return
                }
                onResult({courseName:name.text})
                window.close()
            }
        }

    }

}

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

    title:"注册"

    // 登录控制器
    LoginController {
        id: login_controller
        onRegisterSuccess: {
            window.close()
            FluApp.navigate("/login")
        }
    }

    FluAppBar{
        id:appbar
        title:"注册"
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
            placeholderText: "请输入姓名"
            Layout.preferredWidth: 260
            Layout.alignment: Qt.AlignHCenter
        }

        FluTextBox{
            id: password
            Layout.topMargin: 20
            Layout.preferredWidth: 260
            placeholderText: "请输入密码"
            echoMode:TextInput.Password
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout{
            Layout.preferredWidth: 260
            Layout.alignment: Qt.AlignHCenter

            FluRadioButton {
                id: student_button
                text: "学生"
                selected: true
                onClicked: {
                    teacher_button.selected = false
                    student_button.selected = true
                }
            }

            FluRadioButton {
                id: teacher_button
                text: "教师"
                selected: false
                onClicked: {
                    student_button.selected = false
                    teacher_button.selected = true
                }
            }

        }


        FluFilledButton{
            text:"注册"
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 30
            onClicked:{
                if(password.text === ""){
                    showError("密码不能为空")
                    return
                }
                if(name.text === ""){
                    showError("姓名不能为空")
                    return
                }
                if (student_button.selected) {
                    login_controller.SendRegisterInfo(name.text, password.text, "student");
                }
                else if (teacher_button.selected) {
                    login_controller.SendRegisterInfo(name.text, password.text, "teacher");
                }
            }
        }

    }



}

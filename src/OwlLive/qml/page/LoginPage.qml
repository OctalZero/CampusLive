import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls  2.15
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

    title:"登录"

    // 登录控制器
    LoginController {
        id: login_controller
        onLoginSuccess: {
            login_controller.setUserId(textbox_id.text)
            window.close()
            FluApp.navigate("/")
        }
    }


    FluAppBar{
        id:appbar
        title:"登录"
        width:parent.width
    }

    Image {
        id: logo
        source: "qrc:/assert/logo.png"
        width: 360
        height: 100
        y: 50
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ColumnLayout{
        anchors{
            top: logo.bottom
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }


        FluAutoSuggestBox{
            id:textbox_id
            Layout.topMargin: 20
            text: login_controller.getUserId()
            items:[{title:"1"},{title:"2"},{title:"3"},{title:"4"},{title:"5"}]
            placeholderText: "请输入账号"
            Layout.preferredWidth: 260
            Layout.alignment: Qt.AlignHCenter
        }

        FluTextBox{
            id:textbox_password
            Layout.topMargin: 20
            Layout.preferredWidth: 260
            placeholderText: "请输入密码"
            echoMode:TextInput.Password
            Layout.alignment: Qt.AlignHCenter
            focus: true
        }

        FluFilledButton{
            text:"登录"
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 20
            onClicked:{
                if(textbox_password.text === ""){
                    showError("密码不能为空")
                    return
                }
                if(textbox_id.text === ""){
                    showError("账号不能为空")
                    return
                }

                // 校验账号密码
                login_controller.SendLoginInfo(textbox_id.text, textbox_password.text);
            }
        }

    }

    FluTextButton{

        text:"注册"
        onClicked: {
            window.close()
            FluApp.navigate("/register")
        }
        anchors{
            bottom: parent.bottom
            right: parent.right
            bottomMargin: 20
            rightMargin: 20
        }
    }

}

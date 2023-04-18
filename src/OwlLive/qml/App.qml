import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls  2.15
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import FluentUI 1.0

Window {
    id:app
    color: "#00000000"
    //初始化一个MediaPlayer，解决切换到MediaPalyer页面崩溃问题
    MediaPlayer{}
    Component.onCompleted: {
        FluApp.init(app)
        FluTheme.frameless = ("windows" === Qt.platform.os)
        FluTheme.dark = false
        FluApp.routes = {
            "/":"qrc:/page/MainPage.qml",
            "/about":"qrc:/page/AboutPage.qml",
            "/login":"qrc:/page/LoginPage.qml",
            "/register":"qrc:/page/RegisterPage.qml",
            "/createCourse":"qrc:/page/CreateCoursePage.qml",
            "/joinCourse":"qrc:/page/JoinCoursePage.qml",
            "/attendance":"qrc:/page/AttendancePage.qml",
            "/absent":"qrc:/page/AbsentPage.qml",
            "/discuss":"qrc:/page/DiscussPage.qml",
            "/chat":"qrc:/page/ChatPage.qml",
            "/live":"qrc:/page/LivePage.qml",
            "/media":"qrc:/page/MediaPage.qml"
        }
        FluApp.initialRoute = "/login"
        FluApp.run()
    }

}

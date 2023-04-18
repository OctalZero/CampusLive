import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15
import FluentUI 1.0
import Controller 1.0

// 直播窗口
FluWindow {
    id: window
    title: "课堂直播"
    width: 1600
    height: 900
    minimumWidth: 1600
    minimumHeight: 900
    maximumWidth: 1600
    maximumHeight: 900


    property var attendancePageRegister: registerForPageResult("/attendance")
    property var members: []  // 课堂成员列表
    property var absentMembers: []  // 缺席学生列表
    property string courseId: ""  // 课程id
    property string streamUrl: ""  // 课程流地址
    property bool showControl: false  // 功能控制栏的显示

    onInitArgument:
        (argument)=>{
            courseId = argument.courseId
            streamUrl = argument.streamUrl
        }

    // 从考勤签到界面获取返回数据
    Connections{
        target: attendancePageRegister
        function onResult(data)
        {
            live_controller.Attendance(courseId, data.time)
        }
    }

    // 合并缺勤学生名字
    function mergeAbsentMembersName() {
        var members_name = "";
        for (var i = 0; i < absentMembers.length; i++) {
           var member = absentMembers[i]
           members_name += member["name"] + " "
        }
        return members_name
    }

    // 更新界面的成员列表
    function updateList() {
        for (var i = 0; i < members.length; i++) {
            var member = members[i]
            addMember(member["identification"], member["name"])
        }
    }

    // 添加成员
    function addMember(identity, name) {
        model_member.append({"identity": identity, "name": name})
    }

    // 窗口关闭
    Component.onDestruction: {
        live_controller.ExitClass(courseId)
        if(timer.running) timer.running = flase;
    }

    Component.onCompleted: {
//        live_controller.Broadcast(streamUrl)
//        live_controller.Broadcast("rtmp://114.215.169.66/live/livestream")
        live_controller.Broadcast("rtmp://127.0.0.1/live/livestream")
//        live_controller.Broadcast("rtmp://120.78.82.230:1935/test/s")
        live_controller.UpdateClassList(courseId)
    }

    // 窗口栏
    FluAppBar{
        id: appbar
        title: "课堂直播"
        width:parent.width
    }


    // 直播窗口
    Rectangle {
        id: live_out
        color: FluColors.Black
        clip: true
        anchors{
            left: parent.left
            top: appbar.bottom
            bottom: parent.bottom
        }
        width: parent.width - 200
        // 显示或隐藏功能控制栏
        MouseArea{
            anchors.fill: parent
            onClicked: {
                showControl = !showControl
            }
        }
        // 直播控制器
        LiveController {
            anchors.fill: parent
            id: live_controller
            width: parent.width - 200
            height: parent.height

            onUpdateClassMembersSuccess: {
                members = live_controller.getClassMembers()  // 存储成员列表
                model_member.clear() // 清空原来的成员
                updateList()
            }

            onUpdateAudioCache: {
                audio_cache.text = getAudioCache() + "ms"
            }

            onUpdateVideoCache: {
                video_cache.text = getVideoCache() + "ms"
            }

            onShowAttendanceResult: {
                absentMembers = live_controller.getAbsentMembers()
                FluApp.navigate("/absent", {absentMembersName: mergeAbsentMembersName()})
            }

            onConfirmAttendance: {
                dialog.open()
            }
        }

        // 功能控制栏
        Item{
            height: 60
            y:showControl ? parent.height - 70 : parent.height
            anchors{
                left: parent.left
                right: parent.right
                leftMargin: 10
                rightMargin: 10
            }

            MouseArea{
                anchors.fill: parent
            }

            Behavior on y{
                NumberAnimation{
                    duration: 150
                }
            }

            Rectangle{
                anchors.fill: parent
                color:FluTheme.dark ? Qt.rgba(45/255,45/255,45/255,0.97) : Qt.rgba(237/255,237/255,237/255,0.97)
                radius: 5
            }

            Row{
                spacing: 10
                anchors{
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 10
                }

                Column {
                    spacing: 2

                    FluText{
                        text:"音频缓存"
                        fontStyle: FluText.BodyStrong
                    }

                    FluText{
                        id: audio_cache
                        text:"0ms"
                    }
                }

                Column {
                    spacing: 2

                    FluText{
                        text:"视频缓存"
                        fontStyle: FluText.BodyStrong
                    }

                    FluText{
                        id: video_cache
                        text:"0ms"
                    }
                }
            }

            Row{
                spacing: 10
                anchors{
                    verticalCenter: parent.verticalCenter
                    horizontalCenter: parent.horizontalCenter
                }

                FluFilledButton {
                    id: check_in
                    text: "发起签到"
                    onClicked: {
                        attendancePageRegister.launch()
                    }
                }

                FluFilledButton {
                    id: discuss
                    text: "课堂讨论"
                    onClicked: {
                        FluApp.navigate("/discuss", {courseId:courseId})
                    }
                }

                FluDropDownButton{
                    id: cache_time
                    text:"缓存时间"
                    items:[
                        FluMenuItem{
                            text:"30ms"
                            onClicked: {
                                live_controller.setMaxCache(cache_time.text)
                            }
                        },
                        FluMenuItem{
                            text:"100ms"
                            onClicked: {
                                live_controller.setMaxCache(cache_time.text)
                            }
                        },
                        FluMenuItem{
                            text:"200ms"
                            onClicked: {
                                live_controller.setMaxCache(cache_time.text)
                            }
                        },
                        FluMenuItem{
                            text:"400ms"
                            onClicked: {
                                live_controller.setMaxCache(cache_time.text)
                            }
                        },
                        FluMenuItem{
                            text:"600ms"
                            onClicked: {
                                live_controller.setMaxCache(cache_time.text)
                            }
                        },
                        FluMenuItem{
                            text:"800ms"
                            onClicked: {
                                live_controller.setMaxCache(cache_time.text)
                            }
                        },
                        FluMenuItem{
                            text:"1000ms"
                            onClicked: {
                                live_controller.setMaxCache(cache_time.text)
                            }
                        },
                        FluMenuItem{
                            text:"2000ms"
                            onClicked: {
                                live_controller.setMaxCache(cache_time.text)
                            }
                        },
                        FluMenuItem{
                            text:"4000ms"
                            onClicked: {
                                live_controller.setMaxCache(cache_time.text)
                            }
                        }
                    ]
                }


                FluDropDownButton{
                    id: jitter
                    text:"抖动值"
                    items:[
                        FluMenuItem{
                            text:"30ms"
                            onClicked: {
                                live_controller.setJitter(jitter.text)
                            }
                        },
                        FluMenuItem{
                            text:"100ms"
                            onClicked: {
                                live_controller.setJitter(jitter.text)
                            }
                        },
                        FluMenuItem{
                            text:"200ms"
                            onClicked: {
                                live_controller.setJitter(jitter.text)
                            }
                        },
                        FluMenuItem{
                            text:"400ms"
                            onClicked: {
                                live_controller.setJitter(jitter.text)
                            }
                        },
                        FluMenuItem{
                            text:"600ms"
                            onClicked: {
                                live_controller.setJitter(jitter.text)
                            }
                        },
                        FluMenuItem{
                            text:"800ms"
                            onClicked: {
                                live_controller.setJitter(jitter.text)
                            }
                        },
                        FluMenuItem{
                            text:"1000ms"
                            onClicked: {
                                live_controller.setJitter(jitter.text)
                            }
                        }
                    ]
                }

            }


        }

    }



    // 定时器刷新界面
    Timer {
        id : timer
        interval: 1
        running: true
        repeat: true
        onTriggered: live_controller.update()
    }


//    FluFilledButton {
//        id: test
//        text: "开始直播"
//        onClicked: {
//          live_controller.broadcast();
//        }
//        anchors{
//            right: parent.right
//            top: appbar.bottom
//        }
//    }

    FluText {
        id: memberlist_text
        text: "课堂成员"
        fontStyle: FluText.Title
        anchors {
            left: live_out.right
            top: appbar.bottom
            leftMargin: 50
        }

    }

    FluScrollablePage{
        leftPadding:10
        rightPadding:10
        bottomPadding:10
        anchors {
            left: live_out.right
            top: memberlist_text.bottom
            bottom: parent.bottom
            right: parent.right
        }

        Component {
            id: member_card
            Item {
                id: course_item
                width: 200
                height: 50
                FluArea {
                    width: 200
                    height: 50
                    RowLayout {
                        anchors {
                            left: parent.left
                            top: parent.top
                            right: parent.right
                            topMargin: 13
                            leftMargin: 30
                        }

                        FluText{
                            id: id
                            text: model.identity
                            fontStyle: FluText.BodyStrong
                        }

                        FluText{
                            text: model.name
                            fontStyle: FluText.SubTitle
                        }
                    }
                }
            }
        }

        ListModel {
            id: model_member
        }

        ListView {
            id: model_list
            Layout.fillWidth: true
            implicitHeight: contentHeight
            model: model_member
            interactive: false
            delegate: member_card
        }
    }

    // 确认签到
    FluContentDialog{
        id:dialog
        title:"考勤签到"
        message:"请点击确认按钮进行签到"
        negativeText:"取消"
        onNegativeClicked:{
            showSuccess("取消签到")
        }
        positiveText:"确定"
        onPositiveClicked:{
            showSuccess("确认签到")
            live_controller.ConfirmAttendanceResult(courseId)
        }
    }

}

import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

FluWindow {
    id: window
    width: 300
    height: 300
    minimumWidth: 300
    minimumHeight: 300
    maximumWidth: 300
    maximumHeight: 300

    title:"考勤签到"

    FluAppBar{
        id:appbar
        title:"考勤签到"
        width:parent.width
    }

    ColumnLayout{
        anchors{
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        FluText {
            text: "签到时长设置"
            fontStyle: FluText.SubTitle
            Layout.alignment: Qt.AlignHCenter
        }

        Row {
            spacing: 10
            Layout.topMargin: 20
            FluRadioButton{
                id: radio_1
                text: "60s"
                selected: true
                onClicked: {
                    radio_1.selected = true;
                    radio_2.selected = false;
                    radio_3.selected = false;
                }
            }
            FluRadioButton{
                id: radio_2
                text: "90s"
                onClicked: {
                    radio_2.selected = true;
                    radio_1.selected = false;
                    radio_3.selected = false;
                }
            }
            FluRadioButton{
                id: radio_3
                text: "120s"
                onClicked: {
                    radio_3.selected = true;
                    radio_1.selected = false;
                    radio_2.selected = false;
                }
            }
            Layout.alignment: Qt.AlignHCenter
        }


        FluFilledButton{
            text:"确认"
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 30
            onClicked:{
                if (radio_1.selected) {
                    onResult({time:5})
                }
                else if (radio_2.selected) {
                    onResult({time:90})
                }
                else if(radio_3.selected) {
                    onResult({time:120})
                }
                window.close()
            }
        }

    }



}

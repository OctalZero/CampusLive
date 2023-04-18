pragma Singleton

import QtQuick 2.15
import FluentUI 1.0

FluObject{
    id:footer_items

    property var navigationView

    FluPaneItemSeparator{}

    FluPaneItem{
        title:"关于"
        icon: FluentIcons.ToolTip
        tapFunc:function(){
            FluApp.navigate("/about")
        }
    }

    FluPaneItem{
        title:"设置"
        icon:FluentIcons.Settings
        onTap:{
            navigationView.push("qrc:/Settings.qml")
        }
    }

    FluPaneItem{
        icon: FluentIcons.LockFeedback
        title:"意见反馈"
        onTap:{
            Qt.openUrlExternally("https://github.com/OctalZero/OwlLive/issues/new")
        }
    }
}

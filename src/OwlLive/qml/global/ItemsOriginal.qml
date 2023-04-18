pragma Singleton

import QtQuick 2.15
import FluentUI 1.0

FluObject{

    property var navigationView

    FluPaneItem{
        title:"Home"
        cusIcon: Image{
            anchors.centerIn: parent
            source: FluTheme.dark ? "qrc:/res/svg/home_dark.svg" : "qrc:/res/svg/home.svg"
            sourceSize: Qt.size(30,30)
            width: 18
            height: 18
        }
        onTap:{
            navigationView.push("qrc:/Home.qml")
        }
    }

//    FluPaneItem {
//        title:"live测试"
//        icon:FluentIcons.Home
//        onTap:{
//            FluApp.navigate("/live")
//        }
//    }


    FluPaneItemExpander{
        title:"课程管理"
        icon:FluentIcons.Dictionary

        FluPaneItem{
            title:"教师课程"
            image:"qrc:/res/image/control/Teacher.png"
            recentlyAdded:true
            desc:"支持教师开设课程和结课"
            onTap:{
                navigationView.push("qrc:/CourseInfoTeacher.qml")
            }
        }

        FluPaneItem{
            title:"学生课程"
            image:"qrc:/res/image/control/Student.png"
            recentlyAdded:true
            desc:"支持学生加入课程和退课"
            onTap:{
                navigationView.push("qrc:/CourseInfoStudent.qml")
            }
        }
    }

    FluPaneItemExpander{
        title:"学习空间"
        icon:FluentIcons.MapLayers
        FluPaneItem{
            title:"课堂回放"
            image:"qrc:/res/image/control/MediaPlayerElement.png"
            recentlyAdded:true
            desc:"支持课堂录像文件的回放"
            onTap:{
                navigationView.push("qrc:/ClassPlayback.qml")
            }
        }

    }


    FluPaneItemExpander{
        title:"辅助教学"
        icon:FluentIcons.MyNetwork
        FluPaneItem{
            title:"AI机器人"
            image:"qrc:/res/image/control/AI.png"
            recentlyAdded:true
            desc:"支持与AI对话答疑"
            onTap:{
                FluApp.navigate("/chat")
            }
        }
    }

    FluPaneItem{
        title:"主题"
        icon:FluentIcons.Brightness
        image:"qrc:/res/image/control/Theme.png"
        recentlyAdded:true
        desc:"支持多种主题选择"
        onTap:{
            navigationView.push("qrc:/Theme.qml")
        }
    }

    function getRecentlyAddedData(){
        var arr = []
        for(var i=0;i<children.length;i++){
            var item = children[i]
            if(item instanceof FluPaneItem && item.recentlyAdded){
                arr.push(item)
            }
            if(item instanceof FluPaneItemExpander){
                for(var j=0;j<item.children.length;j++){
                    var itemChild = item.children[j]
                    if(itemChild instanceof FluPaneItem && itemChild.recentlyAdded){
                        arr.push(itemChild)
                    }
                }
            }
        }
        arr.sort(function(o1,o2){ return o2.order-o1.order })
        return arr
    }

    function getRecentlyUpdatedData(){
        var arr = []
        var items = navigationView.getItems();
        for(var i=0;i<items.length;i++){
            var item = items[i]
            if(item instanceof FluPaneItem && item.recentlyUpdated){
                arr.push(item)
            }
        }
        return arr
    }

    function getSearchData(){
        var arr = []
        var items = navigationView.getItems();
        for(var i=0;i<items.length;i++){
            var item = items[i]
            if(item instanceof FluPaneItem){
                arr.push({title:item.title,key:item.key})
            }
        }
        return arr
    }

    function startPageByItem(data){
        navigationView.startPageByItem(data)
    }

}

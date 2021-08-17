import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.1
import obc.qml.Controls 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.0

Page{
    title: qsTr("Room Center")

    // Rectangle{
    //     //anchors.fill: parent
    //     height: 20
    //     color: "gray"
    // }


    Component{
        id: btnStyle;
        ButtonStyle {
            background: Rectangle {
                implicitWidth: 70;
                implicitHeight: 28;
                border.width: control.hovered ? 2 : 1;
                border.color: "#888";
                radius: 4;
                gradient: Gradient {
                    GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
                    GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                }
            }

            label: Text {
                text: control.text;
                font.pointSize: 12;
                color: "blue";
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignVCenter;
            }
            
        }
    }

    RoomService{
        id: roomService
        Component.onCompleted: {
            for(var i = 0; i < roomService.roomItems.length; ++i) {
                console.log("item : " + roomService.roomItems[i].roomName)
            }
        }

        onStartPaintReceived: {
            console.log("start paint")

            //页面跳转过去 这里的行为可能需要封装一下
            load_page("PaintingBoard.qml")
        }
    }

    RowLayout{
        id: roomHall
        visible: true
        anchors.fill: parent
        spacing: 6

        Rectangle{
            Layout.fillWidth: true
            //Layout.implicitHeight: parent.height
            Layout.preferredHeight: parent.height
            Layout.margins: 4
            color: "blue"
            Button{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom

                style: btnStyle


                text: qsTr("Create")
                onClicked: {
                    
                    roomService.createRoom();
                    roomDetail.state = '1'
                }
            }
            Component.onCompleted: console.log(height)
        }


        Rectangle{
            id: roomCenter
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height
            Layout.margins: 4

            property int currentPort
            property string currentIp

            color: "gray"

            Rectangle {
                anchors.centerIn: parent
                anchors.horizontalCenter: parent.horizontalCenter
                width: 180
                height: 400
                color: "transparent"

                Component {
                    id: contactDelegate
                    Item {
                        width: 180; height: 40
                        Column {
                            Text { text: '<b>' + roomName + '</b> ' }
                            Text { text: '<b>IP:</b> ' + address }
                            Text { text: '<b>Port:</b> ' + port }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log("clicked : " + index)
                                list.currentIndex = index
                                roomCenter.currentPort = roomService.roomItems[index].port
                                roomCenter.currentIp = roomService.roomItems[index].address

                                console.log(roomCenter.currentIp, roomCenter.currentPort)
                            }
                        }

                    }
                }

                ListView {
                    id: list
                    anchors.fill: parent
                    model: roomService.roomItems
                    delegate: contactDelegate
                    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                    highlightFollowsCurrentItem: true
                    focus: true

                    onCurrentItemChanged: console.log(list.currentIndex + ' selected')

                }
            }


            Button{
                //anchors.centerIn: parent
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                text: qsTr("Join")
                onClicked: {
                    roomCenter.state  = 'joining'
                    roomService.joinRoom(roomCenter.currentIp, roomCenter.currentPort)
                }
            }


            states: [
                State {
                    name: "joining"
                    PropertyChanges { target: roomCenter; color: "purple"; enabled: false }
                },
                State {
                    name: "joined"
                    PropertyChanges { target: roomCenter; color: "green";  enabled: false }
                }
            ]
        }
    }


    Rectangle{

        id: roomDetail
        anchors.fill: parent
        //visible: true
        state:'0'

        states: [
            State { name: '1';
                PropertyChanges {   target: roomDetail; opacity: 1.0    }
            },
            State { name: '0';
                PropertyChanges {   target: roomDetail; opacity: 0.0    }
            }
        ]
        transitions: Transition {
            NumberAnimation { property: "opacity"; duration: 250}
        }

        ColumnLayout {
            //columns: 3
            id: usersLayout
            anchors.top: parent.top
            height: parent.height - 100
            width: parent.width
            //Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            //visible: true
            spacing: 5
            opacity: 0.5
            state:'1'

            Repeater{
                model: roomService.userItems
                Rectangle{
                    Layout.fillWidth: true
                    //Layout.implicitHeight: parent.height
                    Layout.preferredHeight: parent.height / 5 - 5
                    //Layout.margins: 4
                    color: "yellow"
                    Button{
                        anchors.centerIn: parent
                        text: qsTr("User:" + modelData.userName)
                        onClicked: {
                            //roomService.createRoom();
                            //roomDetail.visible = false
                        }
                    }
                    Component.onCompleted: console.log(height)
                }
            }

        }

        Rectangle{
            anchors.top: usersLayout.bottom
            width: parent.width
            height: 100
            color: "gray"
            RowLayout{
                Text{
                    text: "Hello world"
                }
                Button{
                    text: "Start"
                    style: btnStyle
                    onClicked: {
                        roomService.startPaint()
                        console.log("start paint point 2")
                        for(var i = 0; i < roomService.userItems.length; ++i) {
                            console.log(roomService.userItems[i].userName)
                        }
                        load_page("PaintingBoard.qml")
                    }
                }
            }

        }
    }
}

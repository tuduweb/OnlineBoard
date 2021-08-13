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

    RoomService{
        id: roomService
        Component.onCompleted: {
            for(var i = 0; i < roomService.roomItems.length; ++i) {
                console.log("item : " + roomService.roomItems[i].roomName)
            }
        }
    }

    RowLayout{
        anchors.fill: parent
        spacing: 6

        Rectangle{
            Layout.fillWidth: true
            //Layout.implicitHeight: parent.height
            Layout.preferredHeight: parent.height
            Layout.margins: 4
            color: "blue"
            Button{
                anchors.centerIn: parent
                text: qsTr("Create")
                onClicked: {
                    roomService.createRoom();
                }
            }
            Component.onCompleted: console.log(height)
        }
        

        Rectangle{
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height
            Layout.margins: 4

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
                            Text { text: '<b>IP:</b> ' + address }
                            Text { text: '<b>Port:</b> ' + roomName }
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log("clicked : " + index)
                                list.currentIndex = index
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
            }
        }

    }

}
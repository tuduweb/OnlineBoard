import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.1
import an.qml.Controls 1.0
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
                            Text { text: '<b>IP:</b> ' + ip }
                            Text { text: '<b>Port:</b> ' + port }
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
                    model: ListModel {
                            ListElement {
                                ip: "192.168.123.243"
                                port: "4443"
                            }
                            ListElement {
                                ip: "192.168.123.30"
                                port: "12345"
                            }
                            ListElement {
                                ip: "192.168.123.123"
                                port: "22222"
                            }
                        }
                    delegate: contactDelegate
                    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                    highlightFollowsCurrentItem: true
                    focus: true

                    onCurrentItemChanged: console.log(model.get(list.currentIndex).ip + ' selected')
                    
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
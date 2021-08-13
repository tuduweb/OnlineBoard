import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: window
    width: 800
    height: 480
    visible: true
    title: qsTr("Stack")

    header: ToolBar {
        id: toolBar
        contentHeight: toolButton.implicitHeight

        ToolButton {
            id: toolButton
            text: stackView.depth > 1 ? "\u25C0" : "\u2630"
            font.pixelSize: Qt.application.font.pixelSize * 1.6
            onClicked: {
                if (stackView.depth > 1) {
                    stackView.pop()
                } else {
                    drawer.open()
                }
            }
        }

        Label {
            text: stackView.currentItem.title
            anchors.centerIn: parent
        }

        Label {
            height: parent.height
            anchors.right: parent.right

            //horizontalAlignment: Text.AlignHCenter//(1)
            verticalAlignment: Text.AlignVCenter//(2)

            text: stackView.depth > 1 ? "\u25C0" : "\u2630"
            font.pixelSize: Qt.application.font.pixelSize * 1.6

            color: "blue"

            Label {
                anchors.centerIn: parent
                text: "5"
                color: "white"
            }
        }

    }

    Drawer {
        id: drawer
        width: window.width * 0.66
        height: window.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Page 1")
                width: parent.width
                onClicked: {
                    stackView.push("Page1Form.ui.qml")
                    drawer.close()
                }
            }
            ItemDelegate {
                text: qsTr("index")
                width: parent.width
                onClicked: {
                    stackView.push("index.qml")
                    drawer.close()
                }
            }

            ItemDelegate {
                text: qsTr("PainterBoard")
                width: parent.width
                onClicked: {
                    stackView.push("PainterBoard.qml")
                    drawer.close()
                }
            }


        }
    }

    StackView {
        id: stackView
        initialItem: "RoomIndex.qml"
        //"qml/PainterItem.qml"
        //flickresize pinchEvent
        anchors.fill: parent
    }
}

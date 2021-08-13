import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.1
import an.qml.Controls 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

Page {
    title: "测试板"

    Rectangle {
        id: options;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        implicitHeight: 70;
        color: "lightgray"

        Component{
            id: markBtnStyle;
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
                    horizontalAlignment: Text.AlignRight;
                    verticalAlignment: Text.AlignVCenter;
                }
            }
        }

        RowLayout{
            Repeater {
                model: painter.getMarks()
                Button {
                    id: btn
                    width: 70;
                    height: 28;
                    text: "btn" + index
                    style: markBtnStyle
                    
                    Image{
                        anchors.verticalCenter: parent.verticalCenter
                        source: modelData
                        width: 24; height: 24
                    }

                    onClicked: {
                        console.log(modelData)
                    }
                    Component.onCompleted: {
                        //console.log(modelData)
                    }
                }
            }
            Component.onCompleted: {
                var marks = painter.getMarks()
                for (var i = 0; i < marks.length; i++) {
                    console.log(marks[i])
                }
            }
        }


    }

    APaintedItem {
        id: painter;
        anchors.top: options.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
    }

}

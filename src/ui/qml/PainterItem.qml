import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Window 2.1
import an.qml.Controls 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.0

Page {
    title: "画板"
    visible: true;

    Rectangle {
        id: options;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        implicitHeight: 70;
        color: "lightgray";
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
        ColorPicker {
            id: background;
            anchors.left: parent.left;
            anchors.leftMargin: 4;
            anchors.verticalCenter: parent.verticalCenter;
            text: "背景";
            selectedColor: "white";
            onColorPicked: painter.fillColor = clr;
        }
        ColorPicker {
            id: foreground;
            anchors.left: background.right;
            anchors.top: background.top;
            anchors.leftMargin: 4;
            text: "前景";
            selectedColor: "black";
            onColorPicked: painter.penColor = clr;
        }
        Rectangle {
            id: splitter;
            border.width: 1;
            border.color: "gray";
            anchors.left: foreground.right;
            anchors.leftMargin: 4;
            anchors.top: foreground.top;
            width: 3;
            height: foreground.height;
        }
        Slider {
            id: thickness;
            anchors.left: splitter.right;
            anchors.leftMargin: 4;
            anchors.bottom: splitter.bottom;
            minimumValue: 1;
            maximumValue: 100;
            stepSize: 1.0;
            value: 1;
            width: 280;
            height: 24;
            onValueChanged: if(painter != null)painter.penWidth = value;
        }

        Text {
            id: penThickLabel;
            anchors.horizontalCenter: thickness.horizontalCenter;
            anchors.bottom: thickness.top;
            anchors.bottomMargin: 4;
            text: "画笔:%1px".arg(thickness.value);
            font.pointSize: 16;
            color: "steelblue";
        }

        Text {
            id: minLabel;
            anchors.left: thickness.left;
            anchors.bottom: thickness.top;
            anchors.bottomMargin: 2;
            text: thickness.minimumValue;
            font.pointSize: 12;
        }

        Text {
            id: maxLabel;
            anchors.right: thickness.right;
            anchors.bottom: thickness.top;
            anchors.bottomMargin: 2;
            text: thickness.maximumValue;
            font.pointSize: 12;
        }

        Rectangle {
            id: splitter2;
            border.width: 1;
            border.color: "gray";
            anchors.left: thickness.right;
            anchors.leftMargin: 4;
            anchors.top: foreground.top;
            width: 3;
            height: foreground.height;
        }

        Column {
            anchors.left: splitter2.right;
            anchors.verticalCenter: parent.verticalCenter
            spacing: 2

            Row{

                Button {
                    id: clear;
                    //anchors.left: splitter2.right;
                    //anchors.leftMargin: 4;
                    //anchors.verticalCenter: splitter2.verticalCenter;
                    width: 70;
                    height: 28;
                    text: "清除";
                    style: btnStyle;
                    onClicked: painter.clear();
                }

                Button {
                    id: undo;
                    //anchors.left: clear.right;
                    //anchors.leftMargin: 4;
                    //anchors.top: clear.top;
                    width: 70;
                    height: 28;
                    text: "撤销";
                    style: btnStyle;
                    onClicked: painter.undo();
                }

                Button {
                    id: save;
                    //anchors.left: undo.right;
                    //anchors.leftMargin: 4;
                    //anchors.top: clear.top;
                    width: 70;
                    height: 28;
                    text: "保存";
                    style: btnStyle;
                    onClicked: {
                        painter.grabToImage(function(result) {
                            //painter.visible = false
                            var res2 = result.saveToFile(localUrl + "/savedImage233.jpg")
                            console.log("save res grab : " + res2)
                            //painter.visible = true
                        })

                        //painter.saveImage()
                        painter.saveImage(bgImage.source)
                    }
                }

                Button {
                    id: putSign
                    //anchors.left: save.right;
                    //anchors.leftMargin: 4;
                    //anchors.top: clear.top;
                    width: 70;
                    height: 28;
                    text: painter.paintMode === 1 ? "取消" : "标志"
                    //text: "标志"
                    style: btnStyle;
                    onClicked: {
                        painter.paintMode = 1 - painter.paintMode

                    }
                }
            }

            Row{
                Repeater {
                    model: painter.getMarks()
                    Button {
                        id: btn
                        width: 70;
                        height: 28;
                        text: "btn" + index
                        style: btnStyle
                        
                        Image{
                            anchors.verticalCenter: parent.verticalCenter
                            source: modelData
                            width: 24; height: 24
                        }

                        onClicked: {
                            console.log(modelData)
                            painter.markId = index
                        }
                    }
                }
                // Component.onCompleted: {
                //     var marks = painter.getMarks()
                //     for (var i = 0; i < marks.length; i++) {
                //         console.log(marks[i])
                //     }
                // }

                FileDialog {
                    id: fileDialog
                    title: "Please choose a file"
                    folder: shortcuts.home
                    onAccepted: {
                        console.log("You chose: " + fileDialog.fileUrls)
                        bgImage.source = fileDialog.fileUrl
                        //Qt.quit()
                    }
                    onRejected: {
                        console.log("Canceled")
                        //Qt.quit()
                    }
                    //Component.onCompleted: visible = true
                }

                Button {
                    id: fileBtn
                    width: 70;
                    height: 28;
                    text: "file"
                    style: btnStyle

                    onClicked: {
                        fileDialog.visible = true
                    }
                }

            }
        
        Component.onCompleted: {
            console.log("column height: " + height)
        }


        }
        // RowLayout{
        //     anchors.left: thickness.right;

        // }

        // RowLayout{
        //     anchors.left: thickness.right;
            
        // }


        Rectangle {
            border.width: 1;
            border.color: "gray";
            width: parent.width;
            height: 2;
            anchors.bottom: parent.bottom;
        }
    }

    Image {
        id: bgImage
        anchors.top: options.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        source: "qrc:/assets/marks/mark1.png"
    }

    APaintedItem {
        id: painter;
        anchors.top: options.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
    }

    Component.onCompleted: {
        console.log(painter.getMarks())
    }
}


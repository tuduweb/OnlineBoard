import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Page {

    title: qsTr("WhiteBoard")

    height: 400

    RowLayout {
        anchors.fill: parent

        Rectangle {
            id: sidetools
            color: "#ccc"
            width: 220
            Layout.fillHeight: true
            Layout.maximumHeight: parent.height

            ColumnLayout {
                anchors.fill: parent


                Row {
                    id: colorTools
//                    anchors {
//                        horizontalCenter: parent.horizontalCenter
//                        top: parent.top
//                        topMargin: 8
//                    }
                    property color paintColor: "#33B5E5"
                    spacing: 4
                    Repeater {
                        model: ["#33B5E5", "#99CC00", "#FFBB33", "#FF4444"]
                        ColorSquare {
                            recSize: 30
                            id: red
                            color: modelData
                            active: parent.paintColor === color
                            onClicked: {
                                parent.paintColor = color
                            }
                        }
                    }
                }

                Button{
                    id: saveBtn
                    text: "save"
                    onClicked: {
                        canvas.justPaint()
                        console.log(canvas.canvasSize)
                        console.log(canvas.toDataURL())
                        var res = canvas.save(localUrl + "/savedImage123.png")
                        console.log("save url : " + localUrl + "/savedImage123.png")
                        console.log("save res : " + res)
                        canvas.visible = false

                        canvas.grabToImage(function(result) {
                            var res2 = result.saveToFile(localUrl + "/savedImage245.png")
                            console.log("save res2 : " + res2)
                        })
                        canvas.visible = true
                    }
                }

                Button {
                    id: testBtn
                    text: "test"
                    onClicked: {
                        //canvas.justPaint()
                        console.log("size : " + mycanvas.canvasSize)
                        //console.log("size : " + canvas.canvasWindow)
                        var datas = mycanvas.toDataURL()
                        console.log(datas)
                    }
                }

                Image {
                    id: name
                    source: "file"
                }

                Text {
                    font.pixelSize: 24
                    text: "indexPicShow"
                }
                Repeater {
                    id: recRep
                    model: 1
                    Rectangle {
                        id: items
                        width: 200
                        height: 120

                        Text {
                            anchors.centerIn: parent
                            font.family: "Microsoft YaHei"
                            font.pixelSize: 24
                            text: index + 1
                        }
                    }
                }


            }
        }


        //////////////////////////Painter//////////////////////////////

        Rectangle {
            color: "#ccc"

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.maximumHeight: parent.height

            Canvas {
                property url imageUrl: "qrc:/icons/wrong.png"

                id: mycanvas
                width: 100
                height: 100
                onPaint: {
                    var ctx = getContext("2d");

                    ctx.fillStyle = Qt.rgba(1, 0, 1, 1);
                    ctx.fillRect(0, 0, width/2, height/2);

                    var temp = ctx.getImageData(0, 0, width, height)
                    console.log(temp.height)
                    ctx.drawImage(imageUrl, 0, 0, 30, 30)
                }

                onImageLoaded: {
                    requestPaint()
                }


            }

            Canvas {
                id: canvas

                anchors.fill: parent

                property real lastX
                property real lastY
                property color color: colorTools.paintColor

                property url imageUrl: "qrc:/icons/wrong.png"
                property url imageUrl2: "qrc:/icons/danger.png"


                property bool justPaintIt: false

                onPaint: {


                    if (!isImageLoaded(imageUrl))
                    {
                        //loadImage(imageUrl)
                    }

                    var ctx = getContext('2d')


                    //ctx.fillStyle = Qt.rgba(1, 1, 0, 1);
                    //ctx.fillRect(0, 0, width, height);

                    //ctx.drawImage(imageUrl, 10, 10, 30, 30)
                    //ctx.drawImage(imageUrl2, 10, 10, 50, 50)


                    ctx.lineWidth = 2.0
                    ctx.strokeStyle = canvas.color
                    ctx.beginPath()
                    ctx.moveTo(lastX, lastY)
                    lastX = area.mouseX
                    lastY = area.mouseY
                    ctx.lineTo(lastX, lastY)


                    ctx.stroke()

                    var otherCtx = mycanvas.getContext('2d')
                    ctx.drawImage(otherCtx.getImageData(0, 0, 100, 100), 100, 100, 200, 200)

                }

                onImageLoaded:
                {
                    requestPaint()  // 重绘Canvas
                }


                Component.onCompleted: {
                    //loadImage(imageUrl)
                    //loadImage(imageUrl2)
                }

                MouseArea {
                    id: area
                    anchors.fill: parent
                    onPressed: {
                        canvas.lastX = mouseX
                        canvas.lastY = mouseY

                        console.log(mouseX + " " + mouseY)

                    }
                    onPositionChanged: {
                        canvas.requestPaint()
                    }
                }

                function justPaint()
                {
                    justPaintIt = true
                    requestPaint()
                }

            }

        }



        ///////////////////////////////////////////////////////////////

//        Label {
//            text: qsTr("You are on the home page.")
//        }

//        Label {
//            text: qsTr("You are on the home page.")
        //            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        //        }
    }


}

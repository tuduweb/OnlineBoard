import QtQuick 2.15
import QtQuick.Controls 2.12

Page {
    title: "canvas"

    MouseArea{
        anchors.fill: parent
        property bool active: false
        onPressed: {
            active = true
            selectionArea.startPoint = Qt.point(mouseX,mouseY)
            selectionArea.x = selectionArea.startPoint.x
            selectionArea.y = selectionArea.startPoint.y
            selectionArea.width = 0
            selectionArea.height = 0
            selectionArea.visible = true
        }
        onPositionChanged: {
            if(!active)
                return
            selectionArea.width = Math.abs(mouseX-selectionArea.startPoint.x)
            selectionArea.height = Math.abs(mouseY-selectionArea.startPoint.y)
            selectionArea.visible = selectionArea.width>0 || selectionArea.height > 0
            //console.log(selectionArea.visible)
            selectionArea.x = Math.min(mouseX,selectionArea.startPoint.x)
            selectionArea.y = Math.min(mouseY,selectionArea.startPoint.y)
        }

        onReleased: {
            if(!selectionArea.visible)
                return
            if(selectionArea.width === 0){
                console.log("selectionArea.width === 0")


            }else{


            }

            selectionArea.visible = false
        }
    }

    Row {
        id: colorTools
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 8
        }
        property color paintColor: "#33B5E5"
        spacing: 4
        Repeater {
            model: ["#33B5E5", "#99CC00", "#FFBB33", "#FF4444"]
            ColorSquare {
                id: red
                color: modelData
                active: parent.paintColor == color
                onClicked: {
                    parent.paintColor = color
                }
            }
        }
    }

    // <<M1
    Rectangle {
        anchors.fill: canvas
        border.color: "#666666"
        border.width: 4
    }

    Row {
        id: topTools
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: colorTools.bottom
            topMargin: 8
        }
        property color paintColor: "#33B5E5"
        property int posBias: (parent.width - width) / 2
        spacing: 4
        Repeater {
            model: ["#33B5E5", "#99CC00", "#FFBB33", "#FF4444"]
            Rectangle {
                id: rec
                color: modelData
                width: 48; height: 48
                border.width: 2
                MouseArea{
                    anchors.fill: parent
//                    onClicked: {
//                        console.log("paint color : " + parent.color)
//                        canvas.drawRect(20, 30, 50, 60, parent.color)
//                    }

                    property bool active: false
                    onPressed: {
                        active = true
//                        selectionArea.startPoint = Qt.point(mouseX, mouseY)
//                        selectionArea.x = selectionArea.startPoint.x
//                        selectionArea.y = selectionArea.startPoint.y
//                        selectionArea.width = 0
//                        selectionArea.height = 0
//                        selectionArea.visible = true

                        floatItem.x = mouseX + parent.x + topTools.posBias
                        floatItem.y = mouseY + parent.y + 50
                        floatItem.visible = true

                    }
                    onPositionChanged: {
                        if(!active)
                            return
//                        selectionArea.width = Math.abs(mouseX-selectionArea.startPoint.x)
//                        selectionArea.height = Math.abs(mouseY-selectionArea.startPoint.y)
//                        selectionArea.visible = selectionArea.width>0 || selectionArea.height > 0
//                        //console.log(selectionArea.visible)
//                        selectionArea.x = Math.min(mouseX,selectionArea.startPoint.x)
//                        selectionArea.y = Math.min(mouseY,selectionArea.startPoint.y)

                        floatItem.x = mouseX + parent.x + topTools.posBias
                        floatItem.y = mouseY + parent.y + 50
                    }

                    onReleased: {
                        if(!floatItem.visible)
                            return
//                        if(!selectionArea.visible)
//                            return
//                        if(selectionArea.width === 0){
//                            console.log("selectionArea.width === 0")


//                        }else{


//                        }

                        //selectionArea.visible = false

                        floatItem.visible = false
                        canvas.drawRect(mouseX + parent.x + topTools.posBias, mouseY + parent.y + 50, 48, 48, parent.color)

                        //Note: Only loaded images can be painted on the Canvas item.
                        //canvas.loadImage('qrc:/icons/wrong.png')
                        //var url links =
                        canvas.drawImage(Qt.resolvedUrl('qrc:/icons/wrong.png') , mouseX + parent.x + topTools.posBias, mouseY + parent.y, 48, 48)
                        //canvas.(mouseX + parent.x + topTools.posBias, mouseY + parent.y, 48, 48, parent.color)

                    }


                }

                //active: parent.paintColor == color
            }
        }
    }


    // M2>>
    Canvas {
        id: canvas
        anchors {
            left: parent.left
            right: parent.right
            top: topTools.bottom
            bottom: parent.bottom
            margins: 8
        }
        property real lastX
        property real lastY
        property color color: colorTools.paintColor


        property url imageUrl: "qrc:/icons/wrong.png"
        property url imageUrl2: "qrc:/icons/danger.png"

        onPaint: {

            if (!isImageLoaded(imageUrl))
            {
                //loadImage(imageUrl)
            }

            var ctx = getContext('2d')

            ctx.drawImage(imageUrl, 200, 200, 100, 100)
            ctx.drawImage(imageUrl2, 200, 200, 50, 50)


            ctx.lineWidth = 2.0
            ctx.strokeStyle = canvas.color
            ctx.beginPath()
            ctx.moveTo(lastX, lastY)
            lastX = area.mouseX
            lastY = area.mouseY
            ctx.lineTo(lastX, lastY)


            ctx.stroke()
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

                //backend.send_message_test(mouseX + " " + mouseY)

            }
            onPositionChanged: {
                canvas.requestPaint()
            }
        }

        function drawRect(_x, _y, _width, _height, _color) {
            var ctx = getContext('2d')
            ctx.strokeStyle = parent.color
            ctx.rect(_x, _y, _width, _height, _color)
            ctx.stroke()
            canvas.requestPaint()
        }

        //drawImage的类型需要是url形
        function drawImage(_x, _y, _width, _height, _images) {

            var ctx = getContext('2d')
            //ctx.strokeStyle = parent.color
            ctx.drawImage( imageUrl2, _x, _y, _width, _height)
            //ctx.rect(_x, _y, _width, _height, _color)
            ctx.stroke()
            canvas.requestPaint()
        }

    }


    Rectangle {
        id: floatItem
        x: 200
        y: 200
        width: 48
        height: 48
        color: "blue"
    }



    Rectangle{
        id: selectionArea
        opacity: .2
        color: "black"
        property var startPoint: null
        visible: false

    }



}

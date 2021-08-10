import QtQuick 2.15

Rectangle {
    property int recSize: 20
    id: root
    width: recSize; height: recSize
    color: "green"
    signal clicked
    property bool active: false
    border.color: active? "#666666" : "#f0f0f0"
    border.width: 2

    MouseArea {
        id: area
        anchors.fill :parent
        onClicked: {
            root.clicked()
        }
    }
}

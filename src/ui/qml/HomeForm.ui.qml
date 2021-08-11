import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Page {
    id: page
    //width: 600
    //height: 400

    title: qsTr("WhiteBoard")

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
                Text {
                    font.pixelSize: 24
                    text: "indexPicShow"
                }
                Repeater {
                    id: recRep
                    model: 20
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

        Label {
            text: qsTr("You are on the home page.")
        }

        Label {
            text: qsTr("You are on the home page.")
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        }
    }
}

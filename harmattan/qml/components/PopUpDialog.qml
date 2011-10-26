import QtQuick 1.0
import com.meego 1.0


Rectangle {
    id: bottomPane
    height: 400
    width: parent.width
    state: "Hidden"

    property Item movingItem: undefined
    property alias content: contentLoader.sourceComponent

    Image {
        id: bg
        anchors.fill: parent
        source: "qrc:/images/startwindow_bg.png"
        fillMode: "TileVertically"
        MouseArea{
            id: mouseArea
            anchors.fill: parent
        }
    }

    Button {
        id: closeButton
        text: "Close"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
        onClicked: bottomPane.state = "Hidden"
        width: 150
        height: 50
    }

    Button {
        id: saveButton
        text: "Save"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        onClicked: bottomPane.state = "Hidden"
        width: 150
        height: 50
    }

    Loader {
        id: contentLoader
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: closeButton.bottom
        anchors.margins: 10
    }


    Image{
        id: topShadow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.top
        fillMode: Image.TileHorizontally
        source: "image://theme/meegotouch-menu-shadow-top"
    }



    states: [
        State {
            name: "Hidden"
            PropertyChanges {
                target: bottomPane
                y: parent.height
            }
            PropertyChanges{
                target: movingItem
                contentY: 0
                textSelectionEnabled: false

            }

        },
        State {
            name: "Visible"
            PropertyChanges {
                target: bottomPane
                y: bottomPane.parent.height - 400
            }
            PropertyChanges{
                target: movingItem
                contentY: -(bottomPane.parent.height - 400)
                textSelectionEnabled: true

            }
        }
    ]

    transitions: [
        Transition {
            from: "Hidden"
            to: "Visible"
            PropertyAnimation {
                target: bottomPane
                properties: "y"
                duration: 200
            }
        },
        Transition {
            from: "Visible"
            to: "Hidden"
            PropertyAnimation {
                target: bottomPane
                properties: "y"
                duration: 200
            }
        }
    ]



}

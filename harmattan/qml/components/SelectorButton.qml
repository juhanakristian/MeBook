import QtQuick 1.0
import com.meego 1.0

Rectangle {
    id: selectorButton
    anchors.left: parent.left
    anchors.right: parent.right
    height: 75
    color: mouseArea.pressed ? Qt.rgba(0, 0, 0, 0.3) : Qt.rgba(0, 0, 0, 0)
    anchors.topMargin: 30
    anchors.bottomMargin: 30

    property alias text: titleTextLabel.text
    property alias selectedText: selectedTextLabel.text
    signal clicked

    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        height: childrenRect.height
        anchors.verticalCenter: parent.verticalCenter

        Label{
            id: titleTextLabel
            anchors.left: parent.left
            font.bold: true
            anchors.topMargin: 20
            anchors.margins: 5
            anchors.leftMargin: 10
        }

        Label{
            id: selectedTextLabel
            anchors.left: parent.left
            font.bold: false
            font.pixelSize: 16
            color: "#dddddd"
            anchors.margins: 5
            anchors.leftMargin: 10
        }

    }

    ToolIcon{
        id: icon
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        iconSource: "image://theme/icon-m-common-drilldown-arrow-inverse"
    }



    MouseArea{
        id: mouseArea
        anchors.fill: parent
        onClicked: selectorButton.clicked()
    }
}

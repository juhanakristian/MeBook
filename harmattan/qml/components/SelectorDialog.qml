import QtQuick 1.0
import com.meego 1.0

Dialog {
    id:selectorDialog

    width: parent.width

    property alias titleText: titleLabel.text
    property alias listModel: listView.model

    property int selectedIndex: 0
    signal selected(int index)

    title: Item{
        height: 75
        width: parent.width
        Label{
            id: titleLabel
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Nokia Pure"
            font.pixelSize: 28
            width: parent.width
            anchors.left: parent.left
            anchors.leftMargin: 10

        }

        Rectangle {
            width: parent.width
            height: 2
            color: Qt.rgba(0.3, 0.3, 0.3, 1.0)
            y: parent.height - 10
        }
    }
    content:ListView {
        id: listView
        width: parent.width
        height: 400
        clip:true
        delegate: Rectangle {
            id: itemDelegate
            height: 75
            width: parent.width
            color: mouseArea.pressed ? Qt.rgba(0.6, 0.6, 0.6, 0.6) : Qt.rgba(0.3, 0.3, 0.3, 0.0)
            MouseArea{
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    selectedIndex = index;
                    selected(index);
                    selectorDialog.close();
                }
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10
                text: itemText
                font.pixelSize: 24
                font.bold: true
            }
        }
    }
}

import QtQuick 1.0
import com.meego 1.0

TextField{
    id: searchField
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.margins: 5
    placeholderText: "Search"

    signal search(string s)

    ToolIcon {
        anchors.right: parent.right
        anchors.rightMargin: -10
        anchors.verticalCenter: parent.verticalCenter
        iconSource: "image://theme/icon-m-common-search"
        onClicked: search(searchField.text)
    }

    Keys.onPressed: if(event.key == Qt.Key_Enter) search(searchField.text)
}

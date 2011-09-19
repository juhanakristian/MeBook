import QtQuick 1.0
import com.meego 1.0

TextField{
    id: searchField
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.margins: 5
    placeholderText: "Search"

    signal search(string s)

    SipAttributes{
        id: sipAttributes
        actionKeyLabel: "Search"
    }

    platformSipAttributes: sipAttributes

    Image {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        source: (searchField.text.toString() !== "") ? "image://theme/icon-m-input-clear" : "image://theme/icon-m-common-search"
        MouseArea{
            anchors.fill: parent
            onClicked: console.log("1:" + searchField.text + " 2: " + searchField.text.toString());

        }
    }

    Keys.onPressed: if(event.key == Qt.Key_Enter) search(searchField.text)
}

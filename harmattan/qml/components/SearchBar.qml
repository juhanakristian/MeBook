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
            onClicked: searchField.text = "";

        }
    }

    Keys.onPressed: {
        platformCloseSoftwareInputPanel();
        console.log(searchField.text);
        search(searchField.text);
    }

}

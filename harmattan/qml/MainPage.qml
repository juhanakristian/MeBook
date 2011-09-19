import QtQuick 1.1
import com.meego 1.0

import com.mebook.bookview 1.0
import com.mebook.settings 1.0

import "components"

Page {
    id: root
    tools: mainPageTools

    ToolBarLayout {
        id: mainPageTools
        visible: true

        ToolIcon {
            platformIconId: "toolbar-contact";
        }

        ToolIcon {
            platformIconId: "toolbar-settings";
            onClicked: pageStack.push(settingsPage)
        }

        ToolIcon {
            platformIconId: "toolbar-down";
            onClicked: pageStack.push(downloadBooksPage)
        }

        ToolIcon {
            platformIconId: "toolbar-add";
        }
    }



    Image {
        id: bg
        anchors.fill: parent
        source: "qrc:/images/startwindow_bg.png"
        fillMode: "TileVertically"
    }

    ListView{
        id: bookListView
        anchors.topMargin: 10
        anchors.bottomMargin: 10
        anchors.fill: parent
        model: books
        spacing: 10

        header: Column {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            height:  childrenRect.height + 20
            spacing: 5
            ButtonRow {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5
                Button{ id: authorButton; text: "Author"; onClicked: {books.sortByColumn(3); settings.setSorting(2);}}
                Button{ id: titleButton; text: "Title"; onClicked: {books.sortByColumn(2); settings.setSorting(3);}}
                Button{ id: dateButton; text: "Date"; onClicked: {books.sortByColumn(0); settings.setSorting(0);}}

                Component.onCompleted: {
                    var s = settings.sorting();
                    books.sortByColumn(s);
                    switch(s) {
                    case 3: authorButton.checked = true;
                        break;
                    case 2: titleButton.checked = true;
                        break;
                    case 0: dateButton.checked = true;
                        break;
                    }
                }
            }

        SearchBar{
            id: searchBar

        }


        }

        delegate: Rectangle {
            height: childrenRect.height
            border.color: "#000000"
            border.width: 1
            radius: 10
            color: Qt.rgba(0, 0, 0, 0.6)
            anchors.margins: 20
            anchors.left: parent.left
            anchors.right: parent.right

            MouseArea{
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    if(inputContext.softwareInputPanelVisible) {
                        inputContext.simulateSipClose();
                    } else {
                        readingPage.setBook(id);
                        pageStack.push(readingPage);
                    }
                }
            }

            Row{
                id: contentRow
                spacing: 5
                anchors.margins: 10
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: childrenRect.height + 20
                Image{
                    id: coverImage
                    width: 100
                    fillMode: Image.PreserveAspectFit
                    source: "image://covers/" + id
                }

                Column{
                    anchors.margins: 10
                    width: root.width - coverImage.width - 65
                    spacing: 5

                    Label{
                        id: nameLabel
                        text: title
                        width: parent.width
                        font.pixelSize: 22
                        wrapMode: Text.WordWrap
                    }

                    Label{
                        id: authorLabel
                        text: author
                        width: parent.width
                        font.pixelSize: 16
                        wrapMode: Text.WordWrap
                    }

                    Label{
                        id: categoryLabel
                        text: subject
                        width: parent.width
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }

                }
            }
        }

        ScrollDecorator{ flickableItem: bookListView }

    }

}

import QtQuick 1.0
import com.meego 1.0

import com.mebook.bookview 1.0
import com.mebook.settings 1.0

import "components"

Page {
    id: downloadBooksPage
    tools: downloadPageTools

    ToolBarLayout {
        id: downloadPageTools
        visible: true
        ToolIcon { platformIconId: "toolbar-back";
             anchors.left: parent===undefined ? undefined : parent.left
             onClicked: pageStack.pop()
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
        model: feedbooksModel
        spacing: 10
        header: Item{
            height: childrenRect.height + 20
            anchors.left: parent.left
            anchors.right: parent.right
            SearchBar{
                id: searchBar
                anchors.margins: 10
                onSearch: {console.log(s);feedbooks.searchBooks(s);}
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
//                onClicked: {
//                    readingPage.setBook(id);
//                    pageStack.push(readingPage);
//                }
            }

            Row{
                id: contentRow
                spacing: 5
                anchors.margins: 10
                anchors.left: parent.left
                anchors.top: parent.top
                height: childrenRect.height + 20
                Image{
                    id: coverImage
                    width: 100
                    fillMode: Image.PreserveAspectFit
                    source: cover_url
                }

                Column{
                    anchors.margins: 10
                    spacing: 5
                    Label{
                        id: nameLabel
                        text: title
                        font.pixelSize: 22
                        wrapMode: "WordWrap"
                    }

                    Label{
                        id: authorLabel
                        text: author
                        font.pixelSize: 16
                        wrapMode: "WordWrap"
                    }

                    Label{
                        id: categoryLabel
                        text: subjects
                        font.pixelSize: 12
                        wrapMode: "WordWrap"
                    }

                }
            }
        }

        ScrollDecorator{ flickableItem: bookListView }

    }
}

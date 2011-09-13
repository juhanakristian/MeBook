import QtQuick 1.0
import com.meego 1.0

import com.mebook.bookview 1.0
import com.mebook.settings 1.0

import "components"

Page {
    id: readingPage
    tools: readingPageTools


    Rectangle{
        anchors.fill: parent
        color: "#edeadb"
    }

    ToolBarLayout {
        id: readingPageTools
        visible: true
        ToolIcon { platformIconId: "toolbar-back";
             anchors.left: parent===undefined ? undefined : parent.left
             onClicked: {
                 bookView.closeCurrentBook();
                 pageStack.pop();
             }
        }
        ToolIcon {
            platformIconId: "toolbar-view-menu";
            onClicked: {
                selectorDialog.selected.connect(bookView.openTOCItem);
                selectorDialog.titleText = "Table of content"
                selectorDialog.listModel = bookView.tableOfContent();
                selectorDialog.open();
            }
        }
        ToolIcon {
            platformIconId: "toolbar-previous";
            onClicked: bookView.previousChapter();
        }

        ToolIcon {
            platformIconId: "toolbar-next";
            onClicked: bookView.nextChapter();
        }




    }

    function setBook(id) {
        bookView.loadBookWithId(id);
    }

    BookView {
        id: bookView
        width:parent.width
        anchors.top: parent.top
        anchors.bottom: bottomBar.top
        onPositionInBookChanged: progressSlider.value = positionInBook * 100.0
        onDoubleClicked: {
            appWindow.showToolBar = !appWindow.showToolBar;
        }
    }

    Rectangle{
        id: bottomBar
        height: childrenRect.height
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: Qt.rgba(0.0, 0.0, 0.0, 0.5)
        Slider{
            id: progressSlider
            anchors.left: parent.left
            anchors.right: parent.right
            stepSize: 1
            maximumValue: 100
            minimumValue: 0

        }

    }




    SelectorDialog {
        id: selectorDialog
        onSelectedIndexChanged: selected.disconnect(bookView.openTOCItem);
    }


}

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

    orientationLock: PageOrientation.LockPortrait

    ToolBarLayout {
        id: readingPageTools
        visible: true
        ToolIcon {
            platformIconId: "toolbar-back";
            anchors.left: parent===undefined ? undefined : parent.left
            onClicked: {
                 bookView.closeCurrentBook();
                 pageStack.pop();
            }
        }
        ToolIcon {
            platformIconId: "toolbar-view-menu";
            onClicked: {
                selectorDialog.titleText = "Table of content"
                selectorDialog.model = bookView.tableOfContent();
                selectorDialog.accepted.connect(loadTOCItem);
                selectorDialog.open();
            }
        }
        ToolIcon {
            platformIconId: "toolbar-attachment"
        }
        ToolIcon {
            platformIconId: "toolbar-edit"
        }

        ToolIcon {
            platformIconId: "toolbar-previous";
            onClicked: bookView.previousChapter();
            visible: !settings.viewMode;
        }

        ToolIcon {
            platformIconId: "toolbar-next";
            onClicked: bookView.nextChapter();
            visible: !settings.viewMode;
        }




    }

    function setBook(id) {
        bookView.loadBookWithId(id);
    }

    function loadTOCItem() {
        bookView.openTOCItem(selectorDialog.selectedIndex);
        selectorDialog.accepted.disconnect(loadTOCItem);
    }

    BookView {
        id: bookView
        width:parent.width
        anchors.top: parent.top
        anchors.bottom: bottomBar.top
        Rectangle{
            anchors.fill: parent
            color: "#edeadb"
            z: -1
        }

        onPositionInBookChanged: progressSlider.value = positionInBook * 100.0
        clip: settings.viewMode
        onDoubleClicked: {
            appWindow.showToolBar = !appWindow.showToolBar;
            appWindow.showStatusBar = !appWindow.showStatusBar;
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

    MySelectionDialog{
        id: selectorDialog
    }



}

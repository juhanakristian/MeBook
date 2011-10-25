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

        Behavior on contentY {
            NumberAnimation{
                duration: 200
                easing.type: Easing.InCubic
            }
        }

        onContextMenu: (contextMenu.status == DialogStatus.Closed) ? contextMenu.open() : contextMenu.close()

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

    Rectangle {
        id: bottomPane
        height: 400
        width: parent.width
        state: "Hidden"

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

        Item {
            id: content
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.top: closeButton.bottom
            anchors.margins: 10


            Column {
                anchors.fill: parent
                spacing: 10

                TextArea{
                    id: annotationText
                    width: parent.width
                    height: parent.height - parent.spacing - colorSelector.height
                }

                ColorSelector{
                    id: colorSelector
                    width: parent.width
                    colors: [ "#eeeeec", "#d3d7cf", "#dabdb6", "#fce94f", "#edd400", "#c4a000",
                        "#8ae234", "#73d216", "#4e9a06", "#fcaf3e", "#f57900", "#ce5c00",
                        "#e9b96e", "#c17d11", "#8f5902", "#729fcf", "#3465a4", "#204a87",
                        "#ad7fa8", "#75507b", "#5c3566", "#888a85", "#555753", "#2e3436",
                        "#ef2929", "#cc0000", "#a40000" ]
                }
            }


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
                    target: bookView
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
                    target: bookView
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

    Menu{
        id: contextMenu
        visualParent: readingPage
        platformStyle: Style {
            id: style

            property real leftMargin: ( (screen.currentOrientation == 1) ||
                                         (screen.currentOrientation == 4) ) ? 0 : 427
            property real rightMargin: ( (screen.currentOrientation == 1) ||
                                         (screen.currentOrientation == 4) ) ? 0 : 0
            property real topMargin: ( (screen.currentOrientation == 1) ||
                                       (screen.currentOrientation == 4) ) ? 246 : 0

            property real bottomMargin: 0

            property real leftPadding: 16
            property real rightPadding: 16
            property real topPadding: 16
            property real bottomPadding: 16

            // fader properties
            property double dim: 0.9
            property int fadeInDuration: 350 // ms
            property int fadeOutDuration: 350 // ms
            property int fadeInDelay: 0 // ms
            property int fadeOutDelay: 0 // ms
            property int fadeInEasingType: Easing.InOutQuint
            property int fadeOutEasingType: Easing.InOutQuint
            property url faderBackground: "image://theme/meegotouch-menu-dimmer"

            property int pressDelay: 0 // ms

            property url background: "image://theme/meegotouch-menu-background" + __invertedString
        }

        MenuLayout{
            MenuItem {text: "Add annotation"; onClicked: bottomPane.state = "Visible"; }
            MenuItem {text: "Add bookmark"; onClicked: bottomPane.state = "Visible"; }
            MenuItem {text: "Dictionary"; onClicked: bottomPane.state = "Visible"; }
        }
    }


}

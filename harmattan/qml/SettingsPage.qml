import QtQuick 1.0
import com.meego 1.0

import com.mebook.bookview 1.0
import com.mebook.settings 1.0

import "components"

Page {
    id: settingsPage
    tools: settingPageTools

    ToolBarLayout {
        id: settingPageTools
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

    Flickable {
        anchors.fill: parent
        contentHeight: childrenRect.height
        contentWidth: parent.width
        anchors.topMargin: 10


        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 10
            anchors.bottomMargin: 10
            height:  childrenRect.height + 20
            spacing: 15

//            Label{
//                anchors.left: parent.left
//                font.bold: true
//                text: "Viewing mode"
//                anchors.topMargin: 20
//                anchors.margins: 5
//            }

            //View modes disabled for now
//            ButtonRow {
//                anchors.left: parent.left
//                anchors.right: parent.right
//                anchors.margins: 5
//                Button{ id: scroll; text: "Scroll"; checked: !settings.viewMode; onClicked: settings.setViewMode(0);}
//                Button{ id: page; text: "Page"; checked: settings.viewMode; onClicked: settings.setViewMode(1);}


//            }

            Label{
                anchors.left: parent.left
                font.bold: true
                text: "Color mode"
                anchors.topMargin: 20
                anchors.margins: 5
            }

            ButtonRow {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5
                Button{ id: norm; text: "Normal"; onClicked: settings.setInvertedColors(false);}
                Button{ id: inv; text: "Inverted";onClicked: settings.setInvertedColors(true);}

                Component.onCompleted: {
                    var inverted = settings.invertedColors();
                    if(inverted)
                        inv.checked = true;
                    else
                        norm.checked = true;

                }

            }



            SelectorButton {
                text: "Font"
                onClicked: fontSelectionDialog.open()
                selectedText: fontSelectionDialog.model.get(fontSelectionDialog.selectedIndex).name
                onSelectedTextChanged: {
                    settings.setFontFamily(selectedText);
                }

            }

            Label{
                anchors.left: parent.left
                font.bold: true
                text: "Font size"
                anchors.topMargin: 20
                anchors.margins: 5
            }

            ButtonRow {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5
                Button{ id: b16; text: "A"; font.pixelSize: 16; onClicked: settings.setFontPixelSize(16);}
                Button{ id: b24; text: "A"; font.pixelSize: 24; onClicked: settings.setFontPixelSize(24);}
                Button{ id: b38; text: "A"; font.pixelSize: 38; onClicked: settings.setFontPixelSize(38);}

                Component.onCompleted: {
                    var size = settings.fontPixelSize();
                    switch(size) {
                    case 16: b16.checked = true;
                        break;
                    case 24: b24.checked = true;
                        break;
                    case 32: b32.checked = true;
                        break;
                    default: b24.checked = true; settings.setFontPixelSize(24);
                        break;
                    }
                }
            }

        }
    }

    SelectionDialog{
        id: fontSelectionDialog
        titleText: "Select font"
        Component.onCompleted: {
            var selected = settings.fontFamily();
            console.log("Settings font family:" + selected);
            if(selected == "Serif")
                fontSelectionDialog.selectedIndex = 0;
            else if(selected == "Sans Serif")
                fontSelectionDialog.selectedIndex = 1;
            else
                fontSelectionDialog.selectedIndex = 0
        }

        model:  ListModel {
            id: fontModel
            ListElement { name: "Serif" }
            ListElement { name: "Sans Serif" }
        }
    }


}

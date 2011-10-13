import QtQuick 1.0
import com.meego 1.0

import com.mebook.bookview 1.0
import com.mebook.settings 1.0

import "components"

Page {
    id: bookInfoPage
    tools: ToolBarLayout {
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

}

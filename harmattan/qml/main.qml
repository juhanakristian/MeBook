import QtQuick 1.1
import com.meego 1.0

PageStackWindow {
    id: appWindow

    initialPage: mainPage

    MainPage{id: mainPage}
    SettingsPage{ id: settingsPage }
    ReadingPage{ id: readingPage }
    DownloadBooksPage{ id: downloadBooksPage }
    Component.onCompleted: theme.inverted = true;


//    ToolBarLayout {
//        id: commonTools
//        visible: true
//        ToolIcon { platformIconId: "toolbar-view-menu";
//             anchors.right: parent===undefined ? undefined : parent.right
//             onClicked: (myMenu.status == DialogStatus.Closed) ? myMenu.open() : myMenu.close()
//        }
//    }


}

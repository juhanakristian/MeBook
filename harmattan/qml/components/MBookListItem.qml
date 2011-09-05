import Qt 4.7
import com.meego 1.0

Rectangle{
    id: mbooklistitem

    // anchors.left: parent.left
    // anchors.right: parent.right
    width: 100 

    property int margins: 20
    height: childrenRect.height + margins

    // height: (coverImage.paintedHeight > (
    //             name.paintedHeight + 
    //             author.paintedHeight +
    //             category.paintedHeight)) ? 
    //                 (coverImage.height + margins) : (
    //                 name.paintedHeight + 
    //                 author.paintedHeight +
    //                 category.paintedHeight) + margins + 10

    color: "#000000" 
    border.color: "#999999"
    radius: 10

    property alias bookName: name.text
    property alias bookAuthor: author.text
    property alias bookCategory: category.text
    property alias coverImageSource: coverImage.source

    signal clicked
    signal contextMenu(int x, int y)

    Row{
        id: contentRow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        spacing: 5

        Image{
            id: coverImage
            width: 100
            fillMode: Image.PreserveAspectFit
        }

        Column{
            anchors.margins: 10
            spacing: 5
            width: parent.width - coverImage.width
            Text{
                id: name
                font.family: "Arial"
                font.pixelSize: 22
                font.bold: true
                color: "#ffffff"
                wrapMode: Text.WordWrap
                width: parent.width
            }

            Text{
                id: author
                font.family: "Arial"
                font.pixelSize: 18
                color: "#ffffff"
                anchors.topMargin: -5
                wrapMode: Text.WordWrap
                width: parent.width
            }

            Text{
                id: category
                font.family: "Arial"
                font.pixelSize: 12
                font.italic: true
                color: "#ffffff"
                wrapMode: Text.WordWrap
                width: parent.width
            }

        }
    }

    // MouseArea{
    //     id: mouseArea
    //     anchors.fill: parent
    //     onClicked:{
    //         if(mouse.button == Qt.LeftButton){
    //             mbooklistitem.clicked();
    //         } else if(mouse.button == Qt.RightButton){
    //             mbooklistitem.contextMenu(mouse.x, mouse.y);
    //         }
    //     }
    //     onPressAndHold:{
    //         var obj = mapToItem(null, mouse.x, mouse.y);
    //         mbooklistitem.contextMenu(obj.x, obj.y)
    //     }
    // }

}

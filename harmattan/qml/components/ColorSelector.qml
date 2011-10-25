import Qt 4.7

Item{
    id: colorselector
    height: colorGrid.height
    property variant colors

    property int selectedIndex: 0
    property int colorRectSize: 47

    Component{
        id: colorButton
        Rectangle{
            id: colorRect
            property alias buttonColor: colorRect.color
            property int index
            property bool selected: (selectedIndex == index)
            width: colorRectSize
            height: colorRectSize
            radius: 5
            smooth: true
            border.width: selected ? 3 : 1
            border.color: selected ? "#00AAFF" : "#FFFFFF"
            MouseArea{
                id: mouseArea
                anchors.fill: parent
                onClicked: selectedIndex = index;
            }
        }
    }

    Component.onCompleted: {
        for(var i = 0; i < colors.length; i++){
            var colr = colors[i];
            var obj = colorButton.createObject(colorGrid);//, { "colorRect.buttonColor": colr });
            obj.buttonColor = colr;
            obj.index = i;
            
        }
    }

    Grid{
        id: colorGrid
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 1

        columns: (parent.width / (colorRectSize+3))
        spacing: 4
    }
}

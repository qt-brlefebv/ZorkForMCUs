// ZorkKeyboard.qml
//
// Simple on-screen keyboard for text adventure input.
// Qt for MCUs compatible - uses hardcoded font sizes for Static font engine.
// Font sizes: 16 (regular keys), 20 (special keys < and GO)
import QtQuick

Rectangle {
    id: keyboard
    color: "#1a1a2e"
    
    // Signals - parent handles the text manipulation
    signal keyPressed(string key)
    signal enterPressed()
    signal backspacePressed()
    
    // Keyboard sizing - adjust for different displays
    property int keyWidth: 42
    property int keyHeight: 38
    property int keySpacing: 3
    property int rowSpacing: 3
    
    // Colors matching Zork theme
    property color keyColor: "#263238"
    property color keyPressedColor: "#4f5b62"
    property color keyTextColor: "#ffffff"
    property color specialKeyColor: "#01579b"
    
    // Explicit height (4 rows + spacing + padding)
    height: (keyHeight * 4) + (rowSpacing * 3) + 8
    
    Column {
        anchors.centerIn: parent
        spacing: keyboard.rowSpacing
        
        // Row 1: Numbers
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: keyboard.keySpacing
            
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: ma1.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "1"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: ma1; anchors.fill: parent; onClicked: keyboard.keyPressed("1") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: ma2.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "2"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: ma2; anchors.fill: parent; onClicked: keyboard.keyPressed("2") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: ma3.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "3"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: ma3; anchors.fill: parent; onClicked: keyboard.keyPressed("3") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: ma4.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "4"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: ma4; anchors.fill: parent; onClicked: keyboard.keyPressed("4") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: ma5.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "5"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: ma5; anchors.fill: parent; onClicked: keyboard.keyPressed("5") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: ma6.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "6"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: ma6; anchors.fill: parent; onClicked: keyboard.keyPressed("6") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: ma7.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "7"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: ma7; anchors.fill: parent; onClicked: keyboard.keyPressed("7") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: ma8.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "8"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: ma8; anchors.fill: parent; onClicked: keyboard.keyPressed("8") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: ma9.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "9"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: ma9; anchors.fill: parent; onClicked: keyboard.keyPressed("9") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: ma0.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "0"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: ma0; anchors.fill: parent; onClicked: keyboard.keyPressed("0") }
            }
        }
        
        // Row 2: QWERTYUIOP
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: keyboard.keySpacing
            
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maQ.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "Q"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maQ; anchors.fill: parent; onClicked: keyboard.keyPressed("q") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maW.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "W"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maW; anchors.fill: parent; onClicked: keyboard.keyPressed("w") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maE.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "E"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maE; anchors.fill: parent; onClicked: keyboard.keyPressed("e") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maR.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "R"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maR; anchors.fill: parent; onClicked: keyboard.keyPressed("r") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maT.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "T"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maT; anchors.fill: parent; onClicked: keyboard.keyPressed("t") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maY.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "Y"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maY; anchors.fill: parent; onClicked: keyboard.keyPressed("y") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maU.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "U"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maU; anchors.fill: parent; onClicked: keyboard.keyPressed("u") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maI.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "I"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maI; anchors.fill: parent; onClicked: keyboard.keyPressed("i") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maO.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "O"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maO; anchors.fill: parent; onClicked: keyboard.keyPressed("o") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maP.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "P"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maP; anchors.fill: parent; onClicked: keyboard.keyPressed("p") }
            }
        }
        
        // Row 3: ASDFGHJKL + Backspace
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: keyboard.keySpacing
            
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maA.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "A"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maA; anchors.fill: parent; onClicked: keyboard.keyPressed("a") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maS.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "S"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maS; anchors.fill: parent; onClicked: keyboard.keyPressed("s") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maD.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "D"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maD; anchors.fill: parent; onClicked: keyboard.keyPressed("d") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maF.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "F"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maF; anchors.fill: parent; onClicked: keyboard.keyPressed("f") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maG.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "G"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maG; anchors.fill: parent; onClicked: keyboard.keyPressed("g") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maH.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "H"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maH; anchors.fill: parent; onClicked: keyboard.keyPressed("h") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maJ.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "J"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maJ; anchors.fill: parent; onClicked: keyboard.keyPressed("j") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maK.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "K"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maK; anchors.fill: parent; onClicked: keyboard.keyPressed("k") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maL.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "L"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maL; anchors.fill: parent; onClicked: keyboard.keyPressed("l") }
            }
            // Backspace
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maBS.pressed ? keyboard.keyPressedColor : keyboard.specialKeyColor
                Text { anchors.centerIn: parent; text: "<"; color: keyboard.keyTextColor; font.pixelSize: 20 }
                MouseArea { id: maBS; anchors.fill: parent; onClicked: keyboard.backspacePressed() }
            }
        }
        
        // Row 4: ZXCVBNM + Space + Enter
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: keyboard.keySpacing
            
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maZ.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "Z"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maZ; anchors.fill: parent; onClicked: keyboard.keyPressed("z") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maX.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "X"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maX; anchors.fill: parent; onClicked: keyboard.keyPressed("x") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maC.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "C"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maC; anchors.fill: parent; onClicked: keyboard.keyPressed("c") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maV.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "V"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maV; anchors.fill: parent; onClicked: keyboard.keyPressed("v") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maB.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "B"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maB; anchors.fill: parent; onClicked: keyboard.keyPressed("b") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maN.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "N"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maN; anchors.fill: parent; onClicked: keyboard.keyPressed("n") }
            }
            Rectangle {
                width: keyboard.keyWidth; height: keyboard.keyHeight; radius: 4
                color: maM.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "M"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maM; anchors.fill: parent; onClicked: keyboard.keyPressed("m") }
            }
            // Space bar
            Rectangle {
                width: keyboard.keyWidth * 1.5; height: keyboard.keyHeight; radius: 4
                color: maSpace.pressed ? keyboard.keyPressedColor : keyboard.keyColor
                Text { anchors.centerIn: parent; text: "_"; color: keyboard.keyTextColor; font.pixelSize: 16 }
                MouseArea { id: maSpace; anchors.fill: parent; onClicked: keyboard.keyPressed(" ") }
            }
            // Enter/Go
            Rectangle {
                width: keyboard.keyWidth * 1.5; height: keyboard.keyHeight; radius: 4
                color: maEnter.pressed ? keyboard.keyPressedColor : keyboard.specialKeyColor
                Text { anchors.centerIn: parent; text: "GO"; color: keyboard.keyTextColor; font.pixelSize: 20 }
                MouseArea { id: maEnter; anchors.fill: parent; onClicked: keyboard.enterPressed() }
            }
        }
    }
}

/*
 * ZorkUI_RT1050.qml
 *
 * Stripped-down UI for RT1050 (512KB flash limit).
 * Uses custom ZorkKeyboard instead of VirtualKeyboard module to save ~400KB.
 * Uses Static font engine - all font sizes hardcoded (no runtime bindings).
 */
import QtQuick

Rectangle {
    id: root
    width: FizmoBackend.screenWidth
    height: FizmoBackend.screenHeight
    color: "#1a1a2e"  // Dark blue-black background

    // Hidden text to force static font engine to generate all ASCII glyphs
    // Zork uses basic Latin alphabet, numbers, and punctuation
    // Font sizes: 16 (UI + keyboard), 20 (special keys), 28 (GAME OVER)
    Text {
        visible: false
        font.pixelSize: 16
        text: "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 .,!?'\"-:;()[]@#$%&*+=/<>_"
    }
    Text {
        visible: false
        font.pixelSize: 20
        text: "<GO"
    }
    Text {
        visible: false
        font.pixelSize: 28
        text: "GAMEOVER "
    }

    // Track version changes to trigger text updates
    property int outputVer: FizmoBackend.outputVersion
    property int statusVer: FizmoBackend.statusVersion
    property int cmdVer: FizmoBackend.commandVersion

    // Local state for keyboard visibility (since Qt for MCUs focus is limited)
    property bool keyboardVisible: false

    // Status bar at top
    Rectangle {
        id: statusBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: FizmoBackend.statusHeight
        color: "#16213e"

        // Room name on the left
        Text {
            id: roomText
            anchors.left: parent.left
            anchors.leftMargin: FizmoBackend.margin * 2
            anchors.verticalCenter: parent.verticalCenter
            color: "#e8e8e8"
            font.pixelSize: 16
            text: root.statusVer >= 0 ? FizmoBackend.getStatusRoom() : ""
        }

        // Score/moves on the right
        Text {
            id: scoreText
            anchors.right: parent.right
            anchors.rightMargin: FizmoBackend.margin * 2
            anchors.verticalCenter: parent.verticalCenter
            color: "#a0a0a0"
            font.pixelSize: 16
            text: root.statusVer >= 0 ? FizmoBackend.getStatusScore() : ""
        }
    }

    // Main text output area
    Flickable {
        id: outputFlickable
        anchors.top: statusBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: inputArea.top
        anchors.margins: FizmoBackend.margin
        clip: true
        contentWidth: width
        contentHeight: outputText.height

        Text {
            id: outputText
            width: outputFlickable.width - 70  // Reserve right side for GUI icons
            color: "#00ff88"  // Classic green terminal color
            font.pixelSize: 16
            wrapMode: Text.Wrap
            text: root.outputVer >= 0 ? FizmoBackend.getOutputText() : ""
        }
    }

    // Auto-scroll when output version changes
    onOutputVerChanged: {
        // Scroll to bottom
        if (outputFlickable.contentHeight > outputFlickable.height) {
            outputFlickable.contentY = outputFlickable.contentHeight - outputFlickable.height
        }
    }

    // Compass rose for directional navigation
    Image {
        id: compassRose
        source: "images/compass-rose.png"
        width: 64
        height: 64
        opacity: 0.7
        anchors.right: parent.right
        anchors.bottom: inputArea.top
        anchors.rightMargin: FizmoBackend.margin
        anchors.bottomMargin: FizmoBackend.margin
        visible: FizmoBackend.waitingForInput

        MouseArea {
            anchors.fill: parent
            onClicked: {
                // Calculate angle from center to touch point
                var dx = mouseX - parent.width / 2
                var dy = mouseY - parent.height / 2
                var distance = Math.sqrt(dx * dx + dy * dy)

                // Dead zone in center (less than 25% of radius)
                if (distance < parent.width * 0.125) {
                    return
                }

                // Calculate angle in degrees (-180 to 180, 0 = right/east)
                var angle = Math.atan2(dy, dx) * 180 / Math.PI

                // Map angle to 8 directions (each covers 45 degrees)
                // Adjust so N is at top (-90 degrees in math coords)
                var direction
                if (angle >= -22.5 && angle < 22.5) {
                    direction = "e"
                } else if (angle >= 22.5 && angle < 67.5) {
                    direction = "se"
                } else if (angle >= 67.5 && angle < 112.5) {
                    direction = "s"
                } else if (angle >= 112.5 && angle < 157.5) {
                    direction = "sw"
                } else if (angle >= 157.5 || angle < -157.5) {
                    direction = "w"
                } else if (angle >= -157.5 && angle < -112.5) {
                    direction = "nw"
                } else if (angle >= -112.5 && angle < -67.5) {
                    direction = "n"
                } else {
                    direction = "ne"
                }

                // Send direction command to fizmo
                FizmoBackend.submitLine(direction)
            }
        }
    }

    // Input area
    Rectangle {
        id: inputArea
        anchors.bottom: keyboard.visible ? keyboard.top : parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: FizmoBackend.waitingForInput ? FizmoBackend.inputHeight : 0
        color: "#0f3460"
        visible: FizmoBackend.waitingForInput

        // Prompt indicator
        Text {
            id: promptText
            anchors.left: parent.left
            anchors.leftMargin: FizmoBackend.margin * 2
            anchors.verticalCenter: parent.verticalCenter
            color: "#00ff88"
            font.pixelSize: 16
            text: ">"
        }

        // Command text display (tap to show keyboard)
        Text {
            id: commandDisplay
            anchors.left: promptText.right
            anchors.leftMargin: FizmoBackend.margin
            anchors.right: parent.right
            anchors.rightMargin: FizmoBackend.margin * 2
            anchors.verticalCenter: parent.verticalCenter
            color: "#ffffff"
            font.pixelSize: 16
            text: root.cmdVer >= 0 ? FizmoBackend.getCommandText() : ""
        }
        
        // Placeholder text
        Text {
            anchors.left: promptText.right
            anchors.leftMargin: FizmoBackend.margin
            anchors.verticalCenter: parent.verticalCenter
            color: "#a0a0a0"
            font.pixelSize: 16
            text: "Tap here to type..."
            visible: commandDisplay.text.length === 0
        }

        // Tap anywhere in input area to show keyboard
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (FizmoBackend.vkeyboardAlways) {
                    // Always-visible mode: keyboard is always shown
                } else {
                    // Toggle mode: show keyboard on tap
                    root.keyboardVisible = true
                }
            }
        }
    }

    // Auto-show keyboard when waiting for input (if vkeyboardAlways is true)
    Connections {
        target: FizmoBackend
        function onWaitingForInputChanged(waiting: bool) {
            if (waiting && FizmoBackend.vkeyboardAlways) {
                root.keyboardVisible = true
            }
        }
    }

    // Custom keyboard - lightweight alternative to VirtualKeyboard module
    ZorkKeyboard {
        id: keyboard
        width: parent.width
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        // vkeyboardAlways=true: show whenever waiting for input
        // vkeyboardAlways=false: show only when user taps input area
        visible: FizmoBackend.waitingForInput && (FizmoBackend.vkeyboardAlways || root.keyboardVisible)
    }

    // Handle keyboard signals using Connections (Qt for MCUs requirement)
    Connections {
        target: keyboard
        function onKeyPressed(key: string) {
            FizmoBackend.appendCommandChar(key)
        }

        function onBackspacePressed() {
            FizmoBackend.commandBackspace()
        }

        function onEnterPressed() {
            // Submit command to fizmo
            FizmoBackend.submitCommand()
            // Hide keyboard after submit if not always-visible mode
            if (!FizmoBackend.vkeyboardAlways) {
                root.keyboardVisible = false
            }
        }
    }

    // Handle keyboard input from external keyboard
    Keys.onPressed: {
        if (FizmoBackend.waitingForChar) {
            // Single character input for [MORE] prompts
            FizmoBackend.submitChar(event.key)
        }
        // Note: Full line input from external keyboard would need additional handling
    }

    // Game over overlay
    Rectangle {
        id: gameOverOverlay
        anchors.fill: parent
        color: "#80000000"
        visible: FizmoBackend.gameExited

        Text {
            anchors.centerIn: parent
            color: "#ffffff"
            font.pixelSize: 28
            text: "GAME OVER"
        }
    }
}

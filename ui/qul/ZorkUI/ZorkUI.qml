/*
 * ZorkUI.qml
 *
 * Main UI for the Zork text adventure game.
 * Supports RT1050 EVK (480x272 landscape) and RT1170 EVK-B (720x1280 portrait).
 */
import QtQuick 2.15
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

Rectangle {
    id: root
    width: FizmoBackend.screenWidth
    height: FizmoBackend.screenHeight
    color: "#1a1a2e"  // Dark blue-black background

    // Set keyboard locale to English
    Component.onCompleted: {
        VirtualKeyboardSettings.locale = "en"
    }

    // Track version changes to trigger text updates
    property int outputVer: FizmoBackend.outputVersion
    property int statusVer: FizmoBackend.statusVersion

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
            font.pixelSize: FizmoBackend.fontSize
            text: root.statusVer >= 0 ? FizmoBackend.getStatusRoom() : ""
        }

        // Score/moves on the right
        Text {
            id: scoreText
            anchors.right: parent.right
            anchors.rightMargin: FizmoBackend.margin * 2
            anchors.verticalCenter: parent.verticalCenter
            color: "#a0a0a0"
            font.pixelSize: FizmoBackend.fontSize
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
            font.pixelSize: FizmoBackend.fontSize
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

    // Input area (above keyboard when visible)
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
            font.pixelSize: FizmoBackend.fontSize
            text: ">"
        }

        // Text input field
        TextInput {
            id: commandInput
            anchors.left: promptText.right
            anchors.leftMargin: FizmoBackend.margin
            anchors.right: parent.right
            anchors.rightMargin: FizmoBackend.margin * 2
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height  // Make the hit area taller
            color: "#ffffff"
            font.pixelSize: FizmoBackend.fontSize
            activeFocusOnPress: true

            // Customize enter key for text adventure commands
            EnterKeyAction.actionId: EnterKeyAction.Send
            EnterKeyAction.label: "GO"

            onAccepted: {
                // Submit the command to fizmo
                FizmoBackend.submitLine(text)
                clear()
                // Hide keyboard after submit if not always-visible mode
                if (!FizmoBackend.vkeyboardAlways) {
                    root.forceActiveFocus()
                }
            }
        }
    }

    // Auto-focus input when waiting for input (only if keyboard always visible)
    onStatusVerChanged: {
        if (FizmoBackend.waitingForInput && FizmoBackend.vkeyboardAlways) {
            commandInput.forceActiveFocus()
        }
    }

    // Also trigger on waitingForInput change
    Connections {
        target: FizmoBackend
        function onWaitingForInputChanged(waiting: bool) {
            if (waiting && FizmoBackend.vkeyboardAlways) {
                commandInput.forceActiveFocus()
            }
        }
    }

    // Virtual keyboard - shows automatically when TextInput has focus
    // RT1170: always visible when waiting for input (vkeyboardAlways=true)
    // RT1050: appears on tap, hides on submit (vkeyboardAlways=false)
    InputPanel {
        id: keyboard
        width: FizmoBackend.screenWidth
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        // vkeyboardAlways=true: show whenever waiting for input
        // vkeyboardAlways=false: show only when input field has focus (tap to show)
        visible: FizmoBackend.vkeyboardAlways ? FizmoBackend.waitingForInput : commandInput.activeFocus
    }

    // Handle single character input (for [MORE] prompts, etc.)
    Keys.onPressed: {
        if (FizmoBackend.waitingForChar) {
            FizmoBackend.submitChar(event.key)
        }
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
            font.pixelSize: FizmoBackend.fontSize * 2
            text: "GAME OVER"
        }
    }
}

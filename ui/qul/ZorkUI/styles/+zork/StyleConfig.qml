// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

pragma Singleton

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Styles

Item {
    property KeyboardStyle style

    readonly property bool compactSelectionList: {
        if (InputContext.inputEngine.inputMode === InputEngine.InputMode.Pinyin ||
            InputContext.inputEngine.inputMode === InputEngine.InputMode.Cangjie ||
            InputContext.inputEngine.inputMode === InputEngine.InputMode.Zhuyin) {
            return true;
        } else {
            return false;
        }
    }

    readonly property real keyBackgroundMargin: Math.round(8 * StyleConstants.scaleHint)
    readonly property real keyContentMargin: Math.round(40 * StyleConstants.scaleHint)

    readonly property real keyIconScale: StyleConstants.scaleHint * 0.8
    readonly property string resourcePrefix: "qrc:/"

    readonly property string inputLocale: InputContext.locale
    property color primaryColor: "#263238"
    property color primaryLightColor: "#4f5b62"
    property color primaryDarkColor: "#000a12"
    property color textOnPrimaryColor: "#ffffff"
    property color secondaryColor: "#01579b"
    property color secondaryLightColor: "#4f83cc"
    property color secondaryDarkColor: "#002f6c"
    property color textOnSecondaryColor: "#ffffff"

    property color keyboardBackgroundColor: primaryColor
    property color normalKeyBackgroundColor: primaryDarkColor
    property color highlightedKeyBackgroundColor: primaryLightColor
    property color capsLockKeyAccentColor: secondaryColor
    property color modeKeyAccentColor: textOnPrimaryColor
    property color keyTextColor: textOnPrimaryColor
    property color keySmallTextColor: textOnPrimaryColor

    property color popupBackgroundColor: secondaryColor
    property color popupBorderColor: secondaryLightColor
    property color popupTextColor: textOnSecondaryColor
    property color popupHighlightColor: secondaryLightColor
    property color selectionListTextColor: textOnPrimaryColor
    property color selectionListSeparatorColor: primaryLightColor
    property color selectionListBackgroundColor: primaryColor
    property color navigationHighlightColor: "yellow"

    // Hide language indicator on space bar
    property real inputLocaleIndicatorOpacity: 0
    property Timer inputLocaleIndicatorHighlightTimer: Timer {
        interval: 1000
    }
    onInputLocaleChanged: {
        // Keep opacity at 0
    }
}

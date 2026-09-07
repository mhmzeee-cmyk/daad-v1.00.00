import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

ToolBar {
    id: statusBar
    function loc(key) { var _ = translator.languageVersion; return translator ? translator.translate(key) : key }

    property alias message: statusLabel.text
    property bool isBusy: false

    function showMessage(text) {
        statusLabel.text = text
        hideTimer.restart()
    }

    background: Rectangle {
        color: th.statusBarBg
        Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 1
            color: th.borderStone
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 12

        Rectangle {
            width: 6; height: 6
            radius: 3
            color: isBusy ? th.accentGold : th.accentLapis
        }

        Label {
            id: statusLabel
            text: loc("ready")
            color: th.textSecondary
            font.family: th.fontFamilyUI
            font.pixelSize: th.fontSizeSmall
            Layout.fillWidth: true
            elide: Label.ElideRight
        }

        Rectangle { width: 1; height: 12; color: th.borderStone; Layout.alignment: Qt.AlignVCenter }
        Label { text: "UTF-8"; color: th.textMuted; font.family: th.fontFamilyCode; font.pixelSize: 10; Layout.alignment: Qt.AlignVCenter }
        Rectangle { width: 1; height: 12; color: th.borderStone; Layout.alignment: Qt.AlignVCenter }
        Label { text: "\u0636"; color: th.accentLapis; font.family: th.fontFamilyCode; font.pixelSize: 11; font.bold: true; Layout.alignment: Qt.AlignVCenter }
        Rectangle { width: 1; height: 12; color: th.borderStone; Layout.alignment: Qt.AlignVCenter }
        Label { text: "v2.0.0"; color: th.textMuted; font.family: th.fontFamilyCode; font.pixelSize: 10; Layout.alignment: Qt.AlignVCenter }

        Rectangle {
            width: 1; height: 12
            color: th.borderStone
            Layout.alignment: Qt.AlignVCenter
        }

        Rectangle {
            width: 44; height: 18
            radius: 9
            color: themeToggleMouse.containsMouse ? th.backgroundHover : "transparent"
            border.width: 1
            border.color: th.borderStone

            Label {
                anchors.centerIn: parent
                text: th.isDark ? "L" : "D"
                font.pixelSize: 11
                color: th.textMuted
            }

            MouseArea {
                id: themeToggleMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: th.toggleTheme()
            }

            ToolTip {
                text: th.isDark ? loc("theme_light") : loc("theme_dark")
                visible: themeToggleMouse.containsMouse
                delay: 400
            }
        }
    }

    Timer {
        id: hideTimer
        interval: 5000
        onTriggered: statusLabel.text = loc("ready")
    }
}

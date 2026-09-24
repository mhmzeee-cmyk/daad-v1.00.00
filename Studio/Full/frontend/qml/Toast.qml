import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: toast
    width: Math.min(parent.width - 40, 520)
    height: 48
    radius: 8
    color: th.isDark ? "#252527" : th.panelBg
    border.color: th.borderStone
    border.width: 1
    opacity: 0
    visible: opacity > 0
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 24
    anchors.horizontalCenter: parent.horizontalCenter

    property string message: ""
    property string type: "info"
    property alias timer: hideTimer

    function show(msg, msgType, duration) {
        message = msg
        type = msgType || "info"
        hideTimer.interval = duration || 4000
        showAnim.start()
        hideTimer.restart()
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 10

        Rectangle {
            width: 4
            height: 24
            radius: 2
            color: toast.type === "error" ? th.semanticError :
                   toast.type === "success" ? th.accentLapis : th.accentLapis
            opacity: 0.8
        }

        Label {
            text: toast.message
            color: th.textInk
            font.family: th.fontFamilyUI
            font.pixelSize: 12
            Layout.fillWidth: true
            elide: Label.ElideRight
            maximumLineCount: 2
            wrapMode: Label.Wrap
        }

        Label {
            text: "\u2715"
            color: th.textMuted
            font.pixelSize: 14
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: hideAnim.start()
            }
        }
    }

    NumberAnimation {
        id: showAnim
        target: toast
        property: "opacity"
        from: 0; to: 1
        duration: 200
        easing.type: Easing.OutCubic
    }

    NumberAnimation {
        id: hideAnim
        target: toast
        property: "opacity"
        from: 1; to: 0
        duration: 200
        easing.type: Easing.InCubic
    }

    Timer {
        id: hideTimer
        running: false
        repeat: false
        onTriggered: hideAnim.start()
    }
}

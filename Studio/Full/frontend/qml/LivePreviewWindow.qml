import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: previewWindow
    title: "\u{1F441} " + (translator ? translator.translate("live_preview") : "\u0645\u0639\u0627\u064A\u0646\u0629 \u062D\u064A\u0629")
    width: 800
    height: 600
    visible: false
    color: th.backgroundPaper
    flags: Qt.Window | Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint

    property var designerModel: null
    property bool isDarkTheme: th.isDark

    function loadPreview(model) {
        designerModel = model
        previewContainer.model = model
        previewWindow.visible = true
        previewWindow.raise()
        previewWindow.requestActivate()
    }

    Rectangle {
        anchors.fill: parent
        color: previewWindow.isDarkTheme ? "#1A1A1C" : "#FFFFFF"

        Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 36
            color: previewWindow.isDarkTheme ? "#252527" : "#F1F3F5"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12

                Label {
                    text: "\u{1F441} " + (translator ? translator.translate("live_preview") : "\u0645\u0639\u0627\u064A\u0646\u0629 \u062D\u064A\u0629")
                    color: previewWindow.isDarkTheme ? "#F1F5F9" : "#1E293B"
                    font.family: th.fontFamilyUI
                    font.pixelSize: 12
                    font.bold: true
                }

                Item { Layout.fillWidth: true }

                Rectangle {
                    width: themeRow.width + 12; height: 24
                    radius: th.radiusSmall
                    color: themeArea.containsMouse ? th.accentLapis : "transparent"
                    border.width: 1
                    border.color: th.accentLapis

                    Row {
                        id: themeRow
                        anchors.centerIn: parent
                        spacing: 4
                        Label { text: previewWindow.isDarkTheme ? "\u263E" : "\u2600"; font.pixelSize: 11; color: th.accentLapis; anchors.verticalCenter: parent.verticalCenter }
                        Label {
                            text: previewWindow.isDarkTheme ? (translator ? translator.translate("theme_light") : "\u0627\u0644\u0636\u0648\u0621 \u0627\u0644\u0645\u0636\u064A\u0621") : (translator ? translator.translate("theme_dark") : "\u0627\u0644\u0636\u0648\u0621 \u0627\u0644\u0645\u0638\u0644\u0645")
                            font.family: th.fontFamilyUI; font.pixelSize: 10; color: th.accentLapis; anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    MouseArea {
                        id: themeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: previewWindow.isDarkTheme = !previewWindow.isDarkTheme
                    }
                }

                Rectangle {
                    width: 28; height: 24; radius: 4
                    color: closeArea.containsMouse ? "#EF5350" : "transparent"
                    border.color: "#EF5350"; border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: "\u2715"
                        color: "#EF5350"
                        font.pixelSize: 10; font.bold: true
                    }

                    MouseArea {
                        id: closeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: previewWindow.visible = false
                    }
                }
            }
        }

        Rectangle {
            id: previewContainer
            anchors.top: parent.top
            anchors.topMargin: 36
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 12
            color: "transparent"
            clip: true

            property var model: null

            Repeater {
                model: previewContainer.model

                Rectangle {
                    x: model.widget ? model.widget.x : 0
                    y: model.widget ? model.widget.y : 0
                    width: model.widget ? model.widget.width : 100
                    height: model.widget ? model.widget.height : 40
                    radius: model.widget && model.widget.borderRadius ? model.widget.borderRadius : 4
                    color: {
                        if (!model.widget) return "transparent"
                        return model.widget.color ? model.widget.color : "#14B8A6"
                    }
                    opacity: model.widget ? (model.widget.opacity !== undefined ? model.widget.opacity : 1.0) : 1.0
                    visible: model.widget ? (model.widget.visible !== undefined ? model.widget.visible : true) : true
                    enabled: model.widget ? (model.widget.enabled !== undefined ? model.widget.enabled : true) : true

                    border.color: previewWindow.isDarkTheme ? "#3A3A3D" : "#E2E8F0"
                    border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: model.widget ? model.widget.text : ""
                        color: "white"
                        font.family: th.fontFamilyUI
                        font.pixelSize: model.widget ? model.widget.fontSize : 13
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            previewToast.text = model.widget ? model.widget.variableName + " clicked" : ""
                            previewToast.visible = true
                            hideToast.start()
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                text: previewContainer.model && previewContainer.model.count > 0 ? "" : (translator ? translator.translate("empty_preview") : "\u0644\u0645 \u064A\u062A\u0645 \u0625\u0636\u0627\u0641\u0629 \u0639\u0646\u0627\u0635\u0631 \u0644\u0644\u0645\u0639\u0627\u064A\u0646\u0629")
                color: previewWindow.isDarkTheme ? "#64748B" : "#94A3B8"
                font.family: th.fontFamilyUI
                font.pixelSize: 14
                visible: !previewContainer.model || previewContainer.model.count === 0
            }

            Rectangle {
                id: previewToast
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 20
                width: toastLabel.implicitWidth + 24
                height: 30
                radius: 6
                color: "#14B8A6"
                visible: false
                property alias text: toastLabel.text

                Label {
                    id: toastLabel
                    anchors.centerIn: parent
                    color: "white"
                    font.family: th.fontFamilyUI
                    font.pixelSize: 11
                }

                Timer {
                    id: hideToast
                    interval: 2000
                    onTriggered: previewToast.visible = false
                }
            }
        }
    }
}

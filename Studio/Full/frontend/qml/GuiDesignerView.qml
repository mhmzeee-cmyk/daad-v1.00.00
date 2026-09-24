import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DaadStudio 1.0

Rectangle {
    id: designerView
    function loc(key) { return translator ? translator.translate(key) : key }
    color: th.backgroundPaper
    visible: false

    signal codeChanged(string code)
    function getCode() { return guiModel.generatedCode + eventsPanel.generateEventsCode() }

    GuiDesignerModel {
        id: guiModel
        onGeneratedCodeChanged: designerView.codeChanged(generatedCode + eventsPanel.generateEventsCode())
    }

    property alias designerModel: guiModel

    LivePreviewWindow {
        id: livePreview
    }

    Rectangle {
        anchors.fill: parent
        color: th.backgroundPaper

        RowLayout {
            anchors.fill: parent
            anchors.margins: 4
            spacing: 4

            PropertyInspector {
                id: propInspector
                Layout.preferredWidth: 220
                Layout.fillHeight: true
                designerModel: guiModel
                onInjectEventCode: function(code, functionName) {
                    designerView.codeChanged(code)
                    toast.show(loc("event_injected") + ": " + functionName, "success", 3000)
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: th.backgroundCode
                radius: th.radiusMedium
                border.width: 1
                border.color: th.borderStone

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 4
                    spacing: 4

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 32
                        color: th.backgroundToolbar
                        radius: th.radiusSmall

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8

                            Label {
                                text: "\u{1F3A8} " + loc("design_canvas")
                                color: th.textInk
                                font.family: th.fontFamilyUI
                                font.pixelSize: th.fontSizeSmall
                                font.bold: true
                            }

                            Item { Layout.fillWidth: true }

                            Rectangle {
                                width: previewRow.width + 12; height: 24
                                radius: th.radiusSmall
                                color: previewArea.containsMouse ? "#14B8A6" : "transparent"
                                border.width: 1
                                border.color: "#14B8A6"

                                Row {
                                    id: previewRow
                                    anchors.centerIn: parent
                                    spacing: 4
                                    Label { text: "\u{1F441}"; font.pixelSize: 10; color: "#14B8A6"; anchors.verticalCenter: parent.verticalCenter }
                                    Label { text: loc("live_preview"); font.family: th.fontFamilyUI; font.pixelSize: 9; color: "#14B8A6"; anchors.verticalCenter: parent.verticalCenter }
                                }

                                MouseArea {
                                    id: previewArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: livePreview.loadPreview(guiModel)
                                }
                            }

                            Rectangle {
                                width: themeToggleRow.width + 12; height: 24
                                radius: th.radiusSmall
                                color: themeToggleArea.containsMouse ? "#C792EA" : "transparent"
                                border.width: 1
                                border.color: "#C792EA"

                                Row {
                                    id: themeToggleRow
                                    anchors.centerIn: parent
                                    spacing: 4
                                    Label { text: designCanvas.previewDarkMode ? "\u263E" : "\u2600"; font.pixelSize: 10; color: "#C792EA"; anchors.verticalCenter: parent.verticalCenter }
                                    Label { text: designCanvas.previewDarkMode ? loc("theme_light") : loc("theme_dark"); font.family: th.fontFamilyUI; font.pixelSize: 9; color: "#C792EA"; anchors.verticalCenter: parent.verticalCenter }
                                }

                                MouseArea {
                                    id: themeToggleArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: designCanvas.previewDarkMode = !designCanvas.previewDarkMode
                                }
                            }

                            Rectangle {
                                width: snapRow.width + 12; height: 24
                                radius: th.radiusSmall
                                color: snapArea.containsMouse ? th.accentLapis : "transparent"
                                border.width: 1
                                border.color: th.accentLapis

                                Row {
                                    id: snapRow
                                    anchors.centerIn: parent
                                    spacing: 4
                                    Label { text: "\u25A1"; font.pixelSize: 10; color: th.accentLapis; anchors.verticalCenter: parent.verticalCenter }
                                    Label { text: "\u062A\u0634\u0628\u064A\u0643"; font.family: th.fontFamilyUI; font.pixelSize: 9; color: th.accentLapis; anchors.verticalCenter: parent.verticalCenter }
                                }

                                MouseArea {
                                    id: snapArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: designCanvas.snapEnabled = !designCanvas.snapEnabled
                                }
                            }

                            Rectangle {
                                width: clearRow.width + 12; height: 24
                                radius: th.radiusSmall
                                color: clearArea.containsMouse ? th.semanticError : "transparent"
                                border.width: 1
                                border.color: th.semanticError

                                Row {
                                    id: clearRow
                                    anchors.centerIn: parent
                                    spacing: 4
                                    Label { text: "\u2715"; font.pixelSize: 10; color: th.semanticError; anchors.verticalCenter: parent.verticalCenter }
                                    Label { text: loc("clear_canvas"); font.family: th.fontFamilyUI; font.pixelSize: 10; color: th.semanticError; anchors.verticalCenter: parent.verticalCenter }
                                }

                                MouseArea {
                                    id: clearArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: guiModel.clear()
                                }
                            }

                            Rectangle {
                                width: exportRow.width + 12; height: 24
                                radius: th.radiusSmall
                                color: exportArea.containsMouse ? th.accentLapis : th.accentLapisBg
                                border.width: 1
                                border.color: th.accentLapis

                                Row {
                                    id: exportRow
                                    anchors.centerIn: parent
                                    spacing: 4
                                    Label { text: "\u21E7"; font.pixelSize: 12; color: exportArea.containsMouse ? th.textInverse : th.accentLapis; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                                    Label { text: loc("export_to_code"); font.family: th.fontFamilyUI; font.pixelSize: 10; color: exportArea.containsMouse ? th.textInverse : th.accentLapis; anchors.verticalCenter: parent.verticalCenter }
                                }

                                MouseArea {
                                    id: exportArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        designerView.codeChanged(guiModel.generatedCode + eventsPanel.generateEventsCode())
                                        toast.show(loc("code_exported"), "success", 3000)
                                    }
                                }
                            }
                        }
                    }

                    DesignCanvas {
                        id: designCanvas
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        designerModel: guiModel
                    }
                }
            }

            ColumnLayout {
                Layout.preferredWidth: 280
                Layout.fillHeight: true
                spacing: 0

                EventsPanel {
                    id: eventsPanel
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    designerModel: guiModel
                }
            }

            WidgetPalette {
                id: widgetPalette
                Layout.preferredWidth: 190
                Layout.fillHeight: true
                onWidgetSelected: function(type) {
                    var cx = Math.max(40, Math.min(300, designCanvas.width / 2 - 60))
                    var cy = Math.max(40, Math.min(200, designCanvas.height / 2 - 20))
                    var idx = guiModel.addWidget(type, cx, cy)
                    guiModel.selectedIndex = idx
                }
            }
        }
    }

    Rectangle {
        id: dragOverlay
        anchors.fill: parent
        z: 9999
        visible: widgetPalette.isDragging
        color: "transparent"

        MouseArea {
            id: dragOverlayArea
            anchors.fill: parent
            cursorShape: Qt.DragCopyCursor

            onPositionChanged: function(mouse) {
                var gpos = mapToGlobal(Qt.point(mouse.x, mouse.y))
                widgetPalette.dragGlobalX = gpos.x
                widgetPalette.dragGlobalY = gpos.y
            }

            onReleased: function(mouse) {
                var globalPos = mapToGlobal(Qt.point(mouse.x, mouse.y))
                var wType = widgetPalette.dragWidgetType
                widgetPalette.finishDrag()

                if (wType.length === 0) return

                var localPos = designCanvas.mapFromGlobal(Qt.point(globalPos.x, globalPos.y))
                var inBounds = localPos.x >= 0 && localPos.y >= 0 && localPos.x <= designCanvas.width && localPos.y <= designCanvas.height

                var fx, fy
                if (inBounds) {
                    fx = designCanvas.snapToGrid(localPos.x - 60)
                    fy = designCanvas.snapToGrid(localPos.y - 17)
                } else {
                    fx = Math.max(40, designCanvas.width / 2 - 60)
                    fy = Math.max(40, designCanvas.height / 2 - 17)
                }
                fx = Math.max(0, Math.min(fx, designCanvas.width - 120))
                fy = Math.max(0, Math.min(fy, designCanvas.height - 34))
                var idx = guiModel.addWidget(wType, fx, fy)
                guiModel.selectedIndex = idx
            }

            onCanceled: {
                widgetPalette.finishDrag()
            }
        }

        Rectangle {
            id: dragGhost
            width: 120
            height: 34
            color: th.withAlpha(th.accentLapis, 0.25)
            border.color: th.accentLapis
            border.width: 1.5
            radius: 4
            visible: widgetPalette.isDragging

            property var localPt: dragOverlay.mapFromGlobal(Qt.point(widgetPalette.dragGlobalX, widgetPalette.dragGlobalY))
            x: localPt.x - 60
            y: localPt.y - 17

            Row {
                anchors.centerIn: parent
                spacing: 6
                Label {
                    text: "\u2795"
                    font.pixelSize: 10
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    text: widgetPalette.dragWidgetType
                    color: th.textInk
                    font.family: th.fontFamilyUI
                    font.pixelSize: 11
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

}

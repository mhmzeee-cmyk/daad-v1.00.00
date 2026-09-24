import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: paletteRoot
    color: th.backgroundSidebar

    signal widgetSelected(string widgetType)
    signal widgetDropped(string widgetType, real globalX, real globalY)

    property bool isDragging: false
    property string dragWidgetType: ""
    property real dragGlobalX: 0
    property real dragGlobalY: 0
    readonly property int dragThreshold: 5

    property var widgets: [
        { type: "\u0632\u0631",              label: "\u0632\u0631 \u0623\u0645\u0631",        icon: "\u25A0", cat: "\u0625\u0630\u0631\u0627\u0639\u064A\u0629" },
        { type: "\u062D\u0642\u0644_\u0646\u0635",     label: "\u062D\u0642\u0644 \u0646\u0635",        icon: "\u2500", cat: "\u0625\u0630\u0631\u0627\u0639\u064A\u0629" },
        { type: "\u062A\u0633\u0645\u064A\u0629",      label: "\u062A\u0633\u0645\u064A\u0629",           icon: "Aa",    cat: "\u0625\u0630\u0631\u0627\u0639\u064A\u0629" },
        { type: "\u0645\u0628\u0631\u0632_\u0627\u062E\u062A\u064A\u0627\u0631", label: "\u0645\u0628\u0631\u0632 \u0627\u062E\u062A\u064A\u0627\u0631", icon: "\u2610", cat: "\u0625\u0630\u0631\u0627\u0639\u064A\u0629" },

        { type: "\u0642\u0627\u0626\u0645\u0629_\u062E\u064A\u0627\u0631\u0627\u062A", label: "\u0642\u0627\u0626\u0645\u0629 \u062E\u064A\u0627\u0631\u0627\u062A", icon: "\u2630", cat: "\u0627\u0644\u0642\u0648\u0627\u0626\u0645" },
        { type: "\u0642\u0627\u0626\u0645\u0629_\u0645\u0646\u0633\u062F\u0644\u0629", label: "\u0642\u0627\u0626\u0645\u0629 \u0645\u0646\u0633\u062F\u0644\u0629", icon: "\u25BC", cat: "\u0627\u0644\u0642\u0648\u0627\u0626\u0645" },
        { type: "\u0634\u0631\u0637_\u062A\u0645\u0631\u064A\u0631", label: "\u0634\u0631\u0637 \u062A\u0645\u0631\u064A\u0631", icon: "\u2500", cat: "\u0627\u0644\u0642\u0648\u0627\u0626\u0645" },
        { type: "\u0634\u0631\u0637_\u062A\u0644\u0648\u064A\u0646", label: "\u0634\u0631\u0637 \u062A\u0644\u0648\u064A\u0646", icon: "\u2588", cat: "\u0627\u0644\u0642\u0648\u0627\u0626\u0645" },
        { type: "\u0639\u0644\u0627\u0645\u0629_\u062A\u0628\u0648\u064A\u0628", label: "\u0639\u0644\u0627\u0645\u0629 \u062A\u0628\u0648\u064A\u0628", icon: "\u2587", cat: "\u0627\u0644\u0642\u0648\u0627\u0626\u0645" },

        { type: "\u0635\u0648\u0631\u0629",       label: "\u0635\u0648\u0631\u0629",        icon: "\u25A3", cat: "\u0627\u0644\u062A\u0646\u0633\u064A\u0642" },
        { type: "\u0633\u062D\u0631",              label: "\u0633\u062D\u0631",       icon: "\u2B1C", cat: "\u0627\u0644\u062A\u0646\u0633\u064A\u0642" },

        { type: "\u0639\u0645\u0648\u062F",       label: "\u0639\u0645\u0648\u062F",        icon: "\u2225", cat: "\u0627\u0644\u062A\u062F\u0631\u064A\u0628" },
        { type: "\u0635\u0641",                    label: "\u0635\u0641",           icon: "\u2501", cat: "\u0627\u0644\u062A\u062F\u0631\u064A\u0628" },
        { type: "\u0634\u0628\u0643\u0629",        label: "\u0634\u0628\u0643\u0629", icon: "\u254B", cat: "\u0627\u0644\u062A\u062F\u0631\u064A\u0628" },
        { type: "\u0645\u062C\u0645\u0648\u0639\u0629",  label: "\u0645\u062C\u0645\u0648\u0639\u0629", icon: "\u2B1A", cat: "\u0627\u0644\u062A\u062F\u0631\u064A\u0628" }
    ]

    function buildCategoryList() {
        var result = []
        var lastCat = ""
        for (var i = 0; i < widgets.length; i++) {
            if (widgets[i].cat !== lastCat) {
                lastCat = widgets[i].cat
                result.push({ label: lastCat, isHeader: true })
            }
            result.push({ type: widgets[i].type, label: widgets[i].label, icon: widgets[i].icon, isHeader: false })
        }
        return result
    }

    function startDrag(type, globalX, globalY) {
        dragWidgetType = type
        dragGlobalX = globalX
        dragGlobalY = globalY
        isDragging = true
    }

    function finishDrag() {
        isDragging = false
        dragWidgetType = ""
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 0

        Text {
            text: "\u{1F3DB} \u0623\u062F\u0648\u0627\u062A \u0627\u0644\u0648\u0627\u062C\u0647\u0629"
            color: th.textInk
            font.family: th.fontFamilyUI
            font.pixelSize: 12
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 6
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: th.borderStone
            Layout.bottomMargin: 6
        }

        Flickable {
            id: flickable
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: width
            contentHeight: contentColumn.implicitHeight
            clip: true
            interactive: !paletteRoot.isDragging
            boundsBehavior: Flickable.StopAtBounds
            flickableDirection: Flickable.VerticalFlick

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
                interactive: !paletteRoot.isDragging
            }

            Column {
                id: contentColumn
                width: flickable.width
                spacing: 2

                Repeater {
                    model: paletteRoot.buildCategoryList()

                    delegate: Column {
                        width: contentColumn.width

                        Rectangle {
                            visible: modelData.isHeader === true
                            width: parent.width
                            height: 22
                            color: th.backgroundHover
                            radius: 3

                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter
                                text: modelData.label
                                color: th.textMuted
                                font.family: th.fontFamilyUI
                                font.pixelSize: 9
                                font.bold: true
                            }
                        }

                        Rectangle {
                            visible: modelData.isHeader !== true
                            width: parent.width
                            height: 34
                            radius: 4
                            color: itemArea.containsMouse ? th.backgroundHover : "transparent"

                            Row {
                                anchors.fill: parent
                                anchors.leftMargin: 8
                                anchors.rightMargin: 8
                                spacing: 8

                                Rectangle {
                                    width: 20
                                    height: 20
                                    radius: 4
                                    color: th.accentLapisBg
                                    anchors.verticalCenter: parent.verticalCenter

                                    Text {
                                        anchors.centerIn: parent
                                        text: modelData.isHeader ? "" : (modelData.icon || "")
                                        font.pixelSize: 10
                                        color: th.accentLapis
                                    }
                                }

                                Text {
                                    text: modelData.label
                                    color: th.textInk
                                    font.family: th.fontFamilyUI
                                    font.pixelSize: 11
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }

                            MouseArea {
                                id: itemArea
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                preventStealing: true

                                property real pressX: 0
                                property real pressY: 0

                                onPressed: function(mouse) {
                                    if (modelData.isHeader) return
                                    pressX = mouse.x
                                    pressY = mouse.y
                                }

                                onPositionChanged: function(mouse) {
                                    if (!pressed || modelData.isHeader) return
                                    if (!paletteRoot.isDragging) {
                                        var dx = mouse.x - pressX
                                        var dy = mouse.y - pressY
                                        if (Math.abs(dx) > paletteRoot.dragThreshold || Math.abs(dy) > paletteRoot.dragThreshold) {
                                            var wType = modelData.type || ""
                                            if (wType.length === 0) return
                                            var gpos = mapToGlobal(Qt.point(mouse.x, mouse.y))
                                            paletteRoot.startDrag(wType, gpos.x, gpos.y)
                                        }
                                    } else {
                                        var gpos2 = mapToGlobal(Qt.point(mouse.x, mouse.y))
                                        paletteRoot.dragGlobalX = gpos2.x
                                        paletteRoot.dragGlobalY = gpos2.y
                                    }
                                }

                                onReleased: function(mouse) {
                                    if (modelData.isHeader) return
                                    var wType = modelData.type || ""
                                    if (wType.length === 0) return

                                    if (!paletteRoot.isDragging) {
                                        paletteRoot.widgetSelected(wType)
                                    }
                                }

                                onCanceled: {
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

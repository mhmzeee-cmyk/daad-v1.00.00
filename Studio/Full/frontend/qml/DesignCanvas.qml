import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: canvas
    color: previewDarkMode ? "#1A1A1C" : th.backgroundCode

    required property var designerModel

    property int gridSize: 20
    property bool snapEnabled: true
    property int selectedIndex: designerModel ? designerModel.selectedIndex : -1
    property real magnetThreshold: 10
    property bool previewDarkMode: true

    property var guideLines: []
    property real snapX: -1
    property real snapY: -1

    function snapToGrid(val) { return snapEnabled ? Math.round(val / gridSize) * gridSize : val }

    function computeMagneticGuides(dragIdx, dragX, dragY, dragW, dragH) {
        var lines = []
        var sx = -1
        var sy = -1
        if (!designerModel) return { lines: lines, sx: sx, sy: sy }

        var dragCX = dragX + dragW / 2
        var dragCY = dragY + dragH / 2
        var dragR = dragX + dragW
        var dragB = dragY + dragH

        for (var i = 0; i < designerModel.count; i++) {
            if (i === dragIdx) continue
            var w = designerModel.getWidget(i)
            if (!w) continue

            var wCX = w.x + w.width / 2
            var wCY = w.y + w.height / 2
            var wR = w.x + w.width
            var wB = w.y + w.height

            if (Math.abs(dragX - w.x) < magnetThreshold) { lines.push({x1: w.x, y1: Math.min(dragY, w.y) - 10, x2: w.x, y2: Math.max(dragB, wB) + 10}); sx = w.x }
            if (Math.abs(dragR - wR) < magnetThreshold) { lines.push({x1: wR, y1: Math.min(dragY, w.y) - 10, x2: wR, y2: Math.max(dragB, wB) + 10}); sx = wR - dragW }
            if (Math.abs(dragCX - wCX) < magnetThreshold) { lines.push({x1: wCX, y1: Math.min(dragY, w.y) - 10, x2: wCX, y2: Math.max(dragB, wB) + 10}); sx = wCX - dragW / 2 }
            if (Math.abs(dragY - w.y) < magnetThreshold) { lines.push({x1: Math.min(dragX, w.x) - 10, y1: w.y, x2: Math.max(dragR, wR) + 10, y2: w.y}); sy = w.y }
            if (Math.abs(dragB - wB) < magnetThreshold) { lines.push({x1: Math.min(dragX, w.x) - 10, y1: wB, x2: Math.max(dragR, wR) + 10, y2: wB}); sy = wB - dragH }
            if (Math.abs(dragCY - wCY) < magnetThreshold) { lines.push({x1: Math.min(dragX, w.x) - 10, y1: wCY, x2: Math.max(dragR, wR) + 10, y2: wCY}); sy = wCY - dragH / 2 }
        }

        var canvasCX = canvas.width / 2
        var canvasCY = canvas.height / 2
        if (Math.abs(dragCX - canvasCX) < magnetThreshold) { lines.push({x1: canvasCX, y1: 0, x2: canvasCX, y2: canvas.height}); sx = canvasCX - dragW / 2 }
        if (Math.abs(dragCY - canvasCY) < magnetThreshold) { lines.push({x1: 0, y1: canvasCY, x2: canvas.width, y2: canvasCY}); sy = canvasCY - dragH / 2 }

        return { lines: lines, sx: sx, sy: sy }
    }

    Canvas {
        anchors.fill: parent
        z: -1
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            ctx.strokeStyle = Qt.rgba(0.23, 0.23, 0.24, 1)
            ctx.lineWidth = 0.5
            for (var gx = gridSize; gx < width; gx += gridSize) {
                ctx.beginPath()
                ctx.moveTo(gx, 0)
                ctx.lineTo(gx, height)
                ctx.stroke()
            }
            for (var gy = gridSize; gy < height; gy += gridSize) {
                ctx.beginPath()
                ctx.moveTo(0, gy)
                ctx.lineTo(width, gy)
                ctx.stroke()
            }

            ctx.strokeStyle = "#14B8A6"
            ctx.lineWidth = 1
            ctx.setLineDash([4, 4])
            for (var g = 0; g < guideLines.length; g++) {
                var gl = guideLines[g]
                ctx.beginPath()
                ctx.moveTo(gl.x1, gl.y1)
                ctx.lineTo(gl.x2, gl.y2)
                ctx.stroke()
            }
            ctx.setLineDash([])
        }

        Connections {
            target: canvas
            function onWidthChanged() { canvas.requestPaint() }
            function onHeightChanged() { canvas.requestPaint() }
            function onGuideLinesChanged() { canvas.requestPaint() }
        }
    }

    Repeater {
        id: widgetRepeater
        model: designerModel
        delegate: Item {
            id: widgetDelegate
            z: 2
            x: widgetData ? widgetData.x : 0
            y: widgetData ? widgetData.y : 0
            width: widgetData ? widgetData.width : 100
            height: widgetData ? widgetData.height : 40
            visible: widgetData ? true : false

            property var widgetData: model.widget

            Rectangle {
                id: widgetVisual
                anchors.fill: parent
                radius: widgetDelegate.widgetData && widgetDelegate.widgetData.type === "\u0632\u0631" ? th.radiusSmall : 2
                color: {
                    if (!widgetDelegate.widgetData) return "transparent"
                    var sel = (index === canvas.selectedIndex)
                    if (sel) return th.withAlpha(widgetDelegate.widgetData.color, 0.35)
                    return th.withAlpha(widgetDelegate.widgetData.color, 0.55)
                }
                border.width: index === canvas.selectedIndex ? 2 : 1
                border.color: index === canvas.selectedIndex ? widgetDelegate.widgetData.color : th.textMuted

                Label {
                    anchors.centerIn: parent
                    text: widgetDelegate.widgetData ? widgetDelegate.widgetData.text : ""
                    color: canvas.previewDarkMode ? "#F1F5F9" : "#1E293B"
                    font.family: th.fontFamilyUI
                    font.pixelSize: widgetDelegate.widgetData ? widgetDelegate.widgetData.fontSize : 13
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.SizeAllCursor
                    preventStealing: true
                    drag.target: widgetDelegate

                    property real pressX: 0
                    property real pressY: 0

                    onPressed: function(mouse) {
                        designerModel.selectedIndex = index
                        pressX = mouse.x
                        pressY = mouse.y
                        raise()
                    }

                    onPositionChanged: function(mouse) {
                        if (!pressed || !widgetDelegate.widgetData) return
                        var rawX = widgetDelegate.x + mouse.x - pressX
                        var rawY = widgetDelegate.y + mouse.y - pressY
                        var bx = Math.max(0, Math.min(rawX, canvas.width - widgetDelegate.width))
                        var by = Math.max(0, Math.min(rawY, canvas.height - widgetDelegate.height))

                        var result = canvas.computeMagneticGuides(index, bx, by, widgetDelegate.width, widgetDelegate.height)
                        canvas.guideLines = result.lines

                        var fx = result.sx >= 0 ? result.sx : (canvas.snapEnabled ? canvas.snapToGrid(bx) : bx)
                        var fy = result.sy >= 0 ? result.sy : (canvas.snapEnabled ? canvas.snapToGrid(by) : by)
                        fx = Math.max(0, Math.min(fx, canvas.width - widgetDelegate.width))
                        fy = Math.max(0, Math.min(fy, canvas.height - widgetDelegate.height))

                        if (fx !== widgetDelegate.x || fy !== widgetDelegate.y) {
                            designerModel.updateWidgetProperty(index, "x", fx)
                            designerModel.updateWidgetProperty(index, "y", fy)
                        }
                    }

                    onReleased: {
                        canvas.guideLines = []
                        canvas.snapX = -1
                        canvas.snapY = -1
                        designerModel.updateWidgetProperty(index, "x", widgetDelegate.x)
                        designerModel.updateWidgetProperty(index, "y", widgetDelegate.y)
                    }
                }

                Rectangle {
                    width: 10; height: 10
                    radius: 5
                    color: widgetDelegate.widgetData ? widgetDelegate.widgetData.color : th.accentLapis
                    visible: index === canvas.selectedIndex
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.margins: -5

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.SizeFDiagCursor
                        preventStealing: true

                        property real startX: 0
                        property real startY: 0
                        property real startW: 0
                        property real startH: 0

                        onPressed: function(mouse) {
                            startX = mouseX
                            startY = mouseY
                            startW = widgetDelegate.width
                            startH = widgetDelegate.height
                        }

                        onPositionChanged: function(mouse) {
                            if (!pressed || !widgetDelegate.widgetData) return
                            var dw = mouseX - startX
                            var dh = mouseY - startY
                            var nw = Math.max(40, canvas.snapToGrid(startW + dw))
                            var nh = Math.max(20, canvas.snapToGrid(startH + dh))
                            designerModel.updateWidgetProperty(index, "width", nw)
                            designerModel.updateWidgetProperty(index, "height", nh)
                        }
                    }
                }
            }
        }
    }

    Keys.onDeletePressed: {
        if (selectedIndex >= 0 && designerModel) {
            designerModel.removeWidget(selectedIndex)
        }
    }

    DropArea {
        anchors.fill: parent
        z: 1
        keys: ["daad/widget"]

        onDropped: function(drop) {
            if (!designerModel) return
            var wt = drop.text !== undefined ? drop.text : ""
            if (wt.length === 0) return

            var fx = canvas.snapToGrid(drop.x - 60)
            var fy = canvas.snapToGrid(drop.y - 20)
            fx = Math.max(0, Math.min(fx, canvas.width - 60))
            fy = Math.max(0, Math.min(fy, canvas.height - 40))
            var idx = designerModel.addWidget(wt, fx, fy)
            designerModel.selectedIndex = idx
        }

        onEntered: {
            canvas.forceActiveFocus()
        }
    }

    MouseArea {
        anchors.fill: parent
        z: -1
        onClicked: {
            if (designerModel) designerModel.selectedIndex = -1
            canvas.forceActiveFocus()
        }
    }
}

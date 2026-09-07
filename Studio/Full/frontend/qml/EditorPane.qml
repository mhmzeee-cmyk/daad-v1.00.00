import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0
import DaadStudio 1.0

Pane {
    id: editorPane
    padding: 0

    signal codeChanged(string newCode)
    function loc(key) { var _ = translator.languageVersion; return translator ? translator.translate(key) : key }
    function getCode() { return codeEditor.text }
    function updateCode(code) { codeEditor.text = code }
    function clearCode() { codeEditor.text = "" }

    function getLine(lineNumber) {
        var lines = codeEditor.text.split("\n")
        if (lineNumber < 1 || lineNumber > lines.length) return ""
        return lines[lineNumber - 1]
    }

    function replaceLine(lineNumber, newLineText) {
        var lines = codeEditor.text.split("\n")
        if (lineNumber < 1 || lineNumber > lines.length) return
        lines[lineNumber - 1] = newLineText
        codeEditor.text = lines.join("\n")
    }

    DaadHighlighter {
        id: daadHighlighter
        Component.onCompleted: setDocumentFromQml(codeEditor.textDocument)
    }

    property var daadKeywords: [
        { name: "\u0635\u062D\u064A\u062D",   type: "type" },
        { name: "\u0639\u0634\u0631\u064A",   type: "type" },
        { name: "\u0645\u0646\u0637\u0642\u064A",  type: "type" },
        { name: "\u062D\u0631\u0641",    type: "type" },
        { name: "\u0646\u0635",     type: "type" },
        { name: "\u062B\u0627\u0628\u062A",   type: "modifier" },
        { name: "\u0641\u0631\u0627\u063A",   type: "type" },
        { name: "\u062A\u0644\u0642\u0627\u0626\u064A", type: "type" },
        { name: "\u0635\u0648\u0627\u0628",   type: "value" },
        { name: "\u062E\u0637\u0623",    type: "value" },
        { name: "\u0639\u062F\u0645",    type: "value" },
        { name: "\u0639\u0631\u0651\u0641",   type: "type" },
        { name: "\u0625\u0630\u0627",    type: "keyword" },
        { name: "\u0648\u0625\u0644\u0627",   type: "keyword" },
        { name: "\u0637\u0627\u0644\u0645\u0627",  type: "keyword" },
        { name: "\u0627\u0641\u0639\u0644",   type: "keyword" },
        { name: "\u0644\u0643\u0644",    type: "keyword" },
        { name: "\u0641\u064A",     type: "keyword" },
        { name: "\u0627\u062E\u062A\u0631",   type: "keyword" },
        { name: "\u062D\u0627\u0644\u0629",   type: "keyword" },
        { name: "\u0627\u0641\u062A\u0631\u0627\u0636\u064A", type: "keyword" },
        { name: "\u062A\u0648\u0642\u0641",   type: "keyword" },
        { name: "\u0627\u0633\u062A\u0645\u0631",  type: "keyword" },
        { name: "\u0627\u0646\u062A\u0642\u0644",  type: "keyword" },
        { name: "\u0635\u0646\u0641",    type: "keyword" },
        { name: "\u0647\u064A\u062E\u0644",   type: "keyword" },
        { name: "\u062A\u0639\u062F\u0627\u062F",  type: "keyword" },
        { name: "\u0648\u0627\u062C\u0647\u0629",  type: "keyword" },
        { name: "\u0646\u0637\u0642",   type: "keyword" },
        { name: "\u0639\u0627\u0645",    type: "modifier" },
        { name: "\u062E\u0627\u0635",    type: "modifier" },
        { name: "\u0645\u062D\u0645\u064A",   type: "modifier" },
        { name: "\u064A\u0631\u062B",    type: "keyword" },
        { name: "\u0630\u0627\u062A\u064A",   type: "keyword" },
        { name: "\u0627\u0644\u0623\u0635\u0644",  type: "keyword" },
        { name: "\u0645\u062C\u0631\u0651\u062F",  type: "keyword" },
        { name: "\u062F\u0627\u0644\u0629",   type: "function" },
        { name: "\u0627\u0631\u062C\u0639",   type: "keyword" },
        { name: "\u062C\u062F\u064A\u062F",   type: "keyword" },
        { name: "\u0627\u062D\u0630\u0641",   type: "keyword" },
        { name: "\u0645\u0648\u0634\u0631",   type: "type" },
        { name: "\u0645\u0631\u062C\u0639",   type: "type" },
        { name: "\u0633\u0627\u0643\u0646",   type: "modifier" },
        { name: "\u0645\u0636\u0645\u0646",   type: "modifier" },
        { name: "\u062E\u0627\u0631\u062C\u064A",  type: "modifier" },
        { name: "\u0642\u0627\u0644\u0628",   type: "keyword" },
        { name: "\u062D\u0627\u0648\u0644",   type: "keyword" },
        { name: "\u0627\u0645\u0633\u0643",   type: "keyword" },
        { name: "\u0623\u062E\u064A\u0631\u0627\u064B", type: "keyword" },
        { name: "\u0627\u0631\u0645\u0650",   type: "keyword" },
        { name: "\u062A\u0632\u0627\u0645\u0646",  type: "keyword" },
        { name: "\u0627\u0646\u062A\u0638\u0631",  type: "keyword" },
        { name: "\u062E\u064A\u0637",    type: "keyword" },
        { name: "\u0627\u062D\u062C\u0632",   type: "keyword" },
        { name: "\u0645\u0634\u062A\u0631\u0643",  type: "type" },
        { name: "\u0641\u0631\u064A\u062F",   type: "type" },
        { name: "\u0627\u0633\u062A\u0648\u0631\u062F", type: "keyword" },
        { name: "\u0635\u062F\u0651\u0631",   type: "keyword" },
        { name: "\u0648\u062D\u062F\u0629",   type: "keyword" },
        { name: "\u0628\u062F\u064A\u0644",   type: "keyword" },
        { name: "\u0632\u0631_\u0623\u0645\u0631",  type: "ui" },
        { name: "\u062D\u0642\u0644_\u0646\u0635",  type: "ui" },
        { name: "\u0642\u0627\u0626\u0645\u0629_\u062E\u0635\u0627\u0626\u0631",  type: "ui" },
        { name: "\u0635\u0648\u0631\u0629",   type: "ui" },
        { name: "\u0645\u0628\u0631\u0637_\u0627\u062E\u062A\u064A\u0627\u0631",  type: "ui" },
        { name: "\u0634\u0631\u064A\u0637_\u062A\u0645\u0631\u064A\u0631",  type: "ui" },
        { name: "\u0642\u0627\u0626\u0645\u0629_\u0645\u0646\u0633\u062F\u0644\u0629",  type: "ui" },
        { name: "\u0644\u0648\u062D\u0629",   type: "ui" },
        { name: "\u062A\u0633\u0645\u064A\u0629",   type: "ui" },
        { name: "\u0639\u0645\u0648\u062F",   type: "ui" },
        { name: "\u0635\u0641",    type: "ui" },
        { name: "\u0634\u0628\u0643\u0629",   type: "ui" },
        { name: "\u0634\u0631\u064A\u0637_\u062A\u0644\u0648\u064A\u0646",  type: "ui" },
        { name: "\u0639\u0644\u0627\u0645\u0629_\u062A\u0628\u0648\u064A\u0628",  type: "ui" },
        { name: "\u0645\u0635\u0641\u0648\u0641\u0629",   type: "type" },
        { name: "\u0642\u0627\u0626\u0645\u0629",    type: "type" },
        { name: "\u0642\u0627\u0645\u0648\u0633",    type: "type" },
        { name: "\u0645\u0643\u062F\u0633",     type: "type" },
        { name: "\u0637\u0627\u0628\u0648\u0631",    type: "type" },
        { name: "\u0634\u062C\u0631\u0629",     type: "type" },
        { name: "\u0631\u0633\u0645_\u0628\u064A\u0627\u0646\u064A", type: "type" },
        { name: "\u062C\u062F\u0648\u0644",     type: "type" },
        { name: "\u0637\u0628\u0627\u0639\u0629",         type: "function" },
        { name: "\u0627\u0642\u0631\u0623_\u0633\u0637\u0631",    type: "function" },
        { name: "\u0627\u0643\u062A\u0628_\u0633\u0637\u0631",   type: "function" },
        { name: "\u0627\u0641\u062A\u062D",          type: "function" },
        { name: "\u063A\u0644\u0642",           type: "function" },
        { name: "\u0627\u0645\u0633\u062D_\u0627\u0644\u0634\u0627\u0634\u0629",   type: "function" },
        { name: "\u062D\u0630\u0641",           type: "function" },
        { name: "\u0646\u0633\u062E",           type: "function" },
        { name: "\u0625\u0639\u0627\u062F\u0629_\u062A\u0633\u0645\u064A\u0629",   type: "function" },
        { name: "\u062D\u062C\u0645_\u0627\u0644\u0645\u0644\u0641",     type: "function" },
        { name: "\u0645\u0644\u0641_\u0645\u0648\u062C\u0648\u062F",     type: "function" },
        { name: "\u062C\u0630\u0631",           type: "function" },
        { name: "\u0642\u0648\u0629",           type: "function" },
        { name: "\u0646\u0633\u0628\u0629",          type: "function" },
        { name: "\u062A\u062D\u0648\u064A\u0644_\u0635\u062D\u064A\u062D",    type: "function" },
        { name: "\u062A\u062D\u0648\u064A\u0644_\u0639\u0634\u0631\u064A",    type: "function" },
        { name: "\u062A\u062D\u0648\u064A\u0644_\u0646\u0635",      type: "function" },
        { name: "\u0637\u0648\u0644",           type: "function" },
        { name: "\u062A\u0642\u0644\u064A\u0645",         type: "function" },
        { name: "\u0628\u062D\u062B",           type: "function" },
        { name: "\u0627\u0633\u062A\u0628\u062F\u0627\u0644",       type: "function" },
        { name: "\u064A\u062D\u062A\u0648\u064A",         type: "function" },
        { name: "\u064A\u0628\u062F\u0623",          type: "function" },
        { name: "\u0623\u0643\u0628\u0631",          type: "function" },
        { name: "\u0623\u0635\u063A\u0631",          type: "function" },
        { name: "\u064A\u0633\u0627\u0648\u064A",         type: "function" }
    ]

    function getCurrentWord() {
        var pos = codeEditor.cursorPosition
        var text = codeEditor.text
        if (pos <= 0 || pos > text.length) return ""
        var start = pos - 1
        while (start >= 0) {
            var ch = text.charAt(start)
            if (ch === " " || ch === "\n" || ch === "\t" || ch === "(" || ch === ")" || ch === "{" || ch === "}" || ch === ";" || ch === ",") break
            start--
        }
        start++
        if (start >= pos) return ""
        return text.substring(start, pos)
    }

    function getCursorXY() {
        var rect = codeEditor.cursorRectangle
        var editorX = codeEditor.mapToItem(editorPane, 0, 0).x
        var editorY = codeEditor.mapToItem(editorPane, 0, 0).y
        var xPos = editorX + rect.x + rect.width
        return { x: xPos, y: editorY + rect.y + rect.height }
    }

    function filterSuggestions(word) {
        if (word.length < 2) return []
        var lower = word.toLowerCase()
        var starts = []
        var contains = []
        for (var i = 0; i < daadKeywords.length; i++) {
            var kw = daadKeywords[i]
            var kwLower = kw.name.toLowerCase()
            if (kwLower.indexOf(lower) === 0) {
                starts.push(kw)
            } else if (kwLower.indexOf(lower) !== -1) {
                contains.push(kw)
            }
        }
        return starts.concat(contains)
    }

    function insertAtCursor(text) {
        codeEditor.insert(codeEditor.cursorPosition, text)
    }

    function triggerIntelliSense() {
        var word = getCurrentWord()
        if (word.length >= 2) {
            var suggestions = filterSuggestions(word)
            if (suggestions.length > 0) {
                var xy = getCursorXY()
                intelliSensePopup.showSuggestions(suggestions, word, xy.x, xy.y)
            }
        }
    }

    background: Rectangle {
        color: th.backgroundCode
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: th.backgroundCode

            RowLayout {
                LayoutMirroring.enabled: false
                anchors.fill: parent
                spacing: 0

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ScrollView {
                        anchors.fill: parent
                        clip: true

                        ScrollBar.vertical: ScrollBar {
                            parent: ScrollView.view
                            anchors.right: parent.right
                            anchors.rightMargin: 2
                            width: 8
                            background: Rectangle { color: "transparent" }
                            contentItem: Rectangle {
                                radius: 4
                                color: parent.hovered ? th.accentLapis : th.borderStone
                            }
                        }

                        ScrollBar.horizontal: ScrollBar {
                            parent: ScrollView.view
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 2
                            height: 8
                            background: Rectangle { color: "transparent" }
                            contentItem: Rectangle {
                                radius: 4
                                color: parent.hovered ? th.accentLapis : th.borderStone
                            }
                        }

                        TextArea {
                            id: codeEditor
                            font.family: th.fontFamilyCode
                            font.pixelSize: 13
                            color: th.textInk
                            selectionColor: th.withAlpha(th.accentLapis, 0.25)
                            selectedTextColor: th.textInk
                            wrapMode: TextArea.NoWrap
                            textFormat: TextEdit.PlainText
                            tabStopDistance: 32
                            leftPadding: 12
                            rightPadding: 12
                            LayoutMirroring.enabled: false

                            background: Rectangle { color: th.backgroundCode }

                            onTextChanged: {
                                editorPane.codeChanged(text)
                                if (text.length === 0) {
                                    if (intelliSensePopup.visible) intelliSensePopup.closePopup()
                                    return
                                }
                                var word = editorPane.getCurrentWord()
                                if (word.length < 2) {
                                    if (intelliSensePopup.visible) intelliSensePopup.closePopup()
                                    return
                                }
                                var suggestions = editorPane.filterSuggestions(word)
                                if (suggestions.length > 0) {
                                    if (intelliSensePopup.visible) {
                                        intelliSensePopup.suggestions = suggestions
                                        intelliSensePopup.currentWord = word
                                        intelliSensePopup.selectedIndex = 0
                                    } else {
                                        var xy = editorPane.getCursorXY()
                                        intelliSensePopup.showSuggestions(suggestions, word, xy.x, xy.y)
                                    }
                                } else {
                                    if (intelliSensePopup.visible) intelliSensePopup.closePopup()
                                }
                            }

                            Keys.onPressed: function(event) {
                                if (intelliSensePopup.visible) {
                                    if (event.key === Qt.Key_Down) {
                                        intelliSensePopup.selectedIndex = Math.min(
                                            intelliSensePopup.selectedIndex + 1,
                                            intelliSensePopup.suggestions.length - 1)
                                        intelliSensePopup.suggestionList.positionViewAtIndex(
                                            intelliSensePopup.selectedIndex, ListView.Contain)
                                        event.accepted = true
                                        return
                                    }
                                    if (event.key === Qt.Key_Up) {
                                        intelliSensePopup.selectedIndex = Math.max(
                                            intelliSensePopup.selectedIndex - 1, 0)
                                        intelliSensePopup.suggestionList.positionViewAtIndex(
                                            intelliSensePopup.selectedIndex, ListView.Contain)
                                        event.accepted = true
                                        return
                                    }
                                    if (event.key === Qt.Key_Return || event.key === Qt.Key_Tab) {
                                        var selected = intelliSensePopup.suggestions[intelliSensePopup.selectedIndex]
                                        if (selected) {
                                            intelliSensePopup.accepted(selected.name)
                                        }
                                        intelliSensePopup.closePopup()
                                        event.accepted = true
                                        return
                                    }
                                    if (event.key === Qt.Key_Escape) {
                                        intelliSensePopup.closePopup()
                                        event.accepted = true
                                        return
                                    }
                                }
                            }
                        }
                    }

                    Label {
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.topMargin: 8
                        anchors.rightMargin: 12
                        text: loc("daad_placeholder")
                        color: th.textMuted
                        font.family: th.fontFamilyCode
                        font.pixelSize: 13
                        opacity: 0.4
                        visible: codeEditor.text.length === 0
                    }
                }

                Rectangle {
                    width: 1
                    Layout.fillHeight: true
                    color: th.borderStone
                }

                Rectangle {
                    Layout.preferredWidth: 44
                    Layout.fillHeight: true
                    color: th.backgroundCode

                    Rectangle {
                        anchors.right: parent.right
                        width: 1
                        height: parent.height
                        color: th.borderStone
                    }

                    ListView {
                        id: lineNumbersView
                        anchors.fill: parent
                        anchors.margins: 4
                        model: Math.max(codeEditor.lineCount + 10, 25)
                        clip: true
                        interactive: false

                        property int currentLine: {
                            if (codeEditor.cursorPosition <= 0) return 1
                            return codeEditor.getText(0, codeEditor.cursorPosition).split("\n").length
                        }

                        Connections {
                            target: codeEditor
                            function onCursorRectangleChanged() {
                                var lineH = 16
                                var targetY = (lineNumbersView.currentLine - 1) * lineH - lineNumbersView.height / 2
                                lineNumbersView.contentY = Math.max(0, targetY)
                            }
                        }

                        delegate: Label {
                            property int lineNum: index + 1
                            width: lineNumbersView.width
                            height: 16
                            text: lineNum.toString()
                            color: lineNum === lineNumbersView.currentLine ? th.accentLapis : th.textMuted
                            font.family: th.fontFamilyCode
                            font.pixelSize: 11
                            font.bold: lineNum === lineNumbersView.currentLine
                            horizontalAlignment: Label.AlignRight
                            verticalAlignment: Label.AlignVCenter
                            rightPadding: 6
                        }
                    }
                }

                IntelliSensePopup {
                    id: intelliSensePopup
                    parent: editorPane
                    z: 100
                    onAccepted: function(word) {
                        var cw = intelliSensePopup.currentWord
                        var pos = codeEditor.cursorPosition
                        var start = pos - cw.length
                        if (start >= 0) {
                            codeEditor.remove(start, pos)
                            codeEditor.insert(start, word)
                        }
                        codeEditor.forceActiveFocus()
                    }
                    onCancelled: codeEditor.forceActiveFocus()
                }
            }
        }
    }
}

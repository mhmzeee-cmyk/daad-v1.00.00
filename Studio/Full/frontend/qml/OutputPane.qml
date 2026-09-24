import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Pane {
    id: outputPane
    padding: 0

    background: Rectangle { color: th.terminalBg }

    property string headerOutput: ""
    property string sourceOutput: ""
    property string fullSourceCode: ""

    function loc(key) { var _ = translator.languageVersion; return translator ? translator.translate(key) : key }

    function getSourceOutput() { return sourceOutput }
    function setOutput(success, header, source, diagnostics) {
        headerOutput = header
        sourceOutput = source
        errorModel.clear()
        for (var i = 0; i < diagnostics.length; i++) {
            var d = diagnostics[i]
            errorModel.append({
                severity: ["Note","Warning","Error","Fatal"][d.severity],
                message: d.message,
                line: d.line,
                column: d.column
            })
        }
    }

    function hasErrors() {
        for (var i = 0; i < errorModel.count; i++) {
            var sev = errorModel.get(i).severity
            if (sev === "Error" || sev === "Fatal") return true
        }
        return false
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: th.terminalBg

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 1
                color: th.borderStone
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10

                Rectangle {
                    width: 8; height: 8
                    radius: 4
                    color: outputPane.hasErrors() ? th.semanticError : th.semanticSuccess
                }

                Label {
                    text: outputPane.hasErrors() ? loc("tab_errors") : loc("no_errors")
                    color: outputPane.hasErrors() ? th.semanticError : th.semanticSuccess
                    font.family: th.fontFamilyUI
                    font.pixelSize: 11
                    font.bold: true
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: errorModel.count + loc("count_label")
                    color: th.textMuted
                    font.family: th.fontFamilyCode
                    font.pixelSize: 10
                    visible: errorModel.count > 0
                }
            }
        }

        ListView {
            id: errorList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: ListModel { id: errorModel }

            Label {
                anchors.centerIn: parent
                text: errorModel.count === 0 ? loc("no_errors") : ""
                color: th.semanticSuccess
                font.family: th.fontFamilyUI
                font.pixelSize: th.fontSizeNormal
            }

            delegate: Rectangle {
                width: errorList.width
                height: 32
                color: delegateMouseArea.containsMouse ? th.backgroundHover : "transparent"

                Rectangle {
                    anchors.left: parent.left
                    width: 3
                    height: parent.height
                    color: {
                        if (model.severity === "Error" || model.severity === "Fatal") return th.semanticError
                        if (model.severity === "Warning") return th.semanticWarning
                        return th.terminalAccent
                    }
                    opacity: 0.8
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 12
                    spacing: 8

                    Label {
                        text: model.message
                        color: th.terminalText
                        font.family: th.fontFamilyCode
                        font.pixelSize: 11
                        Layout.fillWidth: true
                        elide: Label.ElideRight
                        horizontalAlignment: Label.AlignRight
                    }

                    Label {
                        text: model.line + ":" + model.column
                        color: th.textMuted
                        font.family: th.fontFamilyCode
                        font.pixelSize: 10
                    }
                }

                MouseArea {
                    id: delegateMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }
}

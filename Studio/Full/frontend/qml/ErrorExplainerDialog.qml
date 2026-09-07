import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: errorDialog
    function loc(key) { return translator ? translator.translate(key) : key }
    visible: false
    z: 50

    property string errorMessage: ""
    property int errorLine: 0
    property string errorSourceCode: ""
    property string explanation: ""
    property string fixedCode: ""

    signal applyFix(string fixedCode, int errorLine)

    function open(msg, line, code) {
        errorMessage = msg
        errorLine = line
        errorSourceCode = code
        explanation = ""
        fixedCode = ""
        errorDialog.visible = true
        geminiBackend.explainError(msg, code, line)
    }

    function close() {
        errorDialog.visible = false
    }

    color: th.backgroundSidebar
    border.width: 1
    border.color: th.borderStone
    radius: th.radiusMedium

    Connections {
        target: geminiBackend
        function onErrorExplained(success, exp, code) {
            if (!errorDialog.visible) return
            explanation = exp
            fixedCode = code
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                spacing: 8

                Rectangle {
                    width: 24; height: 24
                    radius: 12
                    color: th.accentLapisBg

                    Label {
                        anchors.centerIn: parent
                        text: "\u2728"
                        font.pixelSize: 12
                    }
                }

                Label {
                    text: "\u272D " + loc("ai_error_explainer")
                    color: th.textInk
                    font.family: th.fontFamilyUI
                    font.pixelSize: th.fontSizeNormal
                    font.bold: true
                    Layout.fillWidth: true
                }

                Rectangle {
                    width: 24; height: 24
                    radius: 12
                    color: closeArea.containsMouse ? th.backgroundHover : "transparent"

                    Label {
                        anchors.centerIn: parent
                        text: "\u2715"
                        font.pixelSize: 12
                        color: th.textMuted
                    }

                    MouseArea {
                        id: closeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: errorDialog.close()
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: th.borderStone
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: errorHeaderRow.implicitHeight + 16
            color: th.withAlpha(th.semanticError, 0.1)
            radius: th.radiusSmall

            RowLayout {
                id: errorHeaderRow
                anchors.fill: parent
                anchors.margins: 8
                spacing: 6

                Label {
                    text: "\u26A0"
                    font.pixelSize: 14
                    color: th.semanticError
                }

                Label {
                    text: errorDialog.errorMessage
                    color: th.textInk
                    font.family: th.fontFamilyCode
                    font.pixelSize: 11
                    Layout.fillWidth: true
                    wrapMode: Label.Wrap
                    maximumLineCount: 3
                    elide: Label.ElideRight
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            clip: true

            BusyIndicator {
                anchors.centerIn: parent
                running: geminiBackend.errorBusy && errorDialog.visible
                visible: running
                width: 40; height: 40

                contentItem: Item {
                    implicitWidth: 40
                    implicitHeight: 40

                    Rectangle {
                        id: spinner
                        width: 32; height: 32
                        radius: 16
                        color: "transparent"
                        border.width: 3
                        border.color: "transparent"
                        anchors.centerIn: parent

                        Rectangle {
                            width: 8; height: 8
                            radius: 4
                            color: th.accentLapis
                            anchors.top: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.topMargin: 2
                        }

                        SequentialAnimation on rotation {
                            loops: Animation.Infinite
                            running: geminiBackend.errorBusy
                            NumberAnimation { from: 0; to: 360; duration: 1200; easing.type: Easing.InOutQuad }
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                text: loc("ai_analyzing_error")
                color: th.textMuted
                font.family: th.fontFamilyUI
                font.pixelSize: th.fontSizeSmall
                visible: geminiBackend.errorBusy && errorDialog.visible
            }

            ScrollView {
                anchors.fill: parent
                visible: !geminiBackend.errorBusy && explanation.length > 0
                clip: true

                TextArea {
                    id: explanationArea
                    text: errorDialog.explanation
                    readOnly: true
                    color: th.textSecondary
                    font.family: th.fontFamilyUI
                    font.pixelSize: 12
                    wrapMode: TextArea.Wrap
                    textFormat: TextEdit.MarkdownText
                    background: Rectangle { color: "transparent" }
                    leftPadding: 4
                    rightPadding: 4
                }
            }

            Label {
                anchors.centerIn: parent
                text: loc("ai_no_explanation")
                color: th.textMuted
                font.family: th.fontFamilyUI
                font.pixelSize: th.fontSizeSmall
                visible: !geminiBackend.errorBusy && explanation.length === 0
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: th.borderStone
            visible: fixedCode.length > 0 && !geminiBackend.errorBusy
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 6
            visible: fixedCode.length > 0 && !geminiBackend.errorBusy

            Label {
                text: "\u270F " + loc("ai_corrected_code")
                color: th.textMuted
                font.family: th.fontFamilyUI
                font.pixelSize: th.fontSizeSmall
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: fixedCodeArea.implicitHeight + 16
                color: th.backgroundCode
                radius: th.radiusSmall
                border.width: 1
                border.color: th.borderStone

                TextArea {
                    id: fixedCodeArea
                    anchors.fill: parent
                    anchors.margins: 8
                    text: errorDialog.fixedCode
                    readOnly: true
                    color: th.syntaxType
                    font.family: th.fontFamilyCode
                    font.pixelSize: 12
                    wrapMode: TextArea.Wrap
                    textFormat: TextEdit.PlainText
                    background: Rectangle { color: "transparent" }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 34
                radius: th.radiusSmall
                color: applyBtn.containsMouse ? th.accentLapis : th.accentLapisBg
                border.width: 1
                border.color: th.accentLapis

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 6

                    Label {
                        text: "\u2714"
                        font.pixelSize: 13
                        color: applyBtn.containsMouse ? th.textInverse : th.accentLapis
                    }

                    Label {
                        text: loc("ai_apply_fix")
                        color: applyBtn.containsMouse ? th.textInverse : th.accentLapis
                        font.family: th.fontFamilyUI
                        font.pixelSize: th.fontSizeSmall
                        font.bold: true
                    }
                }

                MouseArea {
                    id: applyBtn
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        errorDialog.applyFix(errorDialog.fixedCode, errorDialog.errorLine)
                        errorDialog.close()
                    }
                }
            }
        }
    }
}

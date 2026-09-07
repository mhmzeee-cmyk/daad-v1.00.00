import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: settingsPanel
    color: Qt.rgba(0, 0, 0, 0.6)
    visible: false

    property bool isKeyVisible: false

    function loc(key) { return translator ? translator.translate(key) : key }
    function open() { settingsPanel.visible = true; keyInput.text = ""; }
    function close() { settingsPanel.visible = false; }

    MouseArea { anchors.fill: parent; onClicked: {} }

    Rectangle {
        anchors.centerIn: parent
        width: Math.min(parent.width - 60, 500)
        height: Math.min(parent.height - 60, 440)
        radius: 12
        color: th.backgroundCode
        border.color: th.borderStone
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 0

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Label {
                    text: loc("settings")
                    color: th.textInk
                    font.family: th.fontFamilyUI
                    font.pixelSize: 18
                    font.bold: true
                    Layout.fillWidth: true
                }

                Rectangle {
                    width: 28; height: 28; radius: 6
                    color: closeArea.containsMouse ? th.backgroundHover : "transparent"
                    border.color: th.borderStone; border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: "\u2715"
                        color: th.textMuted
                        font.pixelSize: 12
                    }

                    MouseArea {
                        id: closeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: settingsPanel.close()
                    }
                }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone; Layout.topMargin: 16; Layout.bottomMargin: 20 }

            Label {
                text: loc("ai_settings")
                color: th.accentLapis
                font.family: th.fontFamilyUI
                font.pixelSize: 13
                font.bold: true
                Layout.bottomMargin: 4
            }

            Label {
                text: loc("ai_settings_desc")
                color: th.textMuted
                font.family: th.fontFamilyUI
                font.pixelSize: 11
                wrapMode: Label.Wrap
                Layout.fillWidth: true
                Layout.bottomMargin: 16
            }

            Label {
                text: "Gemini API Key"
                color: th.textSecondary
                font.family: th.fontFamilyUI
                font.pixelSize: 12
                Layout.bottomMargin: 6
            }

            Rectangle {
                Layout.fillWidth: true
                height: 40
                radius: 8
                color: th.backgroundCode
                border.color: keyInput.activeFocus ? th.accentLapis : th.borderStone
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8
                    spacing: 8

                    TextField {
                        id: keyInput
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        placeholderText: loc("api_key_placeholder")
                        placeholderTextColor: th.textMuted
                        color: th.textInk
                        font.family: th.fontFamilyCode
                        font.pixelSize: 12
                        echoMode: settingsPanel.isKeyVisible ? TextInput.Normal : TextInput.Password
                        background: Item {}
                        selectByMouse: true
                        onAccepted: saveBtn.clicked()
                    }

                    Rectangle {
                        width: 28; height: 28; radius: 6
                        color: eyeArea.containsMouse ? th.backgroundHover : "transparent"

                        Label {
                            anchors.centerIn: parent
                            text: settingsPanel.isKeyVisible ? "\uD83D\uDC41" : "\uD83D\uDC41\u200D\u2B55"
                            font.pixelSize: 14
                        }

                        MouseArea {
                            id: eyeArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: settingsPanel.isKeyVisible = !settingsPanel.isKeyVisible
                        }
                    }
                }
            }

            Label {
                text: geminiBackend.hasKey ? loc("key_saved") + " (" + geminiBackend.maskedKey + ")" : loc("no_key_saved")
                color: geminiBackend.hasKey ? th.accentLapis : th.textMuted
                font.family: th.fontFamilyUI
                font.pixelSize: 11
                Layout.topMargin: 6
            }

            Item { Layout.fillWidth: true; Layout.preferredHeight: 16 }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Rectangle {
                    Layout.fillWidth: true
                    height: 38
                    radius: 8
                    color: saveArea.containsMouse ? Qt.darker(th.accentLapis, 1.1) : th.accentLapis

                    Label {
                        anchors.centerIn: parent
                        text: loc("save_key")
                        color: "white"
                        font.family: th.fontFamilyUI
                        font.pixelSize: 13
                        font.bold: true
                    }

                    MouseArea {
                        id: saveArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            geminiBackend.saveKey(keyInput.text)
                            keyInput.text = ""
                            settingsPanel.isKeyVisible = false
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 38
                    radius: 8
                    color: testArea.containsMouse ? Qt.darker(th.backgroundSelected, 1.1) : th.backgroundSelected
                    border.color: th.borderStone
                    border.width: 1
                    enabled: !geminiBackend.testing

                    Label {
                        anchors.centerIn: parent
                        text: geminiBackend.testing ? "..." : loc("test_connection")
                        color: th.textInk
                        font.family: th.fontFamilyUI
                        font.pixelSize: 13
                    }

                    MouseArea {
                        id: testArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (keyInput.text.length > 0) {
                                geminiBackend.saveKey(keyInput.text)
                                keyInput.text = ""
                            }
                            geminiBackend.testConnection()
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 38
                radius: 8
                color: removeArea.containsMouse ? Qt.darker(th.semanticError, 1.1) : "transparent"
                border.color: geminiBackend.hasKey ? th.semanticError : th.borderStone
                border.width: 1
                visible: geminiBackend.hasKey
                Layout.topMargin: 10

                Label {
                    anchors.centerIn: parent
                    text: loc("remove_key")
                    color: th.semanticError
                    font.family: th.fontFamilyUI
                    font.pixelSize: 12
                }

                MouseArea {
                    id: removeArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: geminiBackend.removeKey()
                }
            }

            Item { Layout.fillHeight: true }
        }
    }

    Connections {
        target: geminiBackend
        function onKeySaved(success, msg) {
            toast.show(msg, success ? "success" : "error")
        }
        function onTestResult(success, msg) {
            toast.show(msg, success ? "success" : "error")
        }
    }
}

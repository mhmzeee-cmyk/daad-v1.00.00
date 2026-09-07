import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: chatPanel
    color: th.isDark ? "#1A1A1C" : "#F8F9FA"

    property string systemPrompt: "أنت مساعد برمجي ذكي متخصص في لغة البرمجة العربية 'ض' (Daad). تساعد المبرمجين في كتابة كودDaad صحيح، وشرح المفاهيم البرمجية، وتصحيح الأخطاء. أجب باختصار ووضوح. استخدم الكود البرمجي عند الحاجة."

    function loc(key) { return translator ? translator.translate(key) : key }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: th.isDark ? "#1A1A1C" : "#F8F9FA"

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: th.isDark ? "#3A3A3D" : "#E2E8F0"
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 8
                spacing: 8

                Label {
                    text: "\uD83E\uDD16"
                    font.pixelSize: 14
                }

                Label {
                    text: loc("ai_assistant")
                    color: th.isDark ? "#F1F5F9" : "#1E293B"
                    font.family: th.fontFamilyUI
                    font.pixelSize: 13
                    font.bold: true
                    Layout.fillWidth: true
                }

                Rectangle {
                    width: 24; height: 24; radius: 4
                    color: clearArea.containsMouse ? (th.isDark ? "#3A3A3D" : "#F1F3F5") : "transparent"
                    visible: chatModel.count > 0

                    Label {
                        anchors.centerIn: parent
                        text: "\u2715"
                        color: th.isDark ? "#64748B" : "#94A3B8"
                        font.pixelSize: 10
                    }

                    MouseArea {
                        id: clearArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: geminiBackend.clearChat()
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: th.isDark ? "#1A1A1C" : "#FFFFFF"

            ListModel { id: chatModel }

            ListView {
                id: chatList
                anchors.fill: parent
                anchors.margins: 8
                clip: true
                spacing: 8
                model: chatModel

                Connections {
                    target: geminiBackend
                    function onChatHistoryChanged() {
                        chatModel.clear()
                        var history = geminiBackend.chatHistory
                        for (var i = 0; i < history.length; i++) {
                            chatModel.append({
                                role: history[i].role,
                                text: history[i].text,
                                time: history[i].time || "",
                                isError: history[i].isError || false
                            })
                        }
                        chatList.positionViewAtEnd()
                    }
                }

                add: Transition {
                    NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 150 }
                }

                delegate: Rectangle {
                    width: chatList.width
                    height: bubble.height + 16
                    color: "transparent"

                    Rectangle {
                        id: bubble
                        anchors.left: model.role === "user" ? undefined : parent.left
                        anchors.right: model.role === "user" ? parent.right : undefined
                        anchors.leftMargin: model.role === "user" ? 40 : 0
                        anchors.rightMargin: model.role === "user" ? 0 : 40
                        anchors.top: parent.top
                        anchors.topMargin: 8
                        width: Math.min(parent.width * 0.88, contentLabel.implicitWidth + 24)
                        height: contentLabel.implicitHeight + 24
                        radius: 10
                        color: model.role === "user" ? "#14B8A6" :
                               model.isError ? (th.isDark ? "rgba(239,68,68,0.15)" : "rgba(220,38,38,0.1)") :
                               (th.isDark ? "#252527" : "#F1F3F5")
                        border.color: model.role === "user" ? "transparent" :
                                     model.isError ? (th.isDark ? "#EF4444" : "#DC2626") :
                                     (th.isDark ? "#3A3A3D" : "#E2E8F0")
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 2

                            Label {
                                id: contentLabel
                                text: model.text
                                color: model.role === "user" ? "white" :
                                       model.isError ? (th.isDark ? "#EF4444" : "#DC2626") :
                                       (th.isDark ? "#F1F5F9" : "#1E293B")
                                font.family: model.isError ? th.fontFamilyCode : th.fontFamilyUI
                                font.pixelSize: 12
                                wrapMode: Label.Wrap
                                Layout.fillWidth: true
                            }

                            Label {
                                text: model.time || ""
                                color: model.role === "user" ? "rgba(255,255,255,0.5)" :
                                       (th.isDark ? "#64748B" : "#94A3B8")
                                font.family: th.fontFamilyUI
                                font.pixelSize: 9
                                Layout.alignment: Qt.AlignRight
                                visible: model.time !== ""
                            }
                        }
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                text: loc("ai_welcome")
                color: th.isDark ? "#64748B" : "#94A3B8"
                font.family: th.fontFamilyUI
                font.pixelSize: 13
                visible: chatModel.count === 0
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 52
            color: th.isDark ? "#1A1A1C" : "#F8F9FA"

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 1
                color: th.isDark ? "#3A3A3D" : "#E2E8F0"
            }

            RowLayout {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 6

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 8
                    color: th.isDark ? "#252527" : "#FFFFFF"
                    border.color: inputField.activeFocus ? "#14B8A6" :
                                 (th.isDark ? "#3A3A3D" : "#E2E8F0")
                    border.width: 1

                    TextField {
                        id: inputField
                        anchors.fill: parent
                        anchors.margins: 2
                        placeholderText: loc("ai_input_placeholder")
                        placeholderTextColor: th.isDark ? "#64748B" : "#94A3B8"
                        color: th.isDark ? "#F1F5F9" : "#1E293B"
                        font.family: th.fontFamilyUI
                        font.pixelSize: 12
                        background: Item {}
                        selectByMouse: true
                        enabled: !geminiBackend.chatBusy
                        onAccepted: sendBtn.clicked()
                    }
                }

                Rectangle {
                    id: sendBtn
                    width: 38; height: 38; radius: 8
                    color: sendArea.containsMouse ? Qt.darker("#14B8A6", 1.1) : "#14B8A6"
                    opacity: inputField.text.length > 0 && !geminiBackend.chatBusy ? 1.0 : 0.4

                    Label {
                        anchors.centerIn: parent
                        text: geminiBackend.chatBusy ? "\u23F3" : "\u27A1"
                        color: "white"
                        font.pixelSize: 14
                    }

                    MouseArea {
                        id: sendArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            var txt = inputField.text.trim()
                            if (txt.length === 0 || geminiBackend.chatBusy) return
                            if (!geminiBackend.hasKey) {
                                toast.show(loc("ai_no_key"), "error", 5000)
                                settingsPanel.open()
                                return
                            }
                            inputField.text = ""
                            geminiBackend.sendMessage(txt, systemPrompt)
                        }
                    }
                }
            }
        }
    }
}

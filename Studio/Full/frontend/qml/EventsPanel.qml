import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: eventsPanel
    color: th.backgroundSidebar
    width: 280

    required property var designerModel

    property int selectedIndex: designerModel ? designerModel.selectedIndex : -1
    property var selectedWidget: {
        if (!designerModel || selectedIndex < 0) return null
        return designerModel.getWidget(selectedIndex)
    }

    signal editEventCode(string objectName, string eventName, string code)

    property var eventDefinitions: {
        var t = selectedWidget ? selectedWidget.type : ""
        if (t === "\u0632\u0631" || t === "\u0645\u0628\u0631\u0632_\u0627\u062E\u062A\u064A\u0627\u0631") {
            return [
                { name: "onClick",    label: "\u0639\u0646\u062F \u0627\u0644\u0636\u063A\u0637",   icon: "\u{1F5B1}", desc: "\u064A\u0639\u0645\u0644 \u0639\u0646\u062F \u0636\u063A\u0637 \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645" },
                { name: "onHover",    label: "\u0639\u0646\u062F \u0627\u0644\u062A\u0645\u0631\u0648\u0631", icon: "\u{1F441}", desc: "\u064A\u0639\u0645\u0644 \u0639\u0646\u062F \u062A\u0645\u0631\u0648\u0631 \u0627\u0644\u0645\u0633\u062A\u062E\u062F\u0645 \u0641\u0648\u0642 \u0627\u0644\u0641\u0627\u0631\u0626\u0629" },
                { name: "onEnable",   label: "\u0639\u0646\u062F \u0627\u0644\u062A\u0641\u0639\u064A\u0644",  icon: "\u26A1", desc: "\u064A\u0639\u0645\u0644 \u0639\u0646\u062F \u062A\u0641\u0639\u064A\u0644 \u0627\u0644\u0639\u0646\u0635\u0631" }
            ]
        }
        if (t === "\u062D\u0642\u0644_\u0646\u0635") {
            return [
                { name: "onTextChanged", label: "\u0639\u0646\u062F \u062A\u063A\u064A\u064A\u0631 \u0627\u0644\u0646\u0635", icon: "\u270F", desc: "\u064A\u0639\u0645\u0644 \u0639\u0646\u062F \u062A\u063A\u064A\u064A\u0631 \u0627\u0644\u0646\u0635 \u0641\u064A \u062D\u0642\u0644 \u0627\u0644\u0646\u0635" },
                { name: "onSubmit",    label: "\u0639\u0646\u062F \u0627\u0644\u0625\u062F\u062E\u0627\u0644",   icon: "\u23CE", desc: "\u064A\u0639\u0645\u0644 \u0639\u0646\u062F \u0636\u063A\u0637 Enter" }
            ]
        }
        return []
    }

    property var eventCodes: ({})

    function getEventCode(objectName, eventName) {
        var key = objectName + "." + eventName
        return eventCodes[key] || ""
    }

    function setEventCode(objectName, eventName, code) {
        var key = objectName + "." + eventName
        var newCodes = eventCodes
        newCodes[key] = code
        eventCodes = newCodes
    }

    function generateEventsCode() {
        var code = ""
        for (var i = 0; i < designerModel.count; i++) {
            var w = designerModel.getWidget(i)
            if (!w) continue
            var name = w.variableName
            for (var key in eventCodes) {
                if (key.startsWith(name + ".")) {
                    var evName = key.split(".")[1]
                    var evCode = eventCodes[key]
                    if (evCode.length > 0) {
                        code += "// حدث: " + name + "." + evName + "\n"
                        code += "دالة " + name + "_" + evName + "() {\n"
                        code += "  " + evCode.split("\n").join("\n  ") + "\n}\n\n"
                    }
                }
            }
        }
        return code
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 6

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 26
            color: "transparent"

            Label {
                anchors.centerIn: parent
                text: "\u{1F527} " + "\u0627\u0644\u0623\u062D\u062F\u0627\u062B"
                color: th.textInk
                font.family: th.fontFamilyUI
                font.pixelSize: th.fontSizeSmall
                font.bold: true
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: 4

                Label {
                    text: "\u0627\u062E\u062A\u0631 \u0639\u0646\u0635\u0631\u0627\u064B \u0644\u0639\u0631\u0636 \u0623\u062D\u062F\u0627\u062B\u0647"
                    color: th.textMuted
                    font.family: th.fontFamilyUI
                    font.pixelSize: th.fontSizeSmall
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                    Layout.topMargin: 20
                    visible: !selectedWidget
                }

                ColumnLayout {
                    spacing: 4
                    visible: selectedWidget

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 28
                        radius: th.radiusSmall
                        color: th.backgroundHover

                        Label {
                            anchors.centerIn: parent
                            text: (selectedWidget ? selectedWidget.variableName : "") + " \u2014 " + (selectedWidget ? selectedWidget.type : "")
                            color: th.accentLapis
                            font.family: th.fontFamilyUI
                            font.pixelSize: 10
                            font.bold: true
                        }
                    }

                    Repeater {
                        model: eventsPanel.eventDefinitions

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: eventItemCol.implicitHeight + 12
                            radius: th.radiusSmall
                            color: th.backgroundCode
                            border.color: th.borderStone
                            border.width: 1

                            ColumnLayout {
                                id: eventItemCol
                                anchors.fill: parent
                                anchors.margins: 6
                                spacing: 4

                                RowLayout {
                                    spacing: 6
                                    Layout.fillWidth: true

                                    Item { Layout.fillWidth: true } // Pushes content to the right

                                    Rectangle {
                                        width: 16; height: 16; radius: 3
                                        color: eventsPanel.getEventCode(selectedWidget.variableName, modelData.name).length > 0 ? "#81C784" : th.borderStone

                                        Label {
                                            anchors.centerIn: parent
                                            text: eventsPanel.getEventCode(selectedWidget.variableName, modelData.name).length > 0 ? "\u2714" : "\u2716"
                                            font.pixelSize: 9
                                            color: "white"
                                        }
                                    }

                                    Label {
                                        text: modelData.label
                                        color: th.textInk
                                        font.family: th.fontFamilyUI
                                        font.pixelSize: 11
                                        font.bold: true
                                        horizontalAlignment: Text.AlignRight
                                    }

                                    Label {
                                        text: modelData.icon
                                        font.pixelSize: 14
                                    }
                                }



                                Label {
                                    text: modelData.desc
                                    color: th.textMuted
                                    font.family: th.fontFamilyUI
                                    font.pixelSize: 9
                                    wrapMode: Text.Wrap
                                    Layout.fillWidth: true
                                }

                                Label { // Function signature
                                    text: "دالة " + modelData.daadEvent + "_" + (selectedWidget ? selectedWidget.variableName : "") + "()"
                                    color: th.textMuted
                                    font.family: th.fontFamilyCode; font.pixelSize: 9
                                    Layout.fillWidth: true
                                    elide: Text.ElideLeft
                                    horizontalAlignment: Text.AlignRight
                                }

                                Rectangle { // Code editor background
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 60
                                    radius: 3
                                    color: th.backgroundCode
                                    border.color: codeEdit.activeFocus ? th.accentLapis : th.borderStone
                                    border.width: 1

                                    ScrollView {
                                        anchors.fill: parent
                                        anchors.margins: 2
                                        clip: true

                                        TextArea {
                                            id: codeEdit
                                            property string eventName: modelData.name
                                            property string objectName: selectedWidget ? selectedWidget.variableName : ""
                                            text: eventsPanel.getEventCode(objectName, eventName)
                                            onTextChanged: eventsPanel.setEventCode(objectName, eventName, text)
                                            font.family: th.fontFamilyCode
                                            font.pixelSize: 10
                                            color: th.textInk
                                            background: null
                                            placeholderText: modelData.desc
                                            placeholderTextColor: th.textMuted
                                            wrapMode: TextArea.Wrap
                                            Layout.direction: Qt.RightToLeft // Added for RTL
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: inspector
    function loc(key) { return translator ? translator.translate(key) : key }
    color: th.backgroundSidebar
    width: 220

    required property var designerModel

    property int selectedIndex: designerModel ? designerModel.selectedIndex : -1
    property var selectedWidget: {
        if (!designerModel || selectedIndex < 0) return null
        return designerModel.getWidget(selectedIndex)
    }

    property var colorCycle: ["#14B8A6", "#EF5350", "#64B5F6", "#FBBF24", "#C792EA", "#81C784", "#F472B6", "#06B6D4"]
    property int currentTab: 0

    signal injectEventCode(string code, string functionName)

    function cycleColor() {
        if (!selectedWidget) return
        var cur = selectedWidget.color
        var idx = colorCycle.indexOf(cur)
        var next = (idx + 1) % colorCycle.length
        designerModel.updateWidgetProperty(selectedIndex, "color", colorCycle[next])
    }

    function propVal(def) { return selectedWidget ? def : "" }

    function getEventDefinitions() {
        var t = selectedWidget ? selectedWidget.type : ""
        if (t === "زر" || t === "زر_أمر" || t === "مربع_اختيار") {
            return [
                { name: "onClick", label: "عند النقر", icon: "\u{1F5B1}", daadEvent: "نقر" },
                { name: "onHover", label: "عند التمرير", icon: "\u{1F441}", daadEvent: "تمرير" },
                { name: "onEnable", label: "عند التفعيل", icon: "\u26A1", daadEvent: "تفعيل" }
            ]
        }
        if (t === "حقل_نص") {
            return [
                { name: "onTextChanged", label: "عند التغيير", icon: "\u270F", daadEvent: "تغيير" },
                { name: "onSubmit", label: "عند الإدخال", icon: "\u23CE", daadEvent: "إدخال" }
            ]
        }
        if (t === "قائمة_خيارات" || t === "قائمة_منسدلة") {
            return [
                { name: "onSelect", label: "عند الاختيار", icon: "\u2699", daadEvent: "اختيار" }
            ]
        }
        if (t === "صورة") {
            return [
                { name: "onClick", label: "عند النقر", icon: "\u{1F5B1}", daadEvent: "نقر" },
                { name: "onLoad", label: "عند التحميل", icon: "\u{1F504}", daadEvent: "تحميل" }
            ]
        }
        return []
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
                text: "\u2699 " + loc("properties")
                color: th.textInk; font.family: th.fontFamilyUI; font.pixelSize: th.fontSizeSmall; font.bold: true
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            spacing: 2

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: th.radiusSmall
                color: currentTab === 0 ? th.accentLapisBg : "transparent"
                border.color: currentTab === 0 ? th.accentLapis : "transparent"
                border.width: 1

                Label {
                    anchors.centerIn: parent
                    text: "\u2699 " + loc("properties")
                    color: currentTab === 0 ? th.accentLapis : th.textMuted
                    font.family: th.fontFamilyUI; font.pixelSize: 9
                }
                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: currentTab = 0 }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: th.radiusSmall
                color: currentTab === 1 ? th.accentLapisBg : "transparent"
                border.color: currentTab === 1 ? th.accentLapis : "transparent"
                border.width: 1

                Label {
                    anchors.centerIn: parent
                    text: "\u{1F527} " + loc("events_tab")
                    color: currentTab === 1 ? th.accentLapis : th.textMuted
                    font.family: th.fontFamilyUI; font.pixelSize: 9
                }
                MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: currentTab = 1 }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: currentTab

            Item { // Properties Tab Content
                Layout.fillWidth: true
                Layout.fillHeight: true

                ScrollView {
                    anchors.fill: parent; clip: true
                    ColumnLayout {
                        width: parent.width; spacing: 4
                        Label {
                            text: loc("select_widget")
                            color: th.textMuted; font.family: th.fontFamilyUI; font.pixelSize: th.fontSizeSmall
                            wrapMode: Text.Wrap; horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true; Layout.topMargin: 20; visible: !selectedWidget
                        }
                        ColumnLayout {
                            spacing: 6; visible: selectedWidget
                            Rectangle {
                                Layout.fillWidth: true; Layout.preferredHeight: 28; radius: th.radiusSmall; color: th.backgroundHover
                                Label {
                                    anchors.centerIn: parent
                                    text: selectedWidget ? selectedWidget.type : ""
                                    color: th.accentLapis; font.family: th.fontFamilyUI; font.pixelSize: 11; font.bold: true
                                }
                            }
                            ColumnLayout { spacing: 2
                                Label { text: loc("var_name"); color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                TextField {
                                    Layout.fillWidth: true
                                    text: selectedWidget ? selectedWidget.variableName : ""
                                    onEditingFinished: designerModel.updateWidgetProperty(selectedIndex, "variableName", text)
                                    font.family: th.fontFamilyCode; font.pixelSize: 11; color: th.textInk
                                    background: Rectangle { color: th.backgroundCode; border.color: parent.activeFocus ? th.accentLapis : th.borderStone; border.width: 1; radius: 3 }
                                    Layout.direction: Qt.RightToLeft
                                }
                            }
                            Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone; opacity: 0.5 }
                            ColumnLayout { spacing: 2
                                Label { text: loc("prop_text"); color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                TextField {
                                    Layout.fillWidth: true
                                    text: selectedWidget ? selectedWidget.text : ""
                                    onEditingFinished: designerModel.updateWidgetProperty(selectedIndex, "text", text)
                                    font.family: th.fontFamilyUI; font.pixelSize: 11; color: th.textInk
                                    background: Rectangle { color: th.backgroundCode; border.color: parent.activeFocus ? th.accentLapis : th.borderStone; border.width: 1; radius: 3 }
                                    Layout.direction: Qt.RightToLeft
                                }
                            }
                            GridLayout { Layout.fillWidth: true; columns: 2; columnSpacing: 4; rowSpacing: 4
                                ColumnLayout { spacing: 1
                                    Label { text: loc("prop_width"); color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                    TextField {
                                        Layout.fillWidth: true
                                        text: selectedWidget ? selectedWidget.width : ""
                                        onEditingFinished: designerModel.updateWidgetProperty(selectedIndex, "width", Number(text))
                                        font.family: th.fontFamilyCode; font.pixelSize: 11; color: th.textInk
                                        background: Rectangle { color: th.backgroundCode; border.color: parent.activeFocus ? th.accentLapis : th.borderStone; border.width: 1; radius: 3 }
                                        Layout.direction: Qt.RightToLeft
                                    }
                                }
                                ColumnLayout { spacing: 1
                                    Label { text: loc("prop_height"); color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                    TextField {
                                        Layout.fillWidth: true
                                        text: selectedWidget ? selectedWidget.height : ""
                                        onEditingFinished: designerModel.updateWidgetProperty(selectedIndex, "height", Number(text))
                                        font.family: th.fontFamilyCode; font.pixelSize: 11; color: th.textInk
                                        background: Rectangle { color: th.backgroundCode; border.color: parent.activeFocus ? th.accentLapis : th.borderStone; border.width: 1; radius: 3 }
                                        Layout.direction: Qt.RightToLeft
                                    }
                                }
                            }
                            GridLayout { Layout.fillWidth: true; columns: 2; columnSpacing: 4; rowSpacing: 4
                                ColumnLayout { spacing: 1
                                    Label { text: loc("prop_x"); color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                    TextField {
                                        Layout.fillWidth: true
                                        text: selectedWidget ? selectedWidget.x : ""
                                        onEditingFinished: designerModel.updateWidgetProperty(selectedIndex, "x", Number(text))
                                        font.family: th.fontFamilyCode; font.pixelSize: 11; color: th.textInk
                                        background: Rectangle { color: th.backgroundCode; border.color: parent.activeFocus ? th.accentLapis : th.borderStone; border.width: 1; radius: 3 }
                                        Layout.direction: Qt.RightToLeft
                                    }
                                }
                                ColumnLayout { spacing: 1
                                    Label { text: loc("prop_y"); color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                    TextField {
                                        Layout.fillWidth: true
                                        text: selectedWidget ? selectedWidget.y : ""
                                        onEditingFinished: designerModel.updateWidgetProperty(selectedIndex, "y", Number(text))
                                        font.family: th.fontFamilyCode; font.pixelSize: 11; color: th.textInk
                                        background: Rectangle { color: th.backgroundCode; border.color: parent.activeFocus ? th.accentLapis : th.borderStone; border.width: 1; radius: 3 }
                                        Layout.direction: Qt.RightToLeft
                                    }
                                }
                            }
                            Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone; opacity: 0.5 }
                            RowLayout { spacing: 6
                                Label { text: loc("prop_color"); color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                Rectangle {
                                    width: 24; height: 24; radius: 4
                                    color: selectedWidget ? selectedWidget.color : "gray"
                                    border.color: th.borderStone; border.width: 1
                                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: cycleColor() }
                                }
                                Label { text: selectedWidget ? selectedWidget.color : ""; color: th.textMuted; font.family: th.fontFamilyCode; font.pixelSize: 9 }
                                Item { Layout.fillWidth: true }
                            }
                            ColumnLayout { spacing: 2
                                Label { text: loc("prop_font_size"); color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                RowLayout { spacing: 4
                                    Slider {
                                        id: fsSlider; Layout.fillWidth: true
                                        from: 8; to: 48; stepSize: 1
                                        value: selectedWidget ? selectedWidget.fontSize : 13
                                        onMoved: designerModel.updateWidgetProperty(selectedIndex, "fontSize", value)
                                    }
                                    Label { text: Math.round(fsSlider.value); color: th.textInk; font.family: th.fontFamilyCode; font.pixelSize: 10; Layout.preferredWidth: 24 }
                                }
                            }
                            Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone; opacity: 0.5 }
                            RowLayout { spacing: 8
                                Label { text: "مفعل"; color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                Switch {
                                    checked: selectedWidget ? selectedWidget.enabled : true
                                    onToggled: designerModel.updateWidgetProperty(selectedIndex, "enabled", checked)
                                    scale: 0.7
                                }
                            }
                            RowLayout { spacing: 8
                                Label { text: "مرئي"; color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                Switch {
                                    checked: selectedWidget ? selectedWidget.visible : true
                                    onToggled: designerModel.updateWidgetProperty(selectedIndex, "visible", checked)
                                    scale: 0.7
                                }
                            }
                            Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone; opacity: 0.5 }
                            ColumnLayout { spacing: 2
                                Label { text: "تفسير نصي"; color: th.textMuted; font.pixelSize: 9; font.family: th.fontFamilyUI; horizontalAlignment: Text.AlignRight }
                                TextField {
                                    Layout.fillWidth: true
                                    text: selectedWidget ? selectedWidget.tooltip : ""
                                    onEditingFinished: designerModel.updateWidgetProperty(selectedIndex, "tooltip", text)
                                    placeholderText: "تفسير نصي..."
                                    font.family: th.fontFamilyUI; font.pixelSize: 10; color: th.textInk
                                    background: Rectangle { color: th.backgroundCode; border.color: parent.activeFocus ? th.accentLapis : th.borderStone; border.width: 1; radius: 3 }
                                    Layout.direction: Qt.RightToLeft
                                }
                            }
                            Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone; opacity: 0.5 }
                            Rectangle {
                                Layout.fillWidth: true; Layout.preferredHeight: 30; radius: th.radiusSmall
                                color: delArea.containsMouse ? "#EF5350" : "transparent"
                                border.color: "#EF5350"; border.width: 1
                                Label {
                                    anchors.centerIn: parent
                                    text: "\u{1F5D1} " + loc("delete_widget")
                                    color: delArea.containsMouse ? "white" : "#EF5350"
                                    font.family: th.fontFamilyUI; font.pixelSize: 11; font.bold: true
                                }
                                MouseArea {
                                    id: delArea; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                                    onClicked: designerModel.removeWidget(selectedIndex)
                                }
                            }
                        }
                    }
                }
            }
            Item { // Events Tab Content
                Layout.fillWidth: true
                Layout.fillHeight: true

                ScrollView {
                    anchors.fill: parent; clip: true
                    ColumnLayout {
                        width: parent.width; spacing: 4
                        Label {
                            text: "اختر عنصراً لعرض أحداثه"
                            color: th.textMuted; font.family: th.fontFamilyUI; font.pixelSize: th.fontSizeSmall
                            wrapMode: Text.Wrap; horizontalAlignment: Text.AlignHCenter
                            Layout.fillWidth: true; Layout.topMargin: 20; visible: !selectedWidget
                        }

                        ColumnLayout {
                            spacing: 4; visible: selectedWidget
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 28
                                radius: th.radiusSmall
                                color: th.backgroundHover
                                Label {
                                    anchors.centerIn: parent
                                    text: (selectedWidget ? selectedWidget.variableName : "") + " \u2014 " + (selectedWidget ? selectedWidget.type : "")
                                    color: th.accentLapis
                                    font.family: th.fontFamilyUI; font.pixelSize: 10; font.bold: true
                                }
                            }

                            Repeater {
                                model: inspector.getEventDefinitions()

                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 72
                                    radius: th.radiusSmall
                                    color: th.backgroundCode
                                    border.color: eventMouse.containsMouse ? th.accentLapis : th.borderStone
                                    border.width: 1

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 6
                                        spacing: 4

                                        RowLayout {
                                            spacing: 6
                                            Label { text: modelData.icon; font.pixelSize: 14 }
                                            Label {
                                                text: modelData.label
                                                color: th.textInk
                                                font.family: th.fontFamilyUI; font.pixelSize: 11; font.bold: true
                                                horizontalAlignment: Text.AlignRight
                                                Layout.fillWidth: true
                                            }
                                            Rectangle {
                                                width: 20; height: 16; radius: 3
                                                color: eventMouse.containsMouse ? th.accentLapis : "transparent"
                                                border.color: th.accentLapis; border.width: 1
                                                Label {
                                                    anchors.centerIn: parent
                                                    text: "\u21E7"
                                                    font.pixelSize: 9; font.bold: true
                                                    color: eventMouse.containsMouse ? "white" : th.accentLapis
                                                }
                                            }
                                        }

                                        Label {
                                            text: "دالة " + modelData.daadEvent + "_" + (selectedWidget ? selectedWidget.variableName : "") + "()"
                                            color: th.textMuted
                                            font.family: th.fontFamilyCode; font.pixelSize: 9
                                            Layout.fillWidth: true
                                            elide: Text.ElideRight
                                            horizontalAlignment: Text.AlignRight
                                        }

                                        MouseArea {
                                            id: eventMouse
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            cursorShape: Qt.PointingHandCursor
                                            onDoubleClicked: {
                                                if (!selectedWidget) return
                                                var funcName = modelData.daadEvent + "_" + selectedWidget.variableName
                                                var stub = "دالة " + funcName + "() {\n  // كود الحدث\n}\n"
                                                inspector.injectEventCode(stub, funcName)
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
}

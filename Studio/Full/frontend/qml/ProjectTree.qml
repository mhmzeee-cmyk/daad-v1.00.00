import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Pane {
    id: projectTree
    padding: 0

    background: Rectangle {
        color: th.backgroundSidebar
        Rectangle {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: th.borderStone
        }
    }

    signal projectSelected(string code)
    signal fileOpenRequested()

    property int selectedIndex: -1

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "transparent"

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: th.borderStone
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12

                Label {
                    text: "PROJECTS"
                    color: th.textMuted
                    font.family: th.fontFamilyUI
                    font.pixelSize: 10
                    font.bold: true
                    Layout.fillWidth: true
                    font.capitalization: Font.AllUppercase
                }

                Rectangle {
                    width: 22; height: 22
                    radius: th.radiusSmall
                    color: addFileBtn.containsMouse ? th.backgroundHover : th.borderStone

                    Label {
                        anchors.centerIn: parent
                        text: "+"
                        font.pixelSize: 13
                        font.bold: true
                        color: th.textInk
                    }

                    MouseArea {
                        id: addFileBtn
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: projectTree.fileOpenRequested()
                    }
                }
            }
        }

        ListView {
            id: fileList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            model: ListModel {
                ListElement {
                    name: "basic.ض"
                    code: "\u0635\u062D\u064A\u062D \u0633 = 10 \u061B\n\u0637\u0628\u0627\u0639\u0629( \u0633 ) \u061B\n\u0635\u062D\u064A\u062D \u0627\u0644\u0646\u062A\u064A\u062C\u0629 = 5 + 3 \u061B\n\u0637\u0628\u0627\u0639\u0629( \u0627\u0644\u0646\u062A\u064A\u062C\u0629 ) \u061B"
                    tag: "VAR"
                }
                ListElement {
                    name: "loop.ض"
                    code: "\u0635\u062D\u064A\u062D \u0627 = 0 \u061B\n\u0643\u0631\u0631 ( \u0627 < 5 ) {\n    \u0637\u0628\u0627\u0639\u0629( \u0627 ) \u061B\n    \u0627 = \u0627 + 1 \u061B\n}"
                    tag: "LOOP"
                }
                ListElement {
                    name: "function.ض"
                    code: "\u0635\u062D\u064A\u062D \u0645\u062C\u0645\u0648\u0639( \u0635\u062D\u064A\u062D \u0623 \u060C \u0635\u062D\u064A\u062D \u0628 ) {\n    \u0627\u0631\u062C\u0639 \u0623 + \u0628 \u061B\n}"
                    tag: "FN"
                }
                ListElement {
                    name: "class.ض"
                    code: "\u0641\u0626\u0629 \u0646\u0642\u0627\u0637 {\n    \u0635\u062D\u064A\u062D \u0633 \u061B\n    \u0635\u062D\u064A\u062D \u0635 \u061B\n}"
                    tag: "CLS"
                }
                ListElement {
                    name: "print.ض"
                    code: "\u0637\u0628\u0627\u0639\u0629( \"\u0645\u0631\u062D\u0628\u0627\u064B \u0628\u0627\u0644\u0639\u0627\u0644\u0645\" ) \u061B\n\u0635\u062D\u064A\u062D \u0627\u0633\u0645 = \"\u0636\" \u061B\n\u0637\u0628\u0627\u0639\u0629( \u0627\u0633\u0645 ) \u061B"
                    tag: "IO"
                }
                ListElement {
                    name: "condition.ض"
                    code: "\u0635\u062D\u064A\u062D \u0639\u0645\u0631 = 25 \u061B\n\u0627\u0630\u0627 ( \u0639\u0645\u0631 >= 18 ) {\n    \u0637\u0628\u0627\u0639\u0629( \"\u0628\u0627\u0644\u063A\" ) \u061B\n} \u0648\u0627\u0644\u0627 {\n    \u0637\u0628\u0627\u0639\u0629( \"\u0642\u0627\u0635\u0631\" ) \u061B\n}"
                    tag: "IF"
                }
            }

            delegate: Rectangle {
                required property var model
                required property int index
                width: fileList.width
                height: 44
                color: mouseArea.containsMouse ? th.backgroundHover :
                       (selectedIndex === index ? th.backgroundSelected : "transparent")

                Rectangle {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 2
                    color: th.accentLapis
                    visible: selectedIndex === index
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        fileList.currentIndex = index
                        selectedIndex = index
                        projectTree.projectSelected(model.code)
                    }
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8
                    spacing: 2

                    RowLayout {
                        spacing: 8
                        Layout.fillWidth: true

                        Rectangle {
                            width: 24; height: 16
                            radius: 3
                            color: th.accentLapisBg

                            Label {
                                anchors.centerIn: parent
                                text: model.tag
                                font.family: th.fontFamilyCode
                                font.pixelSize: 8
                                font.bold: true
                                color: th.accentLapis
                            }
                        }

                        Label {
                            text: model.name
                            color: selectedIndex === index ? th.textInk : th.textSecondary
                            font.family: th.fontFamilyCode
                            font.pixelSize: th.fontSizeSmall
                            Layout.fillWidth: true
                            elide: Label.ElideRight
                        }
                    }

                    Label {
                        text: {
                            var lines = model.code.split("\n")
                            return lines[0]
                        }
                        color: th.textMuted
                        font.family: th.fontFamilyCode
                        font.pixelSize: 9
                        Layout.fillWidth: true
                        Layout.leftMargin: 32
                        elide: Label.ElideRight
                        maximumLineCount: 1
                    }
                }
            }

            highlight: Rectangle { color: "transparent" }
            highlightFollowsCurrentItem: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            color: "transparent"

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 1
                color: th.borderStone
            }

            Label {
                anchors.centerIn: parent
                text: fileList.count + " files"
                color: th.textMuted
                font.family: th.fontFamilyCode
                font.pixelSize: 10
            }
        }
    }
}

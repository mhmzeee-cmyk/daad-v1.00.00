import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Item {
    id: studentDashboard

    property var userProfile: ({})
    property var selectedChallenge: null
    property string currentCode: ""

    signal logout()

    function loc(key) { var _ = translator.languageVersion; return translator.translate(key) }
    function profileName() { return userProfile.name || cloudService.username || "" }
    function schoolName() { return (userProfile.school && userProfile.school.name) || "" }

    Component.onCompleted: {
        challengeManager.fetchChallenges(cloudService.userProfile.accessToken || "")
        challengeManager.fetchDailyChallenge(cloudService.userProfile.accessToken || "")
    }

    Rectangle { anchors.fill: parent; color: th.backgroundPaper }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ══════════════════════════════════════════════════════════════════
        // LEFT SIDEBAR
        // ══════════════════════════════════════════════════════════════════
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 260
            color: th.backgroundSidebar

            Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: th.borderStone }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 0
                spacing: 0

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 70
                    color: "transparent"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        spacing: 10

                        Rectangle {
                            width: 40; height: 40; radius: 10
                            color: th.accentLapisBg
                            Label { anchors.centerIn: parent; text: "\u0636"; font.pixelSize: 20; font.bold: true; color: th.accentLapis }
                        }

                        ColumnLayout {
                            spacing: 0
                            Label { text: schoolName(); font.family: th.fontFamilyUI; font.pixelSize: 13; font.bold: true; color: th.textInk; elide: Text.ElideRight; Layout.fillWidth: true }
                            Label { text: loc("nav_track_students"); font.family: th.fontFamilyUI; font.pixelSize: 10; color: th.textMuted }
                        }
                    }
                }

                Rectangle { Layout.fillWidth: true; Layout.leftMargin: 16; Layout.rightMargin: 16; height: 1; color: th.borderStone }

                Item { Layout.preferredHeight: 12 }

                Label { text: loc("challenges"); font.family: th.fontFamilyUI; font.pixelSize: 11; font.bold: true; color: th.textMuted; Layout.leftMargin: 20 }

                Item { Layout.preferredHeight: 6 }

                ScrollView {
                    Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                    ListView {
                        id: challengeList
                        model: challengeManager.challenges
                        spacing: 4
                        Layout.leftMargin: 8; Layout.rightMargin: 8

                        delegate: Rectangle {
                            width: challengeList.width - 16; height: delegateCol.height + 14; radius: 10
                            color: delegateArea.containsMouse ? th.backgroundHover : (studentDashboard.selectedChallenge === modelData ? th.accentLapisBg : "transparent")
                            border.width: 1; border.color: studentDashboard.selectedChallenge === modelData ? th.accentLapis : "transparent"

                            ColumnLayout {
                                id: delegateCol; anchors.fill: parent; anchors.margins: 8; spacing: 3
                                Label { text: modelData.title || ""; font.family: th.fontFamilyUI; font.pixelSize: 12; font.bold: true; color: th.textInk; wrapMode: Text.Wrap; Layout.fillWidth: true }
                                Label { text: modelData.description || ""; font.pixelSize: 10; color: th.textSecondary; wrapMode: Text.Wrap; Layout.fillWidth: true; maximumLineCount: 2; elide: Text.ElideRight }
                                Row {
                                    spacing: 6
                                    Rectangle {
                                        width: lblDiff.width + 10; height: 18; radius: 9
                                        color: (modelData.difficulty === "hard" || modelData.difficulty === "HARD") ? Qt.rgba(0.97, 0.32, 0.29, 0.1) : (modelData.difficulty === "easy" || modelData.difficulty === "EASY") ? Qt.rgba(0.25, 0.73, 0.31, 0.1) : Qt.rgba(1, 0.55, 0, 0.1)
                                        Label { id: lblDiff; anchors.centerIn: parent; text: modelData.difficulty || "medium"; font.pixelSize: 9; font.bold: true; color: (modelData.difficulty === "hard" || modelData.difficulty === "HARD") ? th.semanticError : (modelData.difficulty === "easy" || modelData.difficulty === "EASY") ? th.semanticSuccess : th.semanticWarning }
                                    }
                                }
                            }

                            MouseArea {
                                id: delegateArea; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    studentDashboard.selectedChallenge = modelData
                                    if (modelData.starterCode) {
                                        studentDashboard.currentCode = modelData.starterCode
                                        editor.updateCode(modelData.starterCode)
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle { Layout.fillWidth: true; Layout.leftMargin: 16; Layout.rightMargin: 16; height: 1; color: th.borderStone }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 16; Layout.rightMargin: 16; Layout.topMargin: 10; Layout.bottomMargin: 12
                    spacing: 10

                    Rectangle { width: 32; height: 32; radius: 16; color: th.accentLapis
                        Label { anchors.centerIn: parent; text: profileName().length > 0 ? profileName().charAt(0) : "?"; font.pixelSize: 13; font.bold: true; color: th.textInverse }
                    }

                    ColumnLayout { spacing: 0; Layout.fillWidth: true
                        Label { text: profileName(); font.family: th.fontFamilyUI; font.pixelSize: 11; font.bold: true; color: th.textInk; elide: Text.ElideRight; Layout.fillWidth: true }
                        Label { text: loc("teacher_role"); font.pixelSize: 9; color: th.textMuted }
                    }

                    Rectangle {
                        width: 28; height: 28; radius: 14
                        color: logoutBtnArea.containsMouse ? Qt.rgba(0.97, 0.27, 0.27, 0.1) : "transparent"
                        Label { anchors.centerIn: parent; text: "\u2190"; font.pixelSize: 14; color: th.semanticError }
                        MouseArea { id: logoutBtnArea; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: studentDashboard.logout() }
                    }
                }
            }
        }

        // ══════════════════════════════════════════════════════════════════
        // MAIN EDITOR AREA
        // ══════════════════════════════════════════════════════════════════
        ColumnLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 0

            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 50; color: th.backgroundSidebar
                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: th.borderStone }

                RowLayout {
                    anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16; spacing: 12

                    Rectangle { width: 32; height: 32; radius: 16; color: th.accentLapisBg
                        Label { anchors.centerIn: parent; text: "\u0636"; color: th.accentLapis; font.family: th.fontFamilyUI; font.pixelSize: 16; font.bold: true }
                    }

                    ColumnLayout { spacing: 0
                        Label { text: studentDashboard.selectedChallenge ? studentDashboard.selectedChallenge.title : loc("challenges"); font.family: th.fontFamilyUI; font.pixelSize: 14; font.bold: true; color: th.textInk }
                        Label { text: studentDashboard.selectedChallenge ? (studentDashboard.selectedChallenge.description || "") : ""; font.pixelSize: 10; color: th.textSecondary; elide: Text.ElideRight; maximumLineCount: 1; Layout.fillWidth: true; visible: studentDashboard.selectedChallenge !== null }
                    }

                    Item { Layout.fillWidth: true }

                    Rectangle {
                        visible: studentDashboard.selectedChallenge !== null
                        width: verifyRow.width + 24; height: 34; radius: 17
                        color: challengeManager.loading ? th.textMuted : (verifyArea.containsMouse ? Qt.darker(th.semanticSuccess, 1.1) : th.semanticSuccess)
                        opacity: challengeManager.loading ? 0.5 : 1.0

                        Row { id: verifyRow; anchors.centerIn: parent; spacing: 8
                            Label { text: "\u2714"; font.pixelSize: 13; color: th.textInverse; font.bold: true; anchors.verticalCenter: parent.verticalCenter }
                            Label { text: loc("verify_solution"); font.family: th.fontFamilyUI; font.pixelSize: 12; font.bold: true; color: th.textInverse; anchors.verticalCenter: parent.verticalCenter }
                        }
                        MouseArea {
                            id: verifyArea; anchors.fill: parent; hoverEnabled: true; cursorShape: challengeManager.loading ? Qt.BusyCursor : Qt.PointingHandCursor; enabled: !challengeManager.loading
                            onClicked: {
                                if (studentDashboard.selectedChallenge) {
                                    challengeManager.submitSolution(cloudService.userProfile.accessToken || "", studentDashboard.selectedChallenge.id || "", editor.getCode(), "daad")
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true; visible: resultBanner.visible; height: resultBanner.visible ? 36 : 0
                color: resultBanner.color === th.semanticSuccess ? Qt.rgba(0.25, 0.73, 0.31, 0.15) : Qt.rgba(0.97, 0.32, 0.29, 0.15)
                Behavior on height { NumberAnimation { duration: 200 } }
                Label { id: resultBanner; anchors.centerIn: parent; visible: false; font.family: th.fontFamilyUI; font.pixelSize: 13; font.bold: true }
                Timer { id: resultBannerTimer; interval: 5000; onTriggered: resultBanner.visible = false }
            }

            Connections {
                target: challengeManager
                function onSolutionSubmitted(passed, message) {
                    resultBanner.text = passed ? "\u2714 " + message : "\u2718 " + message
                    resultBanner.color = passed ? th.semanticSuccess : th.semanticError
                    resultBanner.visible = true; resultBannerTimer.restart()
                }
            }

            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 30; color: th.backgroundToolbar
                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: th.borderStone }
                Row { anchors.centerIn: parent; spacing: 0
                    Rectangle {
                        width: daadTabLbl.width + 20; height: 26; radius: 8
                        color: editorTab === 0 ? th.accentLapis : "transparent"
                        Label { id: daadTabLbl; anchors.centerIn: parent; text: loc("tab_daad_to_cpp"); font.family: th.fontFamilyUI; font.pixelSize: 11; font.bold: true; color: editorTab === 0 ? th.textInverse : th.textSecondary }
                        MouseArea { anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: editorTab = 0 }
                    }
                    Rectangle { width: 1; height: 16; color: th.borderStone; anchors.verticalCenter: parent.verticalCenter }
                    Rectangle {
                        width: cppDaadTabLbl.width + 20; height: 26; radius: 8
                        color: editorTab === 1 ? th.accentLapis : "transparent"
                        Label { id: cppDaadTabLbl; anchors.centerIn: parent; text: loc("tab_cpp_to_daad"); font.family: th.fontFamilyUI; font.pixelSize: 11; font.bold: true; color: editorTab === 1 ? th.textInverse : th.textSecondary }
                        MouseArea { anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: editorTab = 1 }
                    }
                }
            }

            Item {
                Layout.fillWidth: true; Layout.fillHeight: true
                RowLayout {
                    anchors.fill: parent; spacing: 0

                    Rectangle {
                        Layout.fillHeight: true; Layout.preferredWidth: 260; color: th.backgroundCode
                        ColumnLayout {
                            anchors.fill: parent; spacing: 0
                            Rectangle {
                                Layout.fillWidth: true; Layout.preferredHeight: 32; color: th.backgroundToolbar
                                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: th.borderStone }
                                RowLayout { anchors.fill: parent; anchors.leftMargin: 10; spacing: 4
                                    Rectangle { width: 8; height: 8; radius: 4; color: th.accentGold }
                                    Label { text: "C++"; color: th.textSecondary; font.family: th.fontFamilyCode; font.pixelSize: 11; font.bold: true }
                                    Item { Layout.fillWidth: true }
                                }
                            }
                            Rectangle {
                                Layout.fillWidth: true; Layout.fillHeight: true; color: th.backgroundCode
                                ScrollView {
                                    anchors.fill: parent; clip: true
                                    TextArea {
                                        id: cppViewer; readOnly: true; font.family: th.fontFamilyCode; font.pixelSize: 13
                                        color: th.textInk; selectionColor: th.withAlpha(th.accentLapis, 0.25)
                                        wrapMode: TextArea.Wrap; textFormat: TextEdit.PlainText
                                        LayoutMirroring.enabled: false; horizontalAlignment: TextEdit.AlignLeft; leftPadding: 8
                                        background: Rectangle { color: th.backgroundCode }
                                    }
                                }
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true; Layout.fillHeight: true; spacing: 0

                        EditorPane {
                            id: editor; visible: editorTab === 0
                            onCodeChanged: function(newCode) { studentDashboard.currentCode = newCode }
                        }

                        TextArea {
                            id: cppInputEditor; visible: editorTab === 1
                            Layout.fillWidth: true; Layout.fillHeight: true
                            color: th.textInk; font.family: th.fontFamilyCode; font.pixelSize: 13
                            selectionColor: th.withAlpha(th.accentLapis, 0.25)
                            wrapMode: TextArea.Wrap; textFormat: TextEdit.PlainText
                            LayoutMirroring.enabled: false; horizontalAlignment: TextEdit.AlignLeft; leftPadding: 8
                            placeholderText: loc("cpp_placeholder"); placeholderTextColor: th.textMuted
                            background: Rectangle { color: th.backgroundCode }
                        }

                        OutputPane {
                            id: outputPane
                            Layout.fillWidth: true; Layout.preferredHeight: 150; Layout.minimumHeight: 60
                        }
                    }
                }
            }

            StatusBar { id: statusBar; height: 28 }
        }
    }

    property int editorTab: 0
    property bool fileBrowserVisible: false
    property string currentSource: ""
    property string currentFilePath: ""
}

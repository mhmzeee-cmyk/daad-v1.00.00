import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Item {
    id: teacherDashboard

    property var userProfile: ({})
    property string currentNavIndex: "home"
    property bool showCreateChallenge: false

    signal logout()

    function loc(key) { var _ = translator.languageVersion; return translator.translate(key) }
    function profileName() { return userProfile.name || cloudService.username || "" }
    function schoolName() { return (userProfile.school && userProfile.school.name) || "" }

    Component.onCompleted: {
        challengeManager.fetchChallenges(cloudService.userProfile.accessToken || "")
        challengeManager.fetchSubmissions(cloudService.userProfile.accessToken || "")
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
            Layout.preferredWidth: 220
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
                            Label { text: loc("teacher_role"); font.family: th.fontFamilyUI; font.pixelSize: 10; color: th.textMuted }
                        }
                    }
                }

                Rectangle { Layout.fillWidth: true; Layout.leftMargin: 16; Layout.rightMargin: 16; height: 1; color: th.borderStone }

                Item { Layout.preferredHeight: 12 }

                Repeater {
                    model: [
                        { key: "home",      label: loc("nav_home") },
                        { key: "courses",   label: loc("nav_courses") },
                        { key: "homework",  label: loc("nav_homework") },
                        { key: "classes",   label: loc("nav_virtual_classes") },
                        { key: "students",  label: loc("nav_track_students") },
                        { key: "challenges", label: loc("nav_dhad_challenges") }
                    ]

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 48
                        Layout.leftMargin: 12
                        Layout.rightMargin: 12
                        radius: 12
                        color: teacherDashboard.currentNavIndex === modelData.key ? th.accentLapisBg : (navArea.containsMouse ? th.backgroundHover : "transparent")

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 16
                            anchors.rightMargin: 16
                            spacing: 12

                            Rectangle {
                                width: 8; height: 8; radius: 4
                                color: teacherDashboard.currentNavIndex === modelData.key ? th.accentLapis : th.textMuted
                            }

                            Label {
                                text: modelData.label
                                font.family: th.fontFamilyUI
                                font.pixelSize: 13
                                font.bold: teacherDashboard.currentNavIndex === modelData.key
                                color: teacherDashboard.currentNavIndex === modelData.key ? th.accentLapis : th.textSecondary
                                Layout.fillWidth: true
                            }
                        }

                        MouseArea {
                            id: navArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: teacherDashboard.currentNavIndex = modelData.key
                        }
                    }
                }

                Item { Layout.fillHeight: true }

                Rectangle { Layout.fillWidth: true; Layout.leftMargin: 16; Layout.rightMargin: 16; height: 1; color: th.borderStone }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    Layout.leftMargin: 12; Layout.rightMargin: 12; Layout.bottomMargin: 12
                    radius: 12
                    color: logoutArea.containsMouse ? Qt.rgba(0.97, 0.27, 0.27, 0.08) : "transparent"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 16; anchors.rightMargin: 16
                        spacing: 12
                        Rectangle { width: 8; height: 8; radius: 4; color: th.semanticError }
                        Label { text: loc("logout"); font.family: th.fontFamilyUI; font.pixelSize: 13; color: th.semanticError }
                    }
                    MouseArea { id: logoutArea; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: teacherDashboard.logout() }
                }
            }
        }

        // ══════════════════════════════════════════════════════════════════
        // MAIN CONTENT
        // ══════════════════════════════════════════════════════════════════
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 64
                color: th.backgroundSidebar

                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: th.borderStone }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 24
                    anchors.rightMargin: 24
                    spacing: 16

                    ColumnLayout {
                        spacing: 2
                        Label {
                            text: loc("welcome_teacher") + "\u060C " + profileName()
                            font.family: th.fontFamilyUI; font.pixelSize: 18; font.bold: true; color: th.textInk
                        }
                        Label {
                            text: loc("teacher_panel") + " \u2014 " + schoolName()
                            font.family: th.fontFamilyUI; font.pixelSize: 12; color: th.textMuted
                        }
                    }

                    Item { Layout.fillWidth: true }

                    Rectangle {
                        width: 36; height: 36; radius: 18
                        color: th.accentLapis
                        Label { anchors.centerIn: parent; text: profileName().length > 0 ? profileName().charAt(0) : "\u0636"; font.pixelSize: 16; font.bold: true; color: th.textInverse }
                    }
                }
            }

            ScrollView {
                id: dashScroll
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                contentWidth: availableWidth
                contentHeight: dashboardContent.height + 48

                ColumnLayout {
                    id: dashboardContent
                    width: dashScroll.availableWidth
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 24
                    spacing: 20

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 16

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 90
                            radius: 14
                            color: th.panelBg
                            border.width: 1; border.color: th.borderStone

                            ColumnLayout {
                                anchors.fill: parent; anchors.margins: 14; spacing: 6

                                Label { text: loc("challenges_in_library"); font.family: th.fontFamilyUI; font.pixelSize: 11; color: th.textSecondary; Layout.fillWidth: true }

                                RowLayout {
                                    spacing: 10
                                    Label { text: challengeManager.challenges.length.toString(); font.pixelSize: 32; font.bold: true; color: th.textInk }
                                    Rectangle { width: 60; height: 22; radius: 11; color: th.accentLapisBg
                                        Label { anchors.centerIn: parent; text: loc("browse_btn"); font.pixelSize: 9; font.bold: true; color: th.accentLapis }
                                    }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 90
                            radius: 14
                            color: th.panelBg
                            border.width: 1; border.color: th.borderStone

                            ColumnLayout {
                                anchors.fill: parent; anchors.margins: 14; spacing: 6

                                Label { text: loc("recent_submissions"); font.family: th.fontFamilyUI; font.pixelSize: 11; color: th.textSecondary; Layout.fillWidth: true }

                                RowLayout {
                                    spacing: 10
                                    Label { text: challengeManager.submissions.length.toString(); font.pixelSize: 32; font.bold: true; color: th.textInk }
                                    Rectangle { width: 60; height: 22; radius: 11; color: th.accentGoldMuted
                                        Label { anchors.centerIn: parent; text: loc("submitted"); font.pixelSize: 9; font.bold: true; color: th.accentGold }
                                    }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 90
                            radius: 14
                            color: th.panelBg
                            border.width: 1; border.color: th.borderStone

                            ColumnLayout {
                                anchors.fill: parent; anchors.margins: 14; spacing: 6

                                Label { text: loc("student_performance"); font.family: th.fontFamilyUI; font.pixelSize: 11; color: th.textSecondary; Layout.fillWidth: true }

                                RowLayout {
                                    spacing: 10
                                    Label { text: challengeManager.submissions.length > 0 ? Math.round(challengeManager.submissions.filter(function(s){ return s.passed }).length / challengeManager.submissions.length * 100) + "%" : "--"; font.pixelSize: 32; font.bold: true; color: th.semanticSuccess }
                                    Rectangle { width: 60; height: 22; radius: 11; color: Qt.rgba(0.26, 0.77, 0.37, 0.1)
                                        Label { anchors.centerIn: parent; text: loc("average"); font.pixelSize: 9; font.bold: true; color: th.semanticSuccess }
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: th.borderStone
                    }

                    Label {
                        text: loc("nav_dhad_challenges")
                        font.family: th.fontFamilyUI; font.pixelSize: 15; font.bold: true; color: th.textInk
                    }

                    Repeater {
                        model: challengeManager.challenges

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 64
                            radius: 12
                            color: th.panelBg
                            border.width: 1; border.color: challengeRowArea.containsMouse ? th.accentLapis : th.borderStone

                            RowLayout {
                                anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16; spacing: 12

                                Rectangle { width: 36; height: 36; radius: 18; color: th.accentLapisBg
                                    Label { anchors.centerIn: parent; text: "\u0636"; font.pixelSize: 16; font.bold: true; color: th.accentLapis }
                                }

                                ColumnLayout { spacing: 2; Layout.fillWidth: true
                                    Label { text: modelData.title || ""; font.family: th.fontFamilyUI; font.pixelSize: 13; font.bold: true; color: th.textInk; elide: Text.ElideRight; Layout.fillWidth: true }
                                    Label { text: modelData.description || ""; font.pixelSize: 10; color: th.textSecondary; elide: Text.ElideRight; maximumLineCount: 1; Layout.fillWidth: true }
                                }

                                Rectangle {
                                    width: lblDiff.width + 12; height: 22; radius: 11
                                    color: (modelData.difficulty === "hard" || modelData.difficulty === "HARD") ? Qt.rgba(0.97, 0.32, 0.29, 0.1) : (modelData.difficulty === "easy" || modelData.difficulty === "EASY") ? Qt.rgba(0.25, 0.73, 0.31, 0.1) : Qt.rgba(1, 0.55, 0, 0.1)
                                    Label { id: lblDiff; anchors.centerIn: parent; text: (modelData.difficulty || "medium"); font.pixelSize: 10; font.bold: true; color: (modelData.difficulty === "hard" || modelData.difficulty === "HARD") ? th.semanticError : (modelData.difficulty === "easy" || modelData.difficulty === "EASY") ? th.semanticSuccess : th.semanticWarning }
                                }
                            }

                            MouseArea {
                                id: challengeRowArea; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: th.borderStone
                        visible: challengeManager.challenges.length === 0
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        radius: 12
                        color: th.panelBg
                        border.width: 1; border.color: th.borderStone
                        visible: challengeManager.challenges.length === 0

                        Label {
                            anchors.centerIn: parent
                            text: loc("no_errors")
                            font.family: th.fontFamilyUI; font.pixelSize: 13; color: th.textMuted
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: th.borderStone
                    }

                    Label {
                        text: loc("recent_submissions")
                        font.family: th.fontFamilyUI; font.pixelSize: 15; font.bold: true; color: th.textInk
                    }

                    Repeater {
                        model: challengeManager.submissions

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 56
                            radius: 12
                            color: th.panelBg
                            border.width: 1; border.color: th.borderStone

                            RowLayout {
                                anchors.fill: parent; anchors.leftMargin: 16; anchors.rightMargin: 16; spacing: 12

                                Rectangle { width: 32; height: 32; radius: 16; color: th.accentLapisBg
                                    Label { anchors.centerIn: parent; text: (modelData.student && modelData.student.name) ? modelData.student.name.charAt(0) : "?"; font.pixelSize: 12; font.bold: true; color: th.accentLapis }
                                }

                                ColumnLayout { spacing: 2; Layout.fillWidth: true
                                    Label { text: (modelData.student && modelData.student.name) || ""; font.family: th.fontFamilyUI; font.pixelSize: 12; font.bold: true; color: th.textInk }
                                    Label { text: (modelData.challenge && modelData.challenge.title) || ""; font.pixelSize: 10; color: th.textSecondary; elide: Text.ElideRight; maximumLineCount: 1; Layout.fillWidth: true }
                                }

                                Rectangle {
                                    width: lblSt.width + 12; height: 22; radius: 11
                                    color: modelData.passed ? Qt.rgba(0.25, 0.73, 0.31, 0.1) : Qt.rgba(0.97, 0.32, 0.29, 0.1)
                                    Label { id: lblSt; anchors.centerIn: parent; text: modelData.passed ? loc("status_passed") : loc("loading"); font.pixelSize: 10; font.bold: true; color: modelData.passed ? th.semanticSuccess : th.semanticError }
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: th.borderStone
                    }

                    Label {
                        text: loc("weekly_tasks")
                        font.family: th.fontFamilyUI; font.pixelSize: 15; font.bold: true; color: th.textInk
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 0

                        Repeater {
                            model: [
                                { label: loc("task_submission"), done: true },
                                { label: loc("task_evaluation"), done: true },
                                { label: loc("task_update"), done: false },
                                { label: loc("task_filtering"), done: false },
                                { label: loc("task_enrollment"), done: false },
                                { label: loc("task_modification"), done: false }
                            ]

                            RowLayout {
                                spacing: 0
                                ColumnLayout {
                                    spacing: 6
                                    Rectangle {
                                        width: 32; height: 32; radius: 16
                                        Layout.alignment: Qt.AlignHCenter
                                        color: modelData.done ? th.accentGold : Qt.rgba(0, 0, 0, 0.05)
                                        border.width: modelData.done ? 0 : 2
                                        border.color: th.borderStone
                                        Label { anchors.centerIn: parent; text: modelData.done ? "\u2714" : ""; font.pixelSize: 14; color: th.textInverse }
                                    }
                                    Label { text: modelData.label; font.pixelSize: 10; color: th.textSecondary; Layout.alignment: Qt.AlignHCenter }
                                }
                                Rectangle { Layout.fillWidth: true; height: 2; color: th.borderStone; anchors.verticalCenter: parent.verticalCenter; visible: index < 5 }
                            }
                        }
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone }
                }
            }
        }
    }

    // ══════════════════════════════════════════════════════════════════════
    // CREATE CHALLENGE POPUP (Modal)
    // ══════════════════════════════════════════════════════════════════════
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.4)
        visible: teacherDashboard.showCreateChallenge
        z: 100

        MouseArea { anchors.fill: parent; onClicked: teacherDashboard.showCreateChallenge = false }

        Rectangle {
            anchors.centerIn: parent
            width: 420
            height: popupCol.height + 32
            radius: 20
            color: th.panelBg
            border.width: 1
            border.color: th.borderStone

            MouseArea { anchors.fill: parent }

            ColumnLayout {
                id: popupCol
                anchors.left: parent.left; anchors.right: parent.right
                anchors.top: parent.top; anchors.topMargin: 16
                anchors.leftMargin: 24; anchors.rightMargin: 24
                spacing: 16

                RowLayout {
                    Layout.fillWidth: true
                    Label { text: loc("create_challenge"); font.family: th.fontFamilyUI; font.pixelSize: 16; font.bold: true; color: th.textInk; Layout.fillWidth: true }
                    Rectangle { width: 28; height: 28; radius: 14; color: th.backgroundHover; Label { anchors.centerIn: parent; text: "\u2715"; font.pixelSize: 12; color: th.textSecondary }
                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: teacherDashboard.showCreateChallenge = false }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: th.borderStone }

                ColumnLayout { Layout.fillWidth: true; spacing: 6
                    Label { text: loc("challenge_title"); font.pixelSize: 12; font.bold: true; color: th.textSecondary }
                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 42; radius: 10; color: th.backgroundInput; border.width: 1; border.color: popupTitle.activeFocus ? th.accentLapis : th.borderStone
                        TextInput { id: popupTitle; anchors.fill: parent; anchors.margins: 12; font.family: th.fontFamilyUI; font.pixelSize: 13; color: th.textInk; clip: true; LayoutMirroring.enabled: false }
                    }
                }

                ColumnLayout { Layout.fillWidth: true; spacing: 6
                    Label { text: loc("challenge_description"); font.pixelSize: 12; font.bold: true; color: th.textSecondary }
                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 80; radius: 10; color: th.backgroundInput; border.width: 1; border.color: popupDesc.activeFocus ? th.accentLapis : th.borderStone
                        Flickable { anchors.fill: parent; anchors.margins: 12; contentHeight: popupDesc.implicitHeight; clip: true
                            TextEdit { id: popupDesc; width: parent.width; font.family: th.fontFamilyUI; font.pixelSize: 13; color: th.textInk; wrapMode: TextEdit.Wrap; LayoutMirroring.enabled: false }
                        }
                    }
                }

                ColumnLayout { Layout.fillWidth: true; spacing: 6
                    Label { text: loc("expected_output"); font.pixelSize: 12; font.bold: true; color: th.textSecondary }
                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 42; radius: 10; color: th.backgroundInput; border.width: 1; border.color: popupOutput.activeFocus ? th.accentLapis : th.borderStone
                        TextInput { id: popupOutput; anchors.fill: parent; anchors.margins: 12; font.family: th.fontFamilyCode; font.pixelSize: 13; color: th.textInk; clip: true; LayoutMirroring.enabled: false; horizontalAlignment: TextInput.AlignLeft }
                    }
                }

                ColumnLayout { Layout.fillWidth: true; spacing: 6
                    Label { text: loc("difficulty"); font.pixelSize: 12; font.bold: true; color: th.textSecondary }
                    ComboBox { id: popupDifficulty; Layout.fillWidth: true; model: [loc("difficulty_easy"), loc("difficulty_medium"), loc("difficulty_hard")]; currentIndex: 1 }
                }

                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 44; radius: 12; color: th.accentLapis
                    Label { anchors.centerIn: parent; text: loc("create_challenge"); font.family: th.fontFamilyUI; font.pixelSize: 14; font.bold: true; color: th.textInverse }
                    MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (popupTitle.text.trim() !== "") {
                                challengeManager.createChallenge(cloudService.userProfile.accessToken || "", popupTitle.text.trim(), popupDesc.text.trim(), popupOutput.text.trim(), ["easy", "medium", "hard"][popupDifficulty.currentIndex])
                                teacherDashboard.showCreateChallenge = false
                                popupTitle.text = ""; popupDesc.text = ""; popupOutput.text = ""; popupDifficulty.currentIndex = 1
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 24
        width: 56; height: 56; radius: 28
        color: fabArea.containsMouse ? Qt.darker(th.accentLapis, 1.1) : th.accentLapis
        visible: !teacherDashboard.showCreateChallenge && teacherDashboard.currentNavIndex === "challenges"
        z: 50

        Label { anchors.centerIn: parent; text: "+"; font.pixelSize: 28; font.bold: true; color: th.textInverse }

        MouseArea {
            id: fabArea; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
            onClicked: teacherDashboard.showCreateChallenge = true
        }

        Rectangle {
            anchors.centerIn: parent; width: 72; height: 72; radius: 36
            color: th.accentLapis; opacity: 0.2; z: -1
        }
    }
}

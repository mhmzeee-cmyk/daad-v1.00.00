import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Item {
    id: loginScreen

    property bool isTeacherMode: false
    property string activeAccent: isTeacherMode ? th.accentGold : th.accent
    property bool isLoading: false
    property string errorMessage: ""
    property bool showError: false

    signal loginSuccess(string role, var profile)
    signal loginFailed(string message)

    function resetForm() {
        emailField.text = ""
        passwordField.text = ""
        accessCodeField.text = ""
        errorMessage = ""
        showError = false
        isLoading = false
    }

    function attemptLogin() {
        if (isTeacherMode) {
            if (emailField.text.trim() === "") {
                triggerError("\u0627\u0644\u0628\u0631\u064A\u062F \u0627\u0644\u0625\u0644\u0643\u062A\u0631\u0648\u0646\u064A \u0645\u0637\u0644\u0648\u0628")
                return
            }
            if (passwordField.text === "") {
                triggerError("\u0643\u0644\u0645\u0629 \u0627\u0644\u0645\u0631\u0648\u0631 \u0645\u0637\u0644\u0648\u0628\u0629")
                return
            }
            startLoading()
            cloudService.login(emailField.text.trim(), passwordField.text)
        } else {
            if (emailField.text.trim() === "") {
                triggerError("\u0627\u0644\u0645\u0639\u0631\u0651\u0641 \u0645\u0637\u0644\u0648\u0628")
                return
            }
            if (accessCodeField.text === "") {
                triggerError("\u0631\u0645\u0632 \u0627\u0644\u062F\u062E\u0648\u0644 \u0645\u0637\u0644\u0648\u0628")
                return
            }
            startLoading()
            cloudService.login(emailField.text.trim(), accessCodeField.text)
        }
    }

    function startLoading() {
        isLoading = true
        showError = false
    }

    function triggerError(msg) {
        errorMessage = msg
        showError = true
        isLoading = false
        shakeAnim.start()
        errorTimer.restart()
    }

    Timer { id: errorTimer; interval: 5000; onTriggered: showError = false }

    SequentialAnimation {
        id: shakeAnim
        PropertyAnimation { target: loginCard; property: "x"; from: loginCard.x - 8; to: loginCard.x + 8; duration: 60; easing.type: Easing.InOutQuad }
        PropertyAnimation { target: loginCard; property: "x"; from: loginCard.x + 8; to: loginCard.x - 6; duration: 60; easing.type: Easing.InOutQuad }
        PropertyAnimation { target: loginCard; property: "x"; from: loginCard.x - 6; to: loginCard.x + 4; duration: 60; easing.type: Easing.InOutQuad }
        PropertyAnimation { target: loginCard; property: "x"; from: loginCard.x + 4; to: loginCard.x - 2; duration: 60; easing.type: Easing.InOutQuad }
        PropertyAnimation { target: loginCard; property: "x"; from: loginCard.x - 2; to: loginCard.x; duration: 60; easing.type: Easing.InOutQuad }
    }

    Rectangle { anchors.fill: parent; color: th.backgroundPaper }

    Rectangle {
        anchors.centerIn: parent
        width: 600; height: 600; radius: 300
        color: loginScreen.isTeacherMode ? Qt.rgba(0.64, 0.81, 0.28, 0.06) : Qt.rgba(0.30, 0.73, 0.92, 0.06)
    }

    Flickable {
        anchors.fill: parent
        contentHeight: mainCol.height + 60
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ColumnLayout {
            id: mainCol
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 80
            width: Math.min(parent.width - 40, 380)
            spacing: 0

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 90

                Row {
                    anchors.centerIn: parent
                    spacing: 2
                    Label { text: "{"; font.family: th.fontFamilyCode; font.pixelSize: 68; font.bold: true; color: th.accent; anchors.baseline: dhad.baseline }
                    Label { id: dhad; text: "\u0636"; font.family: th.fontFamilyUI; font.pixelSize: 68; font.bold: true; color: th.textPrimary }
                    Label { text: "}"; font.family: th.fontFamilyCode; font.pixelSize: 68; font.bold: true; color: th.accentGold; anchors.baseline: dhad.baseline }
                }
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "\u0636 \u0627\u0633\u062A\u062F\u064A\u0648"
                font.family: th.fontFamilyUI; font.pixelSize: 24; font.bold: true; color: th.textInk
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 2
                text: "DHAD STUDIO"
                font.family: th.fontFamilyCode; font.pixelSize: 11; font.letterSpacing: 4
                color: loginScreen.activeAccent; opacity: 0.6
            }

            Item { Layout.preferredHeight: 36 }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 46
                radius: 23
                color: Qt.rgba(0, 0, 0, 0.03)
                border.width: 1
                border.color: th.borderStone

                Row {
                    anchors.fill: parent; anchors.margins: 3
                    layoutDirection: Qt.RightToLeft; spacing: 0

                    Rectangle {
                        width: (parent.width - 6) / 2; height: parent.height - 6; radius: 20
                        color: loginScreen.isTeacherMode ? Qt.rgba(0.64, 0.81, 0.28, 0.15) : "transparent"
                        Behavior on color { ColorAnimation { duration: 200 } }
                        Column {
                            anchors.centerIn: parent; spacing: 1
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: "\u0645\u0639\u0644\u0645"; font.family: th.fontFamilyUI; font.pixelSize: 14; font.bold: true; color: loginScreen.isTeacherMode ? th.accentGold : th.textMuted }
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: "Teacher"; font.family: th.fontFamilyUI; font.pixelSize: 9; color: loginScreen.isTeacherMode ? th.textSecondary : th.textMuted }
                        }
                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { if (!loginScreen.isTeacherMode) { loginScreen.isTeacherMode = true; loginScreen.resetForm() } } }
                    }

                    Rectangle {
                        width: (parent.width - 6) / 2; height: parent.height - 6; radius: 20
                        color: !loginScreen.isTeacherMode ? Qt.rgba(0.30, 0.73, 0.92, 0.15) : "transparent"
                        Behavior on color { ColorAnimation { duration: 200 } }
                        Column {
                            anchors.centerIn: parent; spacing: 1
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: "\u0637\u0627\u0644\u0628"; font.family: th.fontFamilyUI; font.pixelSize: 14; font.bold: true; color: !loginScreen.isTeacherMode ? th.accent : th.textMuted }
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: "Student"; font.family: th.fontFamilyUI; font.pixelSize: 9; color: !loginScreen.isTeacherMode ? th.textSecondary : th.textMuted }
                        }
                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { if (loginScreen.isTeacherMode) { loginScreen.isTeacherMode = false; loginScreen.resetForm() } } }
                    }
                }
            }

            Item { Layout.preferredHeight: 32 }

            Rectangle {
                id: loginCard
                Layout.fillWidth: true
                implicitHeight: cardCol.implicitHeight + 28
                radius: 14
                color: th.backgroundPaper
                border.width: 1
                border.color: th.borderStone

                ColumnLayout {
                    id: cardCol
                    anchors.left: parent.left; anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 14; anchors.rightMargin: 14
                    spacing: 12

                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 46; radius: 10
                        color: th.backgroundInput
                        border.width: 1
                        border.color: emailField.activeFocus ? loginScreen.activeAccent : th.borderStone
                        Behavior on border.color { ColorAnimation { duration: 200 } }

                        RowLayout {
                            anchors.fill: parent; anchors.leftMargin: 14; anchors.rightMargin: 14; spacing: 10
                            Label { text: loginScreen.isTeacherMode ? "\u2709" : "\u263A"; font.pixelSize: 16; color: emailField.activeFocus ? loginScreen.activeAccent : th.textMuted }
                            TextField {
                                id: emailField; Layout.fillWidth: true; Layout.fillHeight: true
                                placeholderText: loginScreen.isTeacherMode ? "\u0627\u0644\u0628\u0631\u064A\u062F \u0627\u0644\u0625\u0644\u0643\u062A\u0631\u0648\u0646\u064A" : "\u0627\u0644\u0645\u0639\u0631\u0651\u0641 \u0627\u0644\u0625\u0644\u0643\u062A\u0631\u0648\u0646\u064A"
                                placeholderTextColor: th.textMuted; color: th.textInk
                                font.family: th.fontFamilyUI; font.pixelSize: 14; background: null
                                leftPadding: 0; rightPadding: 0; topPadding: 0; bottomPadding: 0
                                selectByMouse: true; selectionColor: loginScreen.activeAccent
                                onAccepted: loginScreen.attemptLogin()
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 46; radius: 10
                        color: th.backgroundInput
                        border.width: 1
                        border.color: passwordField.activeFocus ? loginScreen.activeAccent : th.borderStone
                        visible: loginScreen.isTeacherMode
                        Behavior on border.color { ColorAnimation { duration: 200 } }

                        RowLayout {
                            anchors.fill: parent; anchors.leftMargin: 14; anchors.rightMargin: 14; spacing: 10
                            Label { text: "\uD83D\uDD10"; font.pixelSize: 16; color: passwordField.activeFocus ? loginScreen.activeAccent : th.textMuted }
                            TextField {
                                id: passwordField; Layout.fillWidth: true; Layout.fillHeight: true
                                placeholderText: "\u0643\u0644\u0645\u0629 \u0627\u0644\u0645\u0631\u0648\u0631"
                                placeholderTextColor: th.textMuted; color: th.textInk
                                font.family: th.fontFamilyUI; font.pixelSize: 14; background: null
                                leftPadding: 0; rightPadding: 0; topPadding: 0; bottomPadding: 0
                                echoMode: TextInput.Password; passwordCharacter: "\u25CF"; passwordMaskDelay: 300
                                selectByMouse: true; selectionColor: loginScreen.activeAccent
                                onAccepted: loginScreen.attemptLogin()
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 46; radius: 10
                        color: th.backgroundInput
                        border.width: 1
                        border.color: accessCodeField.activeFocus ? loginScreen.activeAccent : th.borderStone
                        visible: !loginScreen.isTeacherMode
                        Behavior on border.color { ColorAnimation { duration: 200 } }

                        RowLayout {
                            anchors.fill: parent; anchors.leftMargin: 14; anchors.rightMargin: 14; spacing: 10
                            Label { text: "\u2738"; font.pixelSize: 16; color: accessCodeField.activeFocus ? loginScreen.activeAccent : th.textMuted }
                            TextField {
                                id: accessCodeField; Layout.fillWidth: true; Layout.fillHeight: true
                                placeholderText: "\u0631\u0645\u0632 \u0627\u0644\u062F\u062E\u0648\u0644 \u0627\u0644\u0645\u062F\u0631\u0633\u064A"
                                placeholderTextColor: th.textMuted; color: th.textInk
                                font.family: th.fontFamilyUI; font.pixelSize: 14; background: null
                                leftPadding: 0; rightPadding: 0; topPadding: 0; bottomPadding: 0
                                echoMode: TextInput.Password; passwordCharacter: "\u25CF"; passwordMaskDelay: 300
                                selectByMouse: true; selectionColor: loginScreen.activeAccent
                                onAccepted: loginScreen.attemptLogin()
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: loginScreen.showError ? 34 : 0
                        clip: true; visible: loginScreen.showError
                        Behavior on Layout.preferredHeight { NumberAnimation { duration: 250 } }
                        Rectangle {
                            anchors.fill: parent; radius: 8
                            color: Qt.rgba(0.97, 0.32, 0.29, 0.1)
                            border.width: 1; border.color: Qt.rgba(0.97, 0.32, 0.29, 0.25)
                            RowLayout {
                                anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10; spacing: 6
                                Label { text: "\u26A0"; font.pixelSize: 13; color: th.semanticError }
                                Label { Layout.fillWidth: true; text: loginScreen.errorMessage; font.family: th.fontFamilyUI; font.pixelSize: 11; color: th.semanticError; elide: Text.ElideRight }
                            }
                        }
                    }
                }
            }

            Item { Layout.preferredHeight: 20 }

            Rectangle {
                Layout.fillWidth: true; Layout.preferredHeight: 50; radius: 12
                color: loginScreen.isLoading ? Qt.rgba(0, 0, 0, 0.05) : loginScreen.activeAccent

                Label {
                    anchors.centerIn: parent
                    text: "\u27A4  \u062A\u0633\u062C\u064A\u0644 \u0627\u0644\u062F\u062E\u0648\u0644"
                    font.family: th.fontFamilyUI; font.pixelSize: 15; font.bold: true; color: "#ffffff"
                    visible: !loginScreen.isLoading
                }

                Item {
                    anchors.centerIn: parent; width: 28; height: 28
                    visible: loginScreen.isLoading

                    Repeater {
                        model: 8
                        Rectangle {
                            property real angle: index / 8 * 2 * Math.PI
                            property real dist: 10
                            width: 4; height: 4; radius: 2; color: "#ffffff"
                            x: 14 + dist * Math.cos(angle) - width / 2
                            y: 14 + dist * Math.sin(angle) - height / 2
                            opacity: 0.15 + (index / 8) * 0.85

                            SequentialAnimation on opacity {
                                loops: Animation.Infinite; running: loginScreen.isLoading
                                NumberAnimation { from: 0.15; to: 1.0; duration: (index / 8) * 1000 }
                                NumberAnimation { from: 1.0; to: 0.15; duration: (1 - index / 8) * 1000 }
                            }
                        }
                    }

                    Rectangle {
                        anchors.centerIn: parent; width: 28; height: 28; radius: 14
                        color: "transparent"; border.width: 2; border.color: "#ffffff"; opacity: 0.15
                        RotationAnimation on rotation { from: 0; to: 360; duration: 2000; loops: Animation.Infinite; running: loginScreen.isLoading }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: loginScreen.isLoading ? Qt.WaitCursor : Qt.PointingHandCursor
                    enabled: !loginScreen.isLoading
                    onClicked: loginScreen.attemptLogin()
                }
            }

            Item { Layout.preferredHeight: 16 }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "\u0627\u0644\u0648\u0636\u0639 \u0628\u062F\u0648\u0646 \u0627\u062A\u0635\u0627\u0644"
                font.family: th.fontFamilyUI; font.pixelSize: 12; color: th.textMuted
                Layout.bottomMargin: 20

                MouseArea {
                    anchors.fill: parent; anchors.margins: -10; cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        cloudService.setOfflineMode(true)
                        loginScreen.loginSuccess("offline", null)
                    }
                }
                SequentialAnimation on color {
                    loops: Animation.Infinite
                    ColorAnimation { from: th.textMuted; to: th.textSecondary; duration: 2000 }
                    ColorAnimation { from: th.textSecondary; to: th.textMuted; duration: 2000 }
                }
            }
        }
    }

    Connections {
        target: cloudService
        function onLoginSuccess(role, profile) {
            loginScreen.isLoading = false
            loginScreen.loginSuccess(role, profile)
        }
        function onLoginFailed(message) {
            loginScreen.triggerError(message)
        }
        function onNetworkError(errorString) {
            loginScreen.triggerError("\u062E\u0637\u0623 \u0641\u064A \u0627\u0644\u0634\u0628\u0643\u0629: " + errorString)
        }
    }
}

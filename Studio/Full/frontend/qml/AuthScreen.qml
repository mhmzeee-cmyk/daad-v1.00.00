import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Item {
    id: authScreen

    property bool isTeacherMode: false
    property string activeAccent: isTeacherMode ? "#ff8c00" : "#00f2ff"
    property bool isLoading: false
    property string errorMessage: ""
    property bool showError: false

    signal loginSuccess(string role, var userProfile)
    signal loginFailed(string message)

    function resetForm() {
        emailField.text = ""
        passwordField.text = ""
        accessCodeField.text = ""
        errorMessage = ""
        showError = false
        isLoading = false
    }

    function showLoading() { isLoading = true; showError = false }
    function hideLoading() { isLoading = false }

    function displayError(message) {
        errorMessage = message
        showError = true
        isLoading = false
        errorTimer.restart()
    }

    function attemptLogin() {
        if (isTeacherMode) {
            if (emailField.text.trim() === "") { displayError("\u0627\u0644\u0628\u0631\u064A\u062F \u0627\u0644\u0625\u0644\u0643\u062A\u0631\u0648\u0646\u064A \u0645\u0637\u0644\u0648\u0628"); return }
            if (passwordField.text === "") { displayError("\u0643\u0644\u0645\u0629 \u0627\u0644\u0645\u0631\u0648\u0631 \u0645\u0637\u0644\u0648\u0628\u0629"); return }
            showLoading()
            cloudService.login(emailField.text.trim(), passwordField.text)
        } else {
            if (emailField.text.trim() === "") { displayError("\u0627\u0644\u0645\u0639\u0631\u0651\u0641 \u0645\u0637\u0644\u0648\u0628"); return }
            if (accessCodeField.text === "") { displayError("\u0631\u0645\u0632 \u0627\u0644\u062F\u062E\u0648\u0644 \u0645\u0637\u0644\u0648\u0628"); return }
            showLoading()
            cloudService.login(emailField.text.trim(), accessCodeField.text)
        }
    }

    Timer { id: errorTimer; interval: 5000; onTriggered: showError = false }

    // ── Background ───────────────────────────────────────────────────────
    Rectangle { anchors.fill: parent; color: "#0a0f1e" }

    // ── Subtle radial glow ──────────────────────────────────────────────
    Rectangle {
        anchors.centerIn: parent
        width: 600; height: 600; radius: 300
        color: authScreen.isTeacherMode ? Qt.rgba(1, 0.55, 0, 0.04) : Qt.rgba(0, 0.95, 1, 0.04)
    }

    // ── Main content (Flickable for small screens) ──────────────────────
    Flickable {
        anchors.fill: parent
        contentHeight: mainCol.height + 40
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ColumnLayout {
            id: mainCol
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 90
            width: Math.min(parent.width - 40, 380)
            spacing: 0

            // ── Logo {ض} ────────────────────────────────────────────────
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 90

                Row {
                    anchors.centerIn: parent
                    spacing: 2

                    Label {
                        text: "{"
                        font.family: th.fontFamilyCode
                        font.pixelSize: 68
                        font.bold: true
                        color: "#00f2ff"
                        anchors.baseline: dhad.baseline
                    }
                    Label {
                        id: dhad
                        text: "\u0636"
                        font.family: th.fontFamilyUI
                        font.pixelSize: 68
                        font.bold: true
                        color: Qt.rgba(0, 0.9, 1, 0.8)
                    }
                    Label {
                        text: "}"
                        font.family: th.fontFamilyCode
                        font.pixelSize: 68
                        font.bold: true
                        color: "#ff8c00"
                        anchors.baseline: dhad.baseline
                    }
                }
            }

            Item { Layout.preferredHeight: 10 }

            // ── App name ────────────────────────────────────────────────
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "\u0636 \u0627\u0633\u062A\u062F\u064A\u0648"
                font.family: th.fontFamilyUI
                font.pixelSize: 24
                font.bold: true
                color: "#ffffff"
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 2
                text: "DHAD STUDIO"
                font.family: th.fontFamilyCode
                font.pixelSize: 11
                font.letterSpacing: 4
                color: authScreen.activeAccent
                opacity: 0.5
            }

            Item { Layout.preferredHeight: 40 }

            // ── Mode switcher ───────────────────────────────────────────
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 46
                radius: 23
                color: Qt.rgba(1, 1, 1, 0.04)
                border.width: 1
                border.color: Qt.rgba(1, 1, 1, 0.06)

                Row {
                    anchors.fill: parent
                    anchors.margins: 3
                    layoutDirection: Qt.RightToLeft
                    spacing: 0

                    Rectangle {
                        width: (parent.width - 6) / 2
                        height: parent.height - 6
                        radius: 20
                        color: authScreen.isTeacherMode ? Qt.rgba(1, 0.55, 0, 0.2) : "transparent"
                        Behavior on color { ColorAnimation { duration: 200 } }

                        Column {
                            anchors.centerIn: parent
                            spacing: 1
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: "\u0645\u0639\u0644\u0645"; font.family: th.fontFamilyUI; font.pixelSize: 14; font.bold: true; color: authScreen.isTeacherMode ? "#ff8c00" : Qt.rgba(1, 1, 1, 0.35) }
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: "Teacher"; font.family: th.fontFamilyUI; font.pixelSize: 9; color: authScreen.isTeacherMode ? Qt.rgba(1, 0.55, 0, 0.7) : Qt.rgba(1, 1, 1, 0.2) }
                        }
                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { if (!authScreen.isTeacherMode) { authScreen.isTeacherMode = true; authScreen.resetForm() } } }
                    }

                    Rectangle {
                        width: (parent.width - 6) / 2
                        height: parent.height - 6
                        radius: 20
                        color: !authScreen.isTeacherMode ? Qt.rgba(0, 0.95, 1, 0.2) : "transparent"
                        Behavior on color { ColorAnimation { duration: 200 } }

                        Column {
                            anchors.centerIn: parent
                            spacing: 1
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: "\u0637\u0627\u0644\u0628"; font.family: th.fontFamilyUI; font.pixelSize: 14; font.bold: true; color: !authScreen.isTeacherMode ? "#00f2ff" : Qt.rgba(1, 1, 1, 0.35) }
                            Label { anchors.horizontalCenter: parent.horizontalCenter; text: "Student"; font.family: th.fontFamilyUI; font.pixelSize: 9; color: !authScreen.isTeacherMode ? Qt.rgba(0, 0.95, 1, 0.7) : Qt.rgba(1, 1, 1, 0.2) }
                        }
                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { if (authScreen.isTeacherMode) { authScreen.isTeacherMode = false; authScreen.resetForm() } } }
                    }
                }
            }

            Item { Layout.preferredHeight: 36 }

            // ── Form card ───────────────────────────────────────────────
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: formCol.implicitHeight + 28
                radius: 14
                color: Qt.rgba(1, 1, 1, 0.03)
                border.width: 1
                border.color: Qt.rgba(1, 1, 1, 0.06)

                ColumnLayout {
                    id: formCol
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    spacing: 12

                    // ── Email / ID field ─────────────────────────────────
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 46
                        radius: 10
                        color: Qt.rgba(1, 1, 1, 0.05)
                        border.width: 1
                        border.color: emailField.activeFocus ? authScreen.activeAccent : Qt.rgba(1, 1, 1, 0.08)
                        Behavior on border.color { ColorAnimation { duration: 200 } }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 14; anchors.rightMargin: 14
                            spacing: 10

                            Label {
                                text: authScreen.isTeacherMode ? "\u2709" : "\u263A"
                                font.pixelSize: 16
                                color: emailField.activeFocus ? authScreen.activeAccent : Qt.rgba(1, 1, 1, 0.25)
                            }
                            TextField {
                                id: emailField
                                Layout.fillWidth: true; Layout.fillHeight: true
                                placeholderText: authScreen.isTeacherMode ? "\u0627\u0644\u0628\u0631\u064A\u062F \u0627\u0644\u0625\u0644\u0643\u062A\u0631\u0648\u0646\u064A" : "\u0627\u0644\u0645\u0639\u0631\u0651\u0641 \u0627\u0644\u0625\u0644\u0643\u062A\u0631\u0648\u0646\u064A"
                                placeholderTextColor: Qt.rgba(1, 1, 1, 0.2)
                                color: "#ffffff"
                                font.family: th.fontFamilyUI; font.pixelSize: 14
                                background: null
                                leftPadding: 0; rightPadding: 0; topPadding: 0; bottomPadding: 0
                                selectByMouse: true
                                selectionColor: authScreen.activeAccent
                                onAccepted: authScreen.attemptLogin()
                            }
                        }
                    }

                    // ── Password field (Teacher) ────────────────────────
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 46
                        radius: 10
                        color: Qt.rgba(1, 1, 1, 0.05)
                        border.width: 1
                        border.color: passwordField.activeFocus ? authScreen.activeAccent : Qt.rgba(1, 1, 1, 0.08)
                        visible: authScreen.isTeacherMode
                        Behavior on border.color { ColorAnimation { duration: 200 } }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 14; anchors.rightMargin: 14
                            spacing: 10

                            Label {
                                text: "\uD83D\uDD10"
                                font.pixelSize: 16
                                color: passwordField.activeFocus ? authScreen.activeAccent : Qt.rgba(1, 1, 1, 0.25)
                            }
                            TextField {
                                id: passwordField
                                Layout.fillWidth: true; Layout.fillHeight: true
                                placeholderText: "\u0643\u0644\u0645\u0629 \u0627\u0644\u0645\u0631\u0648\u0631"
                                placeholderTextColor: Qt.rgba(1, 1, 1, 0.2)
                                color: "#ffffff"
                                font.family: th.fontFamilyUI; font.pixelSize: 14
                                background: null
                                leftPadding: 0; rightPadding: 0; topPadding: 0; bottomPadding: 0
                                echoMode: TextInput.Password
                                passwordCharacter: "\u25CF"
                                passwordMaskDelay: 300
                                selectByMouse: true
                                selectionColor: authScreen.activeAccent
                                onAccepted: authScreen.attemptLogin()
                            }
                        }
                    }

                    // ── Access Code field (Student) ─────────────────────
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 46
                        radius: 10
                        color: Qt.rgba(1, 1, 1, 0.05)
                        border.width: 1
                        border.color: accessCodeField.activeFocus ? authScreen.activeAccent : Qt.rgba(1, 1, 1, 0.08)
                        visible: !authScreen.isTeacherMode
                        Behavior on border.color { ColorAnimation { duration: 200 } }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 14; anchors.rightMargin: 14
                            spacing: 10

                            Label {
                                text: "\u2738"
                                font.pixelSize: 16
                                color: accessCodeField.activeFocus ? authScreen.activeAccent : Qt.rgba(1, 1, 1, 0.25)
                            }
                            TextField {
                                id: accessCodeField
                                Layout.fillWidth: true; Layout.fillHeight: true
                                placeholderText: "\u0631\u0645\u0632 \u0627\u0644\u062F\u062E\u0648\u0644 \u0627\u0644\u0645\u062F\u0631\u0633\u064A"
                                placeholderTextColor: Qt.rgba(1, 1, 1, 0.2)
                                color: "#ffffff"
                                font.family: th.fontFamilyUI; font.pixelSize: 14
                                background: null
                                leftPadding: 0; rightPadding: 0; topPadding: 0; bottomPadding: 0
                                echoMode: TextInput.Password
                                passwordCharacter: "\u25CF"
                                passwordMaskDelay: 300
                                selectByMouse: true
                                selectionColor: authScreen.activeAccent
                                onAccepted: authScreen.attemptLogin()
                            }
                        }
                    }

                    // ── Error bar ────────────────────────────────────────
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: authScreen.showError ? 34 : 0
                        clip: true
                        visible: authScreen.showError
                        Behavior on Layout.preferredHeight { NumberAnimation { duration: 250 } }

                        Rectangle {
                            anchors.fill: parent
                            radius: 8
                            color: Qt.rgba(0.97, 0.2, 0.2, 0.12)
                            border.width: 1
                            border.color: Qt.rgba(0.97, 0.2, 0.2, 0.3)

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 10; anchors.rightMargin: 10
                                spacing: 6
                                Label { text: "\u26A0"; font.pixelSize: 13; color: "#ff4444" }
                                Label { Layout.fillWidth: true; text: authScreen.errorMessage; font.family: th.fontFamilyUI; font.pixelSize: 11; color: "#ff6666"; elide: Text.ElideRight }
                            }
                        }
                    }
                }
            }

            Item { Layout.preferredHeight: 24 }

            // ── Login button ────────────────────────────────────────────
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 50
                radius: 12
                color: "transparent"
                border.width: 2
                border.color: authScreen.isLoading ? Qt.rgba(0.5, 0.5, 0.5, 0.3) : authScreen.activeAccent

                Rectangle {
                    anchors.fill: parent; anchors.margins: 2
                    radius: 10
                    color: authScreen.isLoading ? Qt.rgba(0.5, 0.5, 0.5, 0.15) : authScreen.activeAccent
                    Behavior on color { ColorAnimation { duration: 250 } }
                }

                // Glow
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width + 40; height: parent.height + 40; radius: 32
                    color: authScreen.activeAccent
                    opacity: authScreen.isLoading ? 0 : 0.12
                    visible: !authScreen.isLoading
                    z: -1
                }

                Label {
                    anchors.centerIn: parent
                    text: authScreen.isLoading ? "" : "\u27A4  \u062A\u0633\u062C\u064A\u0644 \u0627\u0644\u062F\u062E\u0648\u0644"
                    font.family: th.fontFamilyUI; font.pixelSize: 15; font.bold: true
                    color: "#ffffff"
                    visible: !authScreen.isLoading
                }

                // ── Dot-circle spinner (visible only when loading) ───────
                Item {
                    anchors.centerIn: parent
                    width: 28; height: 28
                    visible: authScreen.isLoading

                    Repeater {
                        model: 8
                        Rectangle {
                            property real angle: index / 8 * 2 * Math.PI
                            property real dist: 10
                            width: 4; height: 4; radius: 2
                            color: "#ffffff"
                            x: 14 + dist * Math.cos(angle) - width / 2
                            y: 14 + dist * Math.sin(angle) - height / 2
                            opacity: 0.15 + (index / 8) * 0.85

                            SequentialAnimation on opacity {
                                loops: Animation.Infinite
                                running: authScreen.isLoading
                                NumberAnimation { from: 0.15; to: 1.0; duration: (index / 8) * 1000 }
                                NumberAnimation { from: 1.0; to: 0.15; duration: (1 - index / 8) * 1000 }
                            }
                        }
                    }

                    Rectangle {
                        anchors.centerIn: parent
                        width: 28; height: 28; radius: 14
                        color: "transparent"
                        border.width: 2
                        border.color: "#ffffff"
                        opacity: 0.15

                        RotationAnimation on rotation {
                            from: 0; to: 360
                            duration: 2000
                            loops: Animation.Infinite
                            running: authScreen.isLoading
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: authScreen.isLoading ? Qt.WaitCursor : Qt.PointingHandCursor
                    enabled: !authScreen.isLoading
                    onClicked: authScreen.attemptLogin()
                }
            }

            Item { Layout.preferredHeight: 20 }

            // ── Offline mode ────────────────────────────────────────────
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "\u0627\u0644\u0648\u0636\u0639 \u0628\u062F\u0648\u0646 \u0627\u062A\u0635\u0627\u0644"
                font.family: th.fontFamilyUI; font.pixelSize: 12
                color: Qt.rgba(1, 1, 1, 0.25)
                Layout.bottomMargin: 20

                MouseArea {
                    anchors.fill: parent; anchors.margins: -10
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        cloudService.setOfflineMode(true)
                        authScreen.loginSuccess("offline", null)
                    }
                }

                SequentialAnimation on color {
                    loops: Animation.Infinite
                    ColorAnimation { from: Qt.rgba(1, 1, 1, 0.25); to: Qt.rgba(1, 1, 1, 0.45); duration: 2000 }
                    ColorAnimation { from: Qt.rgba(1, 1, 1, 0.45); to: Qt.rgba(1, 1, 1, 0.25); duration: 2000 }
                }
            }
        }
    }

    // ── Cloud service connections ────────────────────────────────────────
    Connections {
        target: cloudService
        function onAuthenticationSuccess() {
            authScreen.hideLoading()
            var profile = cloudService.getCurrentUserProfile()
            authScreen.loginSuccess(profile.role, profile)
        }
        function onAuthenticationFailure(result, message) {
            var displayMsg = message
            if (result === CloudNetworkService.AUTH_INVALID_CREDENTIALS) displayMsg = "\u062E\u0637\u0623 \u0641\u064A \u0627\u0644\u0628\u064A\u0627\u0646\u0627\u062A"
            else if (result === CloudNetworkService.AUTH_NETWORK_ERROR) displayMsg = "\u062E\u0637\u0623 \u0641\u064A \u0627\u0644\u0627\u062A\u0635\u0627\u0644"
            else if (result === CloudNetworkService.AUTH_TOKEN_EXPIRED) displayMsg = "\u0627\u0646\u062A\u0647\u062A \u0635\u0644\u0627\u062D\u064A\u0629 \u0627\u0644\u062C\u0644\u0633\u0629"
            authScreen.displayError(displayMsg)
        }
        function onNetworkError(error, message) {
            authScreen.displayError("\u062E\u0637\u0623 \u0641\u064A \u0627\u0644\u0634\u0628\u0643\u0629: " + message)
        }
    }
}

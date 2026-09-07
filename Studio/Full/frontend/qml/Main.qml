import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0
import DaadStudio 1.0

ApplicationWindow {
    id: root
    width: 1400
    height: 900
    visible: true
    title: qsTr("Daad Studio - " + appVersion)
    color: th.backgroundPaper
    minimumWidth: 900
    minimumHeight: 600

    ThemeManager { id: th }

    property string currentScreen: "login"
    property var userProfile: ({})

    function loc(key) { var _ = translator.languageVersion; return translator.translate(key) }
    function forceCppLtr(text) {
        var lines = text.split("\\n")
        var result = ""
        for (var i = 0; i < lines.length; i++) {
            var line = lines[i]
            line = line.replace(/([\u0600-\u06FF\u0750-\u077F\u08A0-\u08FF\uFB50-\uFDFF\uFE70-\uFEFF]+)/g, '$1\u200E')
            result += line
            if (i < lines.length - 1) result += "\\n"
        }
        return result
    }

    property alias compiler: _compiler
    Compiler { id: _compiler }
    property alias reverseTranspiler: _reverseTranspiler
    CppToDaadTranspiler { id: _reverseTranspiler }

    function navigateTo(role, profile) {
        root.userProfile = profile || {}
        if (role === "teacher" || role === "TEACHER") {
            root.currentScreen = "teacher"
        } else {
            root.currentScreen = "student"
        }
    }

    function navigateToLogin() {
        cloudService.logout()
        root.currentScreen = "login"
        root.userProfile = {}
    }

    Loader {
        id: screenLoader
        anchors.fill: parent
        sourceComponent: {
            switch (root.currentScreen) {
                case "login": return loginScreenComp
                case "student": return studentComp
                case "teacher": return teacherComp
                default: return loginScreenComp
            }
        }

        onItemChanged: {
            if (item && item.cppViewer) {
                item.forceActiveFocus()
            }
        }
    }

    Connections {
        target: _compiler
        function onCompilationFinished(success, headerOutput, sourceOutput, diagnostics) {
            if (screenLoader.item && screenLoader.item.cppViewer)
                screenLoader.item.cppViewer.text = forceCppLtr(headerOutput + "\\n" + sourceOutput)
        }
    }

    Connections {
        target: _reverseTranspiler
        function onTranspileFinished(success, daadOutput, message) {
            if (success && screenLoader.item && screenLoader.item.editor)
                screenLoader.item.editor.updateCode(daadOutput)
        }
    }

    Component {
        id: loginScreenComp
        LoginScreen {
            onLoginSuccess: function(role, profile) {
                root.navigateTo(role, profile)
            }
        }
    }

    Component {
        id: studentComp
        StudentDashboard {
            userProfile: root.userProfile
            onLogout: root.navigateToLogin()
        }
    }

    Component {
        id: teacherComp
        TeacherDashboard {
            userProfile: root.userProfile
            onLogout: root.navigateToLogin()
        }
    }
}

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0
import Qt.labs.platform 1.0
import DaadStudio 1.0

Pane {
    id: fileExplorer
    function loc(key) { var _ = translator.languageVersion; return translator ? translator.translate(key) : key }
    

    signal fileSelected(string path, string content)

    property string currentFolderPath: ""
    property string selectedFilePath: ""
    property alias fsModel: fsModel

    FileSystemModel {
        id: fsModel
        onFileOpened: function(path, content) {
            fileExplorer.fileSelected(path, content)
        }
        onErrorOccurred: function(msg) {
            if (typeof statusBar !== "undefined") statusBar.showMessage(msg)
        }
        onFolderChanged: {
            fileExplorer.refreshTree()
        }
    }

    function refreshTree() {
        fileListModel.clear()
        if (currentFolderPath.length === 0) return
        var items = fsModel.getFolderContents(currentFolderPath)
        if (typeof statusBar !== "undefined")
            statusBar.showMessage(currentFolderPath + " (" + items.length + " items)")
        for (var i = 0; i < items.length; i++) {
            fileListModel.append({
                name: items[i].name,
                path: items[i].path,
                isDir: items[i].isDir,
                icon: items[i].icon,
                size: items[i].size || 0,
                modified: items[i].modified || ""
            })
        }
    }

    function openFolder(path) {
        if (path && path.length > 0) {
            if (/^[A-Za-z]:$/.test(path)) {
                path += "\\"
            }
            currentFolderPath = path
            selectedFilePath = ""
            fsModel.rootPath = path
            refreshTree()
        }
    }

    FileDialog { fileMode: FileDialog.SelectFolder
        id: folderPicker
        title: loc("browse_folder")
        onAccepted: { var url = fileUrl.toString()
            var path = url
            if (Qt.platform.os === "windows") {
                if (path.startsWith("file:///")) {
                    path = path.substring(8)
                }
                if (path.charAt(0) === '/' && path.length > 2 && path.charAt(2) === ':') {
                    path = path.substring(1)
                }
            } else {
                if (path.startsWith("file://")) {
                    path = path.substring(7)
                }
            }
            pathInput.text = path
            fileExplorer.openFolder(path)
        }
    }

    background: Rectangle {
        color: th.backgroundSidebar
        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: th.borderStone
        }
    }

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
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                spacing: 4

                Rectangle {
                    width: 24; height: 24
                    radius: th.radiusSmall
                    color: backBtn.containsMouse ? th.backgroundHover : "transparent"
                    visible: currentFolderPath.length > 0

                    Label {
                        anchors.centerIn: parent
                        text: "\u2190"
                        font.pixelSize: 14
                        color: th.textSecondary
                    }

                    MouseArea {
                        id: backBtn
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            var parentDir = currentFolderPath.replace(/[\\\/][^\\\/]+$/, "")
                            if (/^[A-Za-z]:$/.test(parentDir)) {
                                parentDir += "\\"
                            }
                            if (parentDir.length > 0 && parentDir !== currentFolderPath) {
                                pathInput.text = parentDir
                                openFolder(parentDir)
                            }
                        }
                    }

                    ToolTip {
                        text: loc("parent_folder")
                        visible: backBtn.containsMouse
                        delay: 600
                    }
                }

                Label {
                    text: loc("projects")
                    color: th.textMuted
                    font.family: th.fontFamilyUI
                    font.pixelSize: 10
                    font.bold: true
                    Layout.fillWidth: true
                    font.capitalization: Font.AllUppercase
                }

                Rectangle {
                    width: 24; height: 24
                    radius: th.radiusSmall
                    color: newFolderBtn.containsMouse ? th.accentLapis : th.borderStone
                    visible: currentFolderPath.length > 0

                    Label {
                        anchors.centerIn: parent
                        text: "\uD83D\uDCC1"
                        font.pixelSize: 11
                        color: newFolderBtn.containsMouse ? "white" : th.textInk
                    }

                    MouseArea {
                        id: newFolderBtn
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: newFolderDialog.open()
                    }

                    ToolTip {
                        text: loc("new_folder")
                        visible: newFolderBtn.containsMouse
                        delay: 600
                    }
                }

                Rectangle {
                    width: 24; height: 24
                    radius: th.radiusSmall
                    color: newFileBtn.containsMouse ? th.accentLapis : th.borderStone
                    visible: currentFolderPath.length > 0

                    Label {
                        anchors.centerIn: parent
                        text: "+"
                        font.pixelSize: 13
                        font.bold: true
                        color: newFileBtn.containsMouse ? "white" : th.textInk
                    }

                    MouseArea {
                        id: newFileBtn
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: newFileDialog.open()
                    }

                    ToolTip {
                        text: loc("new_file")
                        visible: newFileBtn.containsMouse
                        delay: 600
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            color: "transparent"

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: th.borderStone
            }

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 4

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: th.radiusSmall
                    color: th.backgroundCode
                    border.color: pathInput.activeFocus ? th.accentLapis : th.borderStone
                    border.width: 1

                    TextField {
                        id: pathInput
                        anchors.fill: parent
                        anchors.margins: 1
                        placeholderText: loc("path_placeholder")
                        placeholderTextColor: th.textMuted
                        color: th.textInk
                        font.family: th.fontFamilyCode
                        font.pixelSize: 10
                        background: Item {}
                        selectByMouse: true
                        onAccepted: {
                            if (text.length > 0) fileExplorer.openFolder(text)
                        }
                    }
                }

                Rectangle {
                    width: 24; height: 24
                    radius: th.radiusSmall
                    color: browseBtn.containsMouse ? th.accentLapis : th.borderStone

                    Label {
                        anchors.centerIn: parent
                        text: "\uD83D\uDCC2"
                        font.pixelSize: 12
                        color: browseBtn.containsMouse ? "white" : th.textInk
                    }

                    MouseArea {
                        id: browseBtn
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: folderPicker.open()
                    }

                    ToolTip {
                        text: loc("browse")
                        visible: browseBtn.containsMouse
                        delay: 600
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 20
            color: "transparent"
            visible: currentFolderPath.length > 0

            Label {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 12
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                text: currentFolderPath
                color: th.textMuted
                font.family: th.fontFamilyCode
                font.pixelSize: 9
                elide: Label.ElideMiddle
                maximumLineCount: 1
            }
        }

        ListView {
            id: fileListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: ListModel { id: fileListModel }

            delegate: Rectangle {
                width: fileListView.width
                height: 32
                color: fileMouse.containsMouse ? th.backgroundHover :
                       (selectedFilePath === model.path ? th.backgroundSelected : "transparent")

                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 2
                    color: th.accentLapis
                    visible: selectedFilePath === model.path
                }

                MouseArea {
                    id: fileMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (model.isDir) {
                            pathInput.text = model.path
                            currentFolderPath = model.path
                            selectedFilePath = ""
                            fsModel.rootPath = model.path
                            refreshTree()
                        } else {
                            selectedFilePath = model.path
                            fsModel.openFile(model.path)
                        }
                    }
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8
                    spacing: 8

                    Label {
                        text: model.icon
                        font.pixelSize: 13
                    }

                    Label {
                        text: model.name
                        color: selectedFilePath === model.path ? th.textInk : th.textSecondary
                        font.family: model.isDir ? th.fontFamilyUI : th.fontFamilyCode
                        font.pixelSize: 11
                        Layout.fillWidth: true
                        elide: Label.ElideRight
                        font.bold: model.isDir
                    }
                }
            }

            Column {
                anchors.centerIn: parent
                spacing: 6
                visible: fileListModel.count === 0

                Label {
                    text: "\uD83D\uDCC1"
                    font.family: th.fontFamilyUI
                    font.pixelSize: 24
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Label {
                    text: currentFolderPath.length === 0 ?
                          loc("path_hint") :
                          loc("empty_folder")
                    color: th.textMuted
                    font.family: th.fontFamilyUI
                    font.pixelSize: 11
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 24
            color: "transparent"

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 1
                color: th.borderStone
            }

            Label {
                anchors.centerIn: parent
                text: fileListModel.count + " items"
                color: th.textMuted
                font.family: th.fontFamilyCode
                font.pixelSize: 10
            }
        }
    }

    Popup { id: newFileDialog
        
        anchors.centerIn: Overlay.overlay
        
        width: 360
        height: 180
        

        background: Rectangle {
            color: th.backgroundSidebar
            border.width: 1
            border.color: th.borderStone
            radius: th.radiusMedium
        }

        contentItem: ColumnLayout {
            spacing: 16

            Label {
                text: loc("file_name")
                color: th.textInk
                font.family: th.fontFamilyUI
                font.pixelSize: 13
            }

            Rectangle {
                Layout.fillWidth: true
                height: 38
                radius: th.radiusSmall
                color: th.backgroundCode
                border.color: newFileInput.activeFocus ? th.accentLapis : th.borderStone
                border.width: 1

                TextField {
                    id: newFileInput
                    anchors.fill: parent
                    anchors.margins: 2
                    placeholderText: loc("file_placeholder")
                    placeholderTextColor: th.textMuted
                    color: th.textInk
                    font.family: th.fontFamilyCode
                    font.pixelSize: 12
                    background: Item {}
                    selectByMouse: true
                    onAccepted: {
                        if (text.length > 0) {
                            fsModel.createNewFile(currentFolderPath, text)
                            newFileDialog.close()
                            text = ""
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Rectangle {
                    Layout.fillWidth: true
                    height: 34
                    radius: th.radiusSmall
                    color: cancelNewFileArea.containsMouse ? th.backgroundHover : "transparent"
                    border.color: th.borderStone
                    border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: loc("cancel")
                        color: th.textInk
                        font.family: th.fontFamilyUI
                        font.pixelSize: 12
                    }

                    MouseArea {
                        id: cancelNewFileArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            newFileDialog.close()
                            newFileInput.text = ""
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 34
                    radius: th.radiusSmall
                    color: createNewFileArea.containsMouse ? Qt.darker(th.accentLapis, 1.1) : th.accentLapis

                    Label {
                        anchors.centerIn: parent
                        text: loc("create")
                        color: "white"
                        font.family: th.fontFamilyUI
                        font.pixelSize: 12
                        font.bold: true
                    }

                    MouseArea {
                        id: createNewFileArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (newFileInput.text.length > 0) {
                                fsModel.createNewFile(currentFolderPath, newFileInput.text)
                                newFileDialog.close()
                                newFileInput.text = ""
                            }
                        }
                    }
                }
            }
        }
    }

    Popup { id: newFolderDialog
        
        anchors.centerIn: Overlay.overlay
        
        width: 360
        height: 180
        

        background: Rectangle {
            color: th.backgroundSidebar
            border.width: 1
            border.color: th.borderStone
            radius: th.radiusMedium
        }

        contentItem: ColumnLayout {
            spacing: 16

            Label {
                text: loc("folder_name")
                color: th.textInk
                font.family: th.fontFamilyUI
                font.pixelSize: 13
            }

            Rectangle {
                Layout.fillWidth: true
                height: 38
                radius: th.radiusSmall
                color: th.backgroundCode
                border.color: newFolderInput.activeFocus ? th.accentLapis : th.borderStone
                border.width: 1

                TextField {
                    id: newFolderInput
                    anchors.fill: parent
                    anchors.margins: 2
                    placeholderText: loc("folder_placeholder")
                    placeholderTextColor: th.textMuted
                    color: th.textInk
                    font.family: th.fontFamilyCode
                    font.pixelSize: 12
                    background: Item {}
                    selectByMouse: true
                    onAccepted: {
                        if (text.length > 0) {
                            fsModel.createNewFolder(currentFolderPath, text)
                            newFolderDialog.close()
                            text = ""
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Rectangle {
                    Layout.fillWidth: true
                    height: 34
                    radius: th.radiusSmall
                    color: cancelNewFolderArea.containsMouse ? th.backgroundHover : "transparent"
                    border.color: th.borderStone
                    border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: loc("cancel")
                        color: th.textInk
                        font.family: th.fontFamilyUI
                        font.pixelSize: 12
                    }

                    MouseArea {
                        id: cancelNewFolderArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            newFolderDialog.close()
                            newFolderInput.text = ""
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 34
                    radius: th.radiusSmall
                    color: createNewFolderArea.containsMouse ? Qt.darker(th.accentLapis, 1.1) : th.accentLapis

                    Label {
                        anchors.centerIn: parent
                        text: loc("create")
                        color: "white"
                        font.family: th.fontFamilyUI
                        font.pixelSize: 12
                        font.bold: true
                    }

                    MouseArea {
                        id: createNewFolderArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (newFolderInput.text.length > 0) {
                                fsModel.createNewFolder(currentFolderPath, newFolderInput.text)
                                newFolderDialog.close()
                                newFolderInput.text = ""
                            }
                        }
                    }
                }
            }
        }
    }
}

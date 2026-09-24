import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.0

Popup {
    id: popup

    property var suggestions: []
    property int selectedIndex: 0
    property string currentWord: ""

    signal accepted(string word)
    signal cancelled()

    x: 0
    y: 0
    width: 300
    height: Math.min(suggestionList.count * 34 + 12, 250)
    padding: 0
    closePolicy: Popup.NoAutoClose
    visible: false

    function showSuggestions(words, word, cursorX, cursorY) {
        if (words.length === 0) { close(); return }
        suggestions = words
        currentWord = word
        selectedIndex = 0
        var maxX = editorPane.width - width - 8
        var minX = 8
        x = Math.max(minX, Math.min(cursorX, maxX))
        y = cursorY + 4
        if (y + height > editorPane.height - 8)
            y = cursorY - height - 4
        if (y < 0) y = 8
        open()
        suggestionList.positionViewAtBeginning()
    }

    function closePopup() {
        close()
        cancelled()
    }

    background: Rectangle {
        color: th.backgroundSidebar
        border.color: th.borderStone
        border.width: 1
        radius: 4

        Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 1
            color: th.accentLapis
            opacity: 0.4
        }
    }

    contentItem: ListView {
        id: suggestionList
        anchors.fill: parent
        anchors.margins: 6
        clip: true
        spacing: 2
        model: popup.suggestions
        currentIndex: popup.selectedIndex
        keyNavigationEnabled: false
        highlightMoveDuration: 80
        highlightMoveVelocity: 400

        highlight: Rectangle {
            radius: 3
            color: th.syntaxKeyword
            Behavior on y { NumberAnimation { duration: 80 } }
        }

        delegate: Rectangle {
            width: suggestionList.width
            height: 32
            radius: 3
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                spacing: 8

                Rectangle {
                    width: 20
                    height: 20
                    radius: 4
                    color: modelData.type === "keyword" ? th.withAlpha(th.syntaxKeyword, 0.15) :
                           modelData.type === "function" ? th.withAlpha(th.syntaxFunction, 0.15) :
                           modelData.type === "type" ? th.withAlpha(th.syntaxType, 0.15) :
                           modelData.type === "modifier" ? th.withAlpha(th.accentGold, 0.15) :
                           modelData.type === "ui" ? th.withAlpha(th.semanticError, 0.15) :
                           th.withAlpha(th.syntaxFunction, 0.15)

                    Label {
                        anchors.centerIn: parent
                        text: modelData.type === "keyword" ? "K" :
                              modelData.type === "function" ? "^" :
                              modelData.type === "type" ? "O" :
                              modelData.type === "modifier" ? "^" :
                              modelData.type === "ui" ? "U" :
                              "P"
                        font.pixelSize: 9
                        color: modelData.type === "keyword" ? th.syntaxKeyword :
                               modelData.type === "function" ? th.syntaxFunction :
                               modelData.type === "type" ? th.syntaxType :
                               modelData.type === "modifier" ? th.accentGold :
                               modelData.type === "ui" ? th.semanticError :
                               th.syntaxFunction
                    }
                }

                Label {
                    text: modelData.name
                    color: th.textInk
                    font.family: th.fontFamilyCode
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    elide: Label.ElideRight
                    horizontalAlignment: Text.AlignRight
                }

                Label {
                    text: modelData.type === "keyword" ? translator.translate("type_keyword") :
                          modelData.type === "function" ? translator.translate("type_function") :
                          modelData.type === "type" ? translator.translate("type_type") :
                          modelData.type === "modifier" ? translator.translate("type_modifier") :
                          modelData.type === "value" ? translator.translate("type_value") :
                          modelData.type === "ui" ? translator.translate("type_widget") : ""
                    color: th.textMuted
                    font.family: th.fontFamilyUI
                    font.pixelSize: 9
                }
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onContainsMouseChanged: {
                    if (containsMouse) popup.selectedIndex = index
                }
                onClicked: {
                    popup.selectedIndex = index
                    popup.accepted(popup.suggestions[index].name)
                    popup.close()
                    popup.cancelled()
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            width: 5
            policy: suggestionList.contentHeight > suggestionList.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
            contentItem: Rectangle {
                width: 5
                radius: 2.5
                color: parent.parent && parent.parent.ScrollBar.vertical.hovered ? th.textMuted : th.borderStone
                Behavior on color { ColorAnimation { duration: 100 } }
            }
            background: Rectangle { color: "transparent" }
        }
    }

    Connections {
        target: suggestionList
        function onCurrentIndexChanged() {
            if (popup.selectedIndex !== suggestionList.currentIndex)
                popup.selectedIndex = suggestionList.currentIndex
        }
    }
}

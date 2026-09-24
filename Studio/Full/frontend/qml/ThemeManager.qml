import QtQuick 2.15

Item {
    id: themeManager
    visible: false

    property bool isDark: false

    property color backgroundPaper:     isDark ? dark.backgroundPaper     : light.backgroundPaper
    property color backgroundSidebar:   isDark ? dark.backgroundSidebar   : light.backgroundSidebar
    property color backgroundToolbar:   isDark ? dark.backgroundToolbar   : light.backgroundToolbar
    property color backgroundHover:     isDark ? dark.backgroundHover     : light.backgroundHover
    property color backgroundPressed:   isDark ? dark.backgroundPressed   : light.backgroundPressed
    property color backgroundSelected:  isDark ? dark.backgroundSelected  : light.backgroundSelected
    property color backgroundCode:      isDark ? dark.backgroundCode      : light.backgroundCode
    property color backgroundInput:    isDark ? dark.backgroundInput    : light.backgroundInput

    property color accent:             isDark ? dark.accent             : light.accent

    property color borderStone:         isDark ? dark.borderStone         : light.borderStone
    property color borderLight:         isDark ? dark.borderLight         : light.borderLight
    property color borderFrameOuter:    isDark ? dark.borderFrameOuter    : light.borderFrameOuter
    property color borderFrameInner:    isDark ? dark.borderFrameInner    : light.borderFrameInner

    property color textInk:             isDark ? dark.textInk             : light.textInk
    property color textSecondary:       isDark ? dark.textSecondary       : light.textSecondary
    property color textMuted:           isDark ? dark.textMuted           : light.textMuted
    property color textInverse:         isDark ? dark.textInverse         : light.textInverse

    property color accentLapis:         isDark ? dark.accentLapis         : light.accentLapis
    property color accentLapisLight:    isDark ? dark.accentLapisLight    : light.accentLapisLight
    property color accentLapisBg:       isDark ? dark.accentLapisBg       : light.accentLapisBg

    property color accentGold:          isDark ? dark.accentGold          : light.accentGold
    property color accentGoldLight:     isDark ? dark.accentGoldLight     : light.accentGoldLight
    property color accentGoldMuted:     isDark ? dark.accentGoldMuted     : light.accentGoldMuted

    property color semanticSuccess:     isDark ? dark.semanticSuccess     : light.semanticSuccess
    property color semanticWarning:     isDark ? dark.semanticWarning     : light.semanticWarning
    property color semanticError:       isDark ? dark.semanticError       : light.semanticError
    property color semanticInfo:        isDark ? dark.semanticInfo        : light.semanticInfo

    property color syntaxKeyword:       isDark ? dark.syntaxKeyword       : light.syntaxKeyword
    property color syntaxType:          isDark ? dark.syntaxType          : light.syntaxType
    property color syntaxString:        isDark ? dark.syntaxString        : light.syntaxString
    property color syntaxComment:       isDark ? dark.syntaxComment       : light.syntaxComment
    property color syntaxNumber:        isDark ? dark.syntaxNumber        : light.syntaxNumber
    property color syntaxFunction:      isDark ? dark.syntaxFunction      : light.syntaxFunction

    property color menuBarBg:           isDark ? dark.menuBarBg           : light.menuBarBg
    property color statusBarBg:         isDark ? dark.statusBarBg         : light.statusBarBg
    property color tabActiveBg:         isDark ? dark.tabActiveBg         : light.tabActiveBg
    property color tabInactiveBg:       isDark ? dark.tabInactiveBg       : light.tabInactiveBg

    property color terminalBg:          isDark ? dark.terminalBg          : light.terminalBg
    property color terminalText:        isDark ? dark.terminalText        : light.terminalText
    property color terminalAccent:      isDark ? dark.terminalAccent      : light.terminalAccent

    property color panelBg:             isDark ? dark.panelBg             : light.panelBg

    property string fontFamilyUI:     "IBM Plex Sans Arabic, Cairo, Segoe UI, Inter, sans-serif"
    property string fontFamilyCode:   "IBM Plex Mono, JetBrains Mono, Consolas, monospace"
    property int    fontSizeSmall:    11
    property int    fontSizeNormal:   13
    property int    fontSizeLarge:    16
    property int    fontSizeTitle:    22

    property int radiusSmall:   10
    property int radiusMedium:  12
    property int radiusLarge:   14

    property int spacingTiny:    2
    property int spacingSmall:   6
    property int spacingMedium:  10
    property int spacingLarge:   16
    property int spacingXLarge:  24

    PaletteLight { id: light }
    PaletteDark  { id: dark }

    function toggleTheme() { isDark = !isDark }
    function withAlpha(color, alpha) { return Qt.rgba(color.r, color.g, color.b, alpha) }
}

#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <cstddef>

namespace daad {

enum class KeywordType {
    // 1. Data Types & Fundamentals
    KwInt, KwDouble, KwBool, KwChar, KwString, KwConst, KwVoid, KwAuto,
    KwTrue, KwFalse, KwNullptr, KwTypedef,

    // 2. Control Flow
    KwIf, KwElse, KwWhile, KwDo, KwFor, KwIn, KwSwitch, KwCase, KwDefault,
    KwBreak, KwContinue, KwGoto, KwReturn,

    // 3. OOP & Scoping
    KwClass, KwStruct, KwEnum, KwInterface, KwNamespace,
    KwPublic, KwPrivate, KwProtected, KwInherit, KwSelf, KwBase, KwAbstract,

    // 4. Functions & Memory
    KwFunction, KwNew, KwDelete, KwPointer, KwReference,
    KwStatic, KwInline, KwExtern, KwTemplate,

    // 5. Exception Handling
    KwTry, KwCatch, KwFinally, KwThrow, KwAssert, KwException,
    KwTypeOf, KwSizeOf, KwIncrement, KwDecrement,

    // 6. Advanced Systems
    KwSync, KwAwait, KwThread, KwLock, KwShared, KwUnique,
    KwImport, KwExport, KwModule, KwAlternative,

    // 7. GUI Keywords
    KwButton, KwTextField, KwComboBox, KwImage, KwCheckBox, KwSlider,
    KwDropDown, KwPanel, KwLabel, KwColumn, KwRow, KwGrid,
    KwProgressBar, KwTabBar,

    // 8. Print / Input
    KwPrint, KwInput,

    // 8c. Arabic logic
    KwAndArabic, KwOrArabic,

    // 9. Image Processing
    KwLoadImage, KwDrawImage, KwImageSize, KwSaveImage, KwCropImage,
    KwResize, KwRotateImage, KwFlipImage, KwOpacity, KwFilter,
    KwOverlay, KwBackground, KwPixel, KwDraw, KwFill,
    KwRectangle, KwCircle, KwLine, KwTextOnCanvas, KwClear,

    // Coordinate axes
    KwX, KwY
};

struct KeywordInfo {
    std::string arabicText;
    std::string cppEquivalent;
};

// Transparent hash to allow find() with string_view on string-keyed maps
struct StringHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept { return std::hash<std::string_view>{}(sv); }
    size_t operator()(const std::string& s) const noexcept { return std::hash<std::string>{}(s); }
};

class KeywordRegistry {
public:
    void initializeStandardKeywords();
    std::optional<KeywordType> findKeyword(std::string_view text) const noexcept;
    std::string_view getKeywordText(KeywordType type) const noexcept;
    std::string_view getCppEquivalent(KeywordType type) const noexcept;

    static bool isDataType(KeywordType type) noexcept;
    static bool isControlFlow(KeywordType type) noexcept;
    static bool isAccessSpecifier(KeywordType type) noexcept;

private:
    std::unordered_map<std::string, KeywordType, StringHash, std::equal_to<>> m_keywordToType;
    std::unordered_map<KeywordType, KeywordInfo> m_typeToInfo;
};

const KeywordRegistry& getStandardKeywordRegistry() noexcept;

} // namespace daad

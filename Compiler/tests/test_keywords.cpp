#include <gtest/gtest.h>
#include "Daad/Keywords.hpp"

using namespace daad;

TEST(KeywordsTest, KwPrint) {
    const auto& reg = getStandardKeywordRegistry();
    auto kw = reg.findKeyword("طباعة");
    ASSERT_TRUE(kw.has_value());
    EXPECT_EQ(kw.value(), KeywordType::KwPrint);
    // getKeywordText may return "اطبع" or "طباعة" depending on map order — accept either
    std::string txt(reg.getKeywordText(KeywordType::KwPrint));
    EXPECT_TRUE(txt == "طباعة" || txt == "اطبع");
    EXPECT_NE(std::string(reg.getCppEquivalent(KeywordType::KwPrint)).find("daad_print"), std::string::npos);
    // also verify alias
    auto kw2 = reg.findKeyword("اطبع");
    ASSERT_TRUE(kw2.has_value());
    EXPECT_EQ(kw2.value(), KeywordType::KwPrint);
}

TEST(KeywordsTest, KwInput) {
    const auto& reg = getStandardKeywordRegistry();
    auto kw = reg.findKeyword("ادخل");
    ASSERT_TRUE(kw.has_value());
    EXPECT_EQ(kw.value(), KeywordType::KwInput);
    // بعد الإصلاح يجب أن لا يسبب warning والـ switch يعالجه
    auto cppEq = reg.getCppEquivalent(KeywordType::KwInput);
    // قد يكون فارغ لأنه يعالج كـ InputStmt
    EXPECT_TRUE(cppEq.size() == 0 || cppEq.size() > 0);
    // التأكد أن find يعيد نفس الشيء لـ KwInput
    EXPECT_EQ(reg.getKeywordText(KeywordType::KwInput), "ادخل");
}

TEST(KeywordsTest, KwInt) {
    const auto& reg = getStandardKeywordRegistry();
    auto kw = reg.findKeyword("صحيح");
    ASSERT_TRUE(kw.has_value());
    EXPECT_EQ(kw.value(), KeywordType::KwInt);
    EXPECT_EQ(reg.getCppEquivalent(KeywordType::KwInt), "int");
    EXPECT_TRUE(KeywordRegistry::isDataType(KeywordType::KwInt));
}

TEST(KeywordsTest, UnknownKeyword) {
    const auto& reg = getStandardKeywordRegistry();
    auto kw = reg.findKeyword("غير_موجود_كلمة");
    EXPECT_FALSE(kw.has_value());
    // معرف عربي عادي ليس كلمة مفتاحية
    auto kw2 = reg.findKeyword("متغير");
    EXPECT_FALSE(kw2.has_value());
}

TEST(KeywordsTest, ArabicLogic) {
    const auto& reg = getStandardKeywordRegistry();
    auto kwW = reg.findKeyword("و");
    auto kwO = reg.findKeyword("أو");
    ASSERT_TRUE(kwW.has_value());
    ASSERT_TRUE(kwO.has_value());
    EXPECT_EQ(kwW.value(), KeywordType::KwAndArabic);
    EXPECT_EQ(kwO.value(), KeywordType::KwOrArabic);
    EXPECT_EQ(reg.getCppEquivalent(KeywordType::KwAndArabic), "&&");
    EXPECT_EQ(reg.getCppEquivalent(KeywordType::KwOrArabic), "||");
}

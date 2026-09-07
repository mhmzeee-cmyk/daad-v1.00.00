/**
 * @file DaadRuntime.hpp
 * @brief طبقة بيئة التشغيل العابرة للمنصات (Cross-platform Runtime)
 */
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>

#ifdef __ANDROID__
#include <android/log.h>
#define DAAD_LOG_TAG "DAAD_RUNTIME"
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace daad::runtime {

inline std::string daad_to_string(const std::string& v) { return v; }
inline std::string daad_to_string(const char* v) { return v; }
inline std::string daad_to_string(char v) { return std::string(1, v); }
inline std::string daad_to_string(bool v) { return v ? "صواب" : "خطأ"; }

template <typename T>
inline auto daad_to_string(T v) -> std::enable_if_t<std::is_arithmetic_v<T>, std::string> {
    std::ostringstream oss;
    if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
        oss << v;
    } else {
        oss << v;
    }
    return oss.str();
}

inline void daad_print_impl(const std::string& text) {
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_INFO, DAAD_LOG_TAG, "%s", text.c_str());
#elif defined(__APPLE__)
    printf("%s\n", text.c_str());
#else
    std::cout << text << std::endl;
#endif
}

template <typename... Args>
inline void daad_print(Args&&... args) {
    std::string result;
    ((result += daad_to_string(std::forward<Args>(args))), ...);
    daad_print_impl(result);
}

} // namespace daad::runtime
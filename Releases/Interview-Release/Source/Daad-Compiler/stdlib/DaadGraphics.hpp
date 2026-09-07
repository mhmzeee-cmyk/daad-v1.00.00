/**
 * @file DaadGraphics.hpp
 * @brief طبقة الرسوميات — ANSI Terminal Canvas
 *        مطابق لواجهة __dhad في الويب: مستطيل، دائرة، خط، بكسل، نص
 *        صفر اعتماديات — ANSI escape codes فقط
 */
#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstdlib>

namespace daad::graphics {

// ═══════════════════════════════════════════════════════════════════════════════
// الألوان
// ═══════════════════════════════════════════════════════════════════════════════

struct Color {
    uint8_t r = 0, g = 0, b = 0;
    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    bool operator==(const Color& o) const { return r==o.r && g==o.g && b==o.b; }
    bool operator!=(const Color& o) const { return !(*this == o); }
};

namespace colors {
    inline Color aswad{0,0,0};        // أسود
    inline Color abyad{255,255,255};   // أبيض
    inline Color ahmar{220,50,50};     // أحمر
    inline Color akhdar{50,200,50};    // أخضر
    inline Color azraq{50,100,220};    // أزرق
    inline Color asfar{230,210,50};    // أصفر
    inline Color burtuqali{230,130,30}; // برتقالي
    inline Color banafsaji{160,50,200}; // بنفسجي
    inline Color samawi{50,200,200};   // سماوي
    inline Color wardi{230,100,150};   // وردي
    inline Color ramadi{128,128,128};  // رمادي
    inline Color ramadi_fatih{192,192,192}; // رمادي فاتح
    inline Color ramadi_dakin{64,64,64};    // رمادي داكن
    inline Color boni{139,69,19};      // بني
    inline Color nayli{70,130,180};    // نيلي
    inline Color zumurudi{80,200,120}; // زمردي
}

inline Color from_name(const std::string& name) {
    if (name == "\xd8\xa3\xd8\xb3\xd9\x88\xd8\xaf" || name == "\xd8\xa7\xd8\xb3\xd9\x88\xd8\xaf") return colors::aswad;
    if (name == "\xd8\xa3\xd8\xa8\xd9\x8a\xd8\xb6" || name == "\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xb6") return colors::abyad;
    if (name == "\xd8\xa3\xd8\xad\xd9\x85\xd8\xb1" || name == "\xd8\xa7\xd8\xad\xd9\x85\xd8\xb1") return colors::ahmar;
    if (name == "\xd8\xa3\xd8\xae\xd8\xb6\xd8\xb1" || name == "\xd8\xa7\xd8\xae\xd8\xb6\xd8\xb1") return colors::akhdar;
    if (name == "\xd8\xa3\xd8\xb2\xd8\xb1\xd9\x82" || name == "\xd8\xa7\xd8\xb2\xd8\xb1\xd9\x82") return colors::azraq;
    if (name == "\xd8\xa3\xd8\xb5\xd9\x81\xd8\xb1" || name == "\xd8\xa7\xd8\xb5\xd9\x81\xd8\xb1") return colors::asfar;
    if (name == "\xd8\xa8\xd8\xb1\xd8\xaa\xd9\x82\xd8\xa7\xd9\x84\xd9\x8a") return colors::burtuqali;
    if (name == "\xd8\xa8\xd9\x86\xd9\x81\xd8\xb3\xd8\xac\xd9\x8a") return colors::banafsaji;
    if (name == "\xd8\xb3\xd9\x85\xd8\xa7\xd9\x88\xd9\x8a") return colors::samawi;
    if (name == "\xd9\x88\xd8\xb1\xd8\xaf\xd9\x8a") return colors::wardi;
    if (name == "\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a") return colors::ramadi;
    if (name == "\xd8\xa8\xd9\x86\xd9\x8a") return colors::boni;
    if (name == "\xd9\x86\xd9\x8a\xd9\x84\xd9\x8a") return colors::nayli;
    if (name == "\xd8\xb2\xd9\x85\xd8\xb1\xd8\xaf\xd9\x8a") return colors::zumurudi;
    // hex fallback: #RRGGBB
    if (name.size() == 7 && name[0] == '#') {
        unsigned int hex = std::stoul(name.substr(1), nullptr, 16);
        return Color((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }
    return colors::abyad;
}

// ═══════════════════════════════════════════════════════════════════════════════
// اللوحة (Canvas)
// ═══════════════════════════════════════════════════════════════════════════════

class Canvas {
public:
    int w = 0, h = 0;
    std::vector<Color> pixels;
    Color fg = colors::abyad;
    Color bg = colors::aswad;

    Canvas() = default;

    void create(int width, int height) {
        w = width;
        h = height;
        pixels.resize(w * h, bg);
    }

    Color& at(int x, int y) {
        static Color dummy;
        if (x < 0 || x >= w || y < 0 || y >= h) return dummy;
        return pixels[y * w + x];
    }

    const Color& at(int x, int y) const {
        static Color dummy;
        if (x < 0 || x >= w || y < 0 || y >= h) return dummy;
        return pixels[y * w + x];
    }

    void set(int x, int y, const Color& c) {
        if (x >= 0 && x < w && y >= 0 && y < h)
            pixels[y * w + x] = c;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// المحرك العام (Singleton)
// ═══════════════════════════════════════════════════════════════════════════════

static Canvas& engine() {
    static Canvas c;
    return c;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ANSI Escape Codes
// ═══════════════════════════════════════════════════════════════════════════════

namespace ansi {
    inline void move_to(int row, int col) {
        printf("\033[%d;%dH", row, col);
    }
    inline void clear_screen() { printf("\033[2J"); }
    inline void hide_cursor() { printf("\033[?25l"); }
    inline void show_cursor() { printf("\033[?25h"); }
    inline void bg_rgb(uint8_t r, uint8_t g, uint8_t b) {
        printf("\033[48;2;%u;%u;%um", r, g, b);
    }
    inline void fg_rgb(uint8_t r, uint8_t g, uint8_t b) {
        printf("\033[38;2;%u;%u;%um", r, g, b);
    }
    inline void reset() { printf("\033[0m"); }
}

// ═══════════════════════════════════════════════════════════════════════════════
// واجهة الرسم — مطابقة لـ __dhad في الويب
// ═══════════════════════════════════════════════════════════════════════════════

inline void setCanvasSize(int width, int height) {
    engine().create(width, height);
}

inline void setColor(int r, int g, int b) {
    engine().fg = Color(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
}

inline void setColor(const std::string& name) {
    engine().fg = from_name(name);
}

inline void setBgColor(int r, int g, int b) {
    engine().bg = Color(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
}

inline void clear() {
    auto& e = engine();
    std::fill(e.pixels.begin(), e.pixels.end(), e.bg);
}

inline void rectangle(int x, int y, int w, int h) {
    auto& e = engine();
    const Color& c = e.fg;
    for (int j = y; j < y + h; j++)
        for (int i = x; i < x + w; i++)
            e.set(i, j, c);
}

inline void strokeRect(int x, int y, int w, int h) {
    auto& e = engine();
    const Color& c = e.fg;
    for (int i = x; i < x + w; i++) {
        e.set(i, y, c);
        e.set(i, y + h - 1, c);
    }
    for (int j = y; j < y + h; j++) {
        e.set(x, j, c);
        e.set(x + w - 1, j, c);
    }
}

inline void circle(int cx, int cy, int r) {
    auto& e = engine();
    const Color& c = e.fg;
    for (int y = -r; y <= r; y++)
        for (int x = -r; x <= r; x++)
            if (x*x + y*y <= r*r)
                e.set(cx + x, cy + y, c);
}

inline void strokeCircle(int cx, int cy, int r) {
    auto& e = engine();
    const Color& c = e.fg;
    for (int angle = 0; angle < 360; angle++) {
        double rad = angle * M_PI / 180.0;
        int px = cx + static_cast<int>(r * cos(rad));
        int py = cy + static_cast<int>(r * sin(rad));
        e.set(px, py, c);
    }
}

inline void line(int x1, int y1, int x2, int y2) {
    auto& e = engine();
    const Color& c = e.fg;
    int dx = std::abs(x2 - x1), dy = std::abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    while (true) {
        e.set(x1, y1, c);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

inline void pixel(int x, int y) {
    engine().set(x, y, engine().fg);
}

inline void pixel(int x, int y, int r, int g, int b) {
    engine().set(x, y, Color(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)));
}

inline void fill(int x, int y, int w, int h, int r, int g, int b) {
    Color old = engine().fg;
    engine().fg = Color(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
    rectangle(x, y, w, h);
    engine().fg = old;
}

inline void background(int r, int g, int b) {
    engine().bg = Color(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
    clear();
}

// ═══════════════════════════════════════════════════════════════════════════════
// العرض في التيرمينال
// ═══════════════════════════════════════════════════════════════════════════════

inline void render() {
    auto& e = engine();
    if (e.w == 0 || e.h == 0) return;

    ansi::hide_cursor();
    ansi::clear_screen();

    for (int y = 0; y < e.h; y++) {
        ansi::move_to(y + 1, 1);
        for (int x = 0; x < e.w; x++) {
            const Color& c = e.at(x, y);
            ansi::bg_rgb(c.r, c.g, c.b);
            putchar(' ');
        }
    }
    ansi::reset();
    ansi::show_cursor();
    fflush(stdout);
}

// ═══════════════════════════════════════════════════════════════════════════════
// النصوص
// ═══════════════════════════════════════════════════════════════════════════════

static const uint8_t FONT_5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // space
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x07,0x00,0x07,0x00}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x55,0x22,0x50}, // &
    {0x00,0x05,0x03,0x00,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x14,0x08,0x3E,0x08,0x14}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x50,0x30,0x00,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x08,0x14,0x22,0x41,0x00}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x00,0x41,0x22,0x14,0x08}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?
    {0x32,0x49,0x79,0x41,0x3E}, // @
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x3F,0x40,0x38,0x40,0x3F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z
};

inline void text(const std::string& t, int x, int y, int scale = 1) {
    auto& e = engine();
    const Color& c = e.fg;
    int cx = x;
    for (char ch : t) {
        if (ch == '\n') { cx = x; y += 8 * scale; continue; }
        if (ch < 32 || ch > 90) { cx += 6 * scale; continue; }
        int idx = ch - 32;
        for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 5; col++) {
                bool on = (FONT_5x7[idx][col] >> row) & 1;
                if (on) {
                    for (int dy = 0; dy < scale; dy++)
                        for (int dx = 0; dx < scale; dx++)
                            e.set(cx + col*scale + dx, y + row*scale + dy, c);
                }
            }
        }
        cx += 6 * scale;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// حفظ / تحميل
// ═══════════════════════════════════════════════════════════════════════════════

inline void saveImage(const std::string& path) {
    auto& e = engine();
    FILE* f = fopen(path.c_str(), "w");
    if (!f) return;
    fprintf(f, "%d %d\n", e.w, e.h);
    for (int y = 0; y < e.h; y++) {
        for (int x = 0; x < e.w; x++) {
            const Color& c = e.at(x, y);
            fprintf(f, "%u,%u,%u ", c.r, c.g, c.b);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

inline void loadImage(const std::string& path) {
    auto& e = engine();
    FILE* f = fopen(path.c_str(), "r");
    if (!f) return;
    int w, h;
    if (fscanf(f, "%d %d", &w, &h) != 2) { fclose(f); return; }
    e.create(w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int r, g, b;
            if (fscanf(f, "%d,%d,%d ", &r, &g, &b) == 3)
                e.set(x, y, Color(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)));
        }
    }
    fclose(f);
}

// ═══════════════════════════════════════════════════════════════════════════════
// معلومات اللوحة
// ═══════════════════════════════════════════════════════════════════════════════

inline int canvasWidth() { return engine().w; }
inline int canvasHeight() { return engine().h; }

} // namespace daad::graphics

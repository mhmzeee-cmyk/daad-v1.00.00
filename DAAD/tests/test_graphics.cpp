/**
 * @file test_graphics.cpp
 * @brief اختبارات وحدة — DaadGraphics.hpp (ANSI Canvas)
 */

#include "DaadGraphics.hpp"
#include <cstdio>
#include <cstring>
#include <cassert>

static int tests = 0;
static int passed = 0;

#define TEST(name) do { tests++; printf("  %-50s", name); } while(0)
#define OK() do { printf("[PASS]\n"); passed++; } while(0)
#define FAIL(msg) do { printf("[FAIL] %s\n", msg); } while(0)

using namespace daad::graphics;

static void test_canvas_create() {
    TEST("setCanvasSize creates canvas");
    setCanvasSize(80, 24);
    if (canvasWidth() == 80 && canvasHeight() == 24) OK(); else FAIL("wrong size");
}

static void test_color_set() {
    TEST("setColor sets foreground");
    setCanvasSize(10, 10);
    setColor(255, 0, 0);
    pixel(0, 0);
    const Color& c = engine().at(0, 0);
    if (c.r == 255 && c.g == 0 && c.b == 0) OK(); else FAIL("wrong color");
}

static void test_rectangle_fill() {
    TEST("rectangle fills area");
    setCanvasSize(20, 20);
    clear();
    setColor(0, 255, 0);
    rectangle(5, 5, 4, 3);
    bool ok = true;
    for (int y = 5; y < 8; y++)
        for (int x = 5; x < 9; x++)
            if (engine().at(x, y).g != 255) ok = false;
    // outside should be black
    if (engine().at(4, 5).r == 0 && engine().at(4, 5).g == 0) {}
    else ok = false;
    if (ok) OK(); else FAIL("rectangle");
}

static void test_stroke_rect() {
    TEST("strokeRect draws outline only");
    setCanvasSize(20, 20);
    clear();
    setColor(0, 0, 255);
    strokeRect(2, 2, 6, 4);
    bool ok = true;
    // top edge
    for (int x = 2; x < 8; x++)
        if (engine().at(x, 2).b != 255) ok = false;
    // bottom edge
    for (int x = 2; x < 8; x++)
        if (engine().at(x, 5).b != 255) ok = false;
    // interior should be black
    if (engine().at(4, 3).b != 0) ok = false;
    if (engine().at(4, 4).b != 0) ok = false;
    if (ok) OK(); else FAIL("strokeRect");
}

static void test_circle() {
    TEST("circle fills circle area");
    setCanvasSize(30, 30);
    clear();
    setColor(255, 255, 0);
    circle(15, 15, 5);
    bool ok = true;
    // center should be yellow
    if (engine().at(15, 15).r != 255 || engine().at(15, 15).g != 255) ok = false;
    // point far outside should be black
    if (engine().at(0, 0).r != 0) ok = false;
    if (ok) OK(); else FAIL("circle");
}

static void test_line() {
    TEST("line draws Bresenham line");
    setCanvasSize(20, 20);
    clear();
    setColor(255, 128, 0);
    line(0, 0, 9, 9);
    bool ok = true;
    // diagonal pixels should be set
    for (int i = 0; i <= 9; i++)
        if (engine().at(i, i).r != 255) ok = false;
    // off-diagonal should be black
    if (engine().at(5, 3).r != 0) ok = false;
    if (ok) OK(); else FAIL("line");
}

static void test_clear() {
    TEST("clear resets to background");
    setCanvasSize(10, 10);
    setColor(255, 255, 255);
    rectangle(0, 0, 10, 10);
    clear();
    bool ok = true;
    for (int y = 0; y < 10; y++)
        for (int x = 0; x < 10; x++)
            if (engine().at(x, y).r != 0 || engine().at(x, y).g != 0) ok = false;
    if (ok) OK(); else FAIL("clear");
}

static void test_background() {
    TEST("background fills entire canvas");
    setCanvasSize(10, 10);
    background(100, 100, 100);
    bool ok = true;
    for (int y = 0; y < 10; y++)
        for (int x = 0; x < 10; x++)
            if (engine().at(x, y).r != 100) ok = false;
    if (ok) OK(); else FAIL("background");
}

static void test_fill_area() {
    TEST("fill colors rectangle with given RGB");
    setBgColor(0, 0, 0);
    setCanvasSize(10, 10);
    clear();
    fill(2, 2, 3, 3, 50, 100, 150);
    bool ok = true;
    for (int y = 2; y < 5; y++)
        for (int x = 2; x < 5; x++) {
            const Color& c = engine().at(x, y);
            if (c.r != 50 || c.g != 100 || c.b != 150) ok = false;
        }
    if (engine().at(0, 0).r != 0 || engine().at(0, 0).g != 0 || engine().at(0, 0).b != 0) ok = false;
    if (ok) OK(); else {
        printf("\n    got(%d,%d,%d) expected(50,100,150)\n",
            engine().at(2, 2).r, engine().at(2, 2).g, engine().at(2, 2).b);
        FAIL("fill");
    }
}

static void test_pixel_coords() {
    TEST("pixel at arbitrary coordinates");
    setCanvasSize(50, 50);
    clear();
    setColor(200, 100, 50);
    pixel(49, 49);
    const Color& c = engine().at(49, 49);
    if (c.r == 200 && c.g == 100 && c.b == 50) OK(); else FAIL("pixel coords");
}

static void test_out_of_bounds() {
    TEST("out of bounds pixel is ignored");
    setBgColor(0, 0, 0);
    setCanvasSize(10, 10);
    clear();
    setColor(255, 255, 255);
    pixel(-1, -1);
    pixel(100, 100);
    const Color& c = engine().at(0, 0);
    if (c.r == 0 && c.g == 0 && c.b == 0) OK(); else FAIL("OOB");
}

static void test_stroke_circle() {
    TEST("strokeCircle draws outline");
    setBgColor(0, 0, 0);
    setCanvasSize(30, 30);
    clear();
    setColor(0, 200, 200);
    strokeCircle(15, 15, 5);
    bool ok = true;
    if (engine().at(15, 15).g != 0) ok = false;
    bool has_edge = false;
    for (int angle = 0; angle < 360; angle++) {
        double rad = angle * M_PI / 180.0;
        int px = 15 + static_cast<int>(5 * cos(rad));
        int py = 15 + static_cast<int>(5 * sin(rad));
        if (engine().at(px, py).g == 200) has_edge = true;
    }
    if (!has_edge) ok = false;
    if (ok) OK(); else FAIL("strokeCircle");
}

static void test_save_load() {
    TEST("save/load roundtrip");
    setCanvasSize(5, 5);
    clear();
    setColor(100, 150, 200);
    rectangle(1, 1, 3, 3);
    saveImage("/tmp/daad_gfx_test.ppm");

    Canvas backup = engine();
    setCanvasSize(5, 5);
    clear();
    loadImage("/tmp/daad_gfx_test.ppm");

    bool ok = true;
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++) {
            const Color& a = backup.at(x, y);
            const Color& b = engine().at(x, y);
            if (a.r != b.r || a.g != b.g || a.b != b.b) ok = false;
        }
    // restore
    engine() = backup;
    if (ok) OK(); else FAIL("save/load");
}

static void test_multiple_colors() {
    TEST("multiple colors coexist");
    setCanvasSize(10, 10);
    clear();
    setColor(255, 0, 0);   pixel(0, 0);
    setColor(0, 255, 0);   pixel(1, 0);
    setColor(0, 0, 255);   pixel(2, 0);
    setColor(255, 255, 0); pixel(3, 0);
    bool ok = true;
    if (engine().at(0, 0).r != 255) ok = false;
    if (engine().at(1, 0).g != 255) ok = false;
    if (engine().at(2, 0).b != 255) ok = false;
    if (engine().at(3, 0).r != 255 || engine().at(3, 0).g != 255) ok = false;
    if (ok) OK(); else FAIL("multi-color");
}

static void test_color_name() {
    TEST("from_name converts Arabic color names");
    Color c1 = from_name("\xd8\xa3\xd8\xad\xd9\x85\xd8\xb1"); // أحمر
    Color c2 = from_name("\xd8\xa3\xd8\xae\xd8\xb6\xd8\xb1"); // أخضر
    Color c3 = from_name("#FF8800");
    bool ok = true;
    if (c1.r < 200) ok = false;
    if (c2.g < 100) ok = false;
    if (c3.r != 255 || c3.g != 0x88 || c3.b != 0) ok = false;
    if (ok) OK(); else FAIL("color name");
}

int main() {
    printf("=== DaadGraphics Unit Tests ===\n\n");

    test_canvas_create();
    test_color_set();
    test_rectangle_fill();
    test_stroke_rect();
    test_circle();
    test_line();
    test_clear();
    test_background();
    test_fill_area();
    test_pixel_coords();
    test_out_of_bounds();
    test_stroke_circle();
    test_save_load();
    test_multiple_colors();
    test_color_name();

    printf("\n=== Results: %d/%d passed ===\n", passed, tests);
    return (passed == tests) ? 0 : 1;
}

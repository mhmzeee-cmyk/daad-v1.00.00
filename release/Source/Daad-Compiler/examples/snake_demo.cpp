/**
 * @file snake_demo.cpp
 * @brief لعبة ثعبان مكتبية — Daad Graphics ANSI Terminal
 *        تجربة لطبقة الرسومياتWITHOUT any dependencies
 *        ضغط WASD للتحكم، q للخروج
 */

#include "DaadGraphics.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <deque>

using namespace daad::graphics;

const int GRID_W = 40;
const int GRID_H = 20;
const int CELL = 2;
const int SCREEN_W = GRID_W * CELL;
const int SCREEN_H = GRID_H * CELL;

struct Point { int x, y; };

int main() {
    std::deque<Point> snake;
    snake.push_back({GRID_W / 2, GRID_H / 2});
    snake.push_back({GRID_W / 2 - 1, GRID_H / 2});
    snake.push_back({GRID_W / 2 - 2, GRID_H / 2});

    Point food = {rand() % GRID_W, rand() % GRID_H};
    int dx = 1, dy = 0;
    int score = 0;
    bool running = true;

    setCanvasSize(SCREEN_W, SCREEN_H);

    auto frame_time = std::chrono::milliseconds(120);

    while (running) {
        auto start = std::chrono::steady_clock::now();

        // --- Draw ---
        background(10, 10, 30);

        // Grid border
        setColor(60, 60, 80);
        strokeRect(0, 0, SCREEN_W, SCREEN_H);

        // Food
        setColor(220, 50, 50);
        rectangle(food.x * CELL + 1, food.y * CELL + 1, CELL - 2, CELL - 2);

        // Snake
        for (size_t i = 0; i < snake.size(); i++) {
            int bright = 100 + static_cast<int>(155 * (1.0 - (double)i / snake.size()));
            setColor(50, bright, 50);
            rectangle(snake[i].x * CELL, snake[i].y * CELL, CELL, CELL);
        }

        // Head highlight
        setColor(100, 255, 100);
        rectangle(snake[0].x * CELL, snake[0].y * CELL, CELL, CELL);

        // Score text (simple)
        char score_buf[32];
        snprintf(score_buf, sizeof(score_buf), "Score: %d", score);
        setColor(255, 255, 255);
        // Draw score at top-right (right-aligned manually)
        int tx = SCREEN_W - 10 * 6;
        for (int i = 0; score_buf[i]; i++) {
            char c = score_buf[i];
            if (c >= 32 && c <= 90) {
                int idx = c - 32;
                for (int row = 0; row < 7; row++)
                    for (int col = 0; col < 5; col++) {
                        bool on = (FONT_5x7[idx][col] >> row) & 1;
                        if (on)
                            pixel(tx + col, 2 + row);
                    }
            }
            tx += 6;
        }

        render();

        // --- Input (non-blocking via terminal raw mode) ---
        // Use ANSI query + timeout approach
        // For simplicity, use system-level kbhit if available
        {
            struct timeval tv = {0, 0};
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);
            // No wait - just check
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
                char ch = 0;
                read(STDIN_FILENO, &ch, 1);
                if (ch == 'q' || ch == 'Q') running = false;
                else if ((ch == 'w' || ch == 'W' || ch == 13) && dy != 1) { dx = 0; dy = -1; }
                else if ((ch == 's' || ch == 'S') && dy != -1) { dx = 0; dy = 1; }
                else if ((ch == 'a' || ch == 'A') && dx != 1) { dx = -1; dy = 0; }
                else if ((ch == 'd' || ch == 'D') && dx != -1) { dx = 1; dy = 0; }
            }
        }

        // --- Move ---
        Point head = {snake[0].x + dx, snake[0].y + dy};

        // Wall collision (wrap around)
        if (head.x < 0) head.x = GRID_W - 1;
        if (head.x >= GRID_W) head.x = 0;
        if (head.y < 0) head.y = GRID_H - 1;
        if (head.y >= GRID_H) head.y = 0;

        // Self collision
        for (size_t i = 0; i < snake.size(); i++) {
            if (snake[i].x == head.x && snake[i].y == head.y) {
                running = false;
                break;
            }
        }

        if (!running) break;

        snake.push_front(head);

        // Eat food
        if (head.x == food.x && head.y == food.y) {
            score += 10;
            food = {rand() % GRID_W, rand() % GRID_H};
        } else {
            snake.pop_back();
        }

        // Frame timing
        auto end = std::chrono::steady_clock::now();
        auto elapsed = end - start;
        if (elapsed < frame_time) {
            std::this_thread::sleep_for(frame_time - elapsed);
        }
    }

    // Game over screen
    background(0, 0, 0);
    setColor(255, 0, 0);
    const char* msg = "GAME OVER";
    int mx = SCREEN_W / 2 - (strlen(msg) * 6) / 2;
    int my = SCREEN_H / 2 - 4;
    for (int i = 0; msg[i]; i++) {
        char c = msg[i];
        if (c >= 32 && c <= 90) {
            int idx = c - 32;
            for (int row = 0; row < 7; row++)
                for (int col = 0; col < 5; col++) {
                    bool on = (FONT_5x7[idx][col] >> row) & 1;
                    if (on)
                        for (int dy = 0; dy < 3; dy++)
                            for (int dx = 0; dx < 3; dx++)
                                pixel(mx + i * 18 + col * 3 + dx, my + row * 3 + dy);
                }
        }
    }
    char score_msg[64];
    snprintf(score_msg, sizeof(score_msg), "Score: %d", score);
    setColor(255, 255, 255);
    int sx = SCREEN_W / 2 - (strlen(score_msg) * 6) / 2;
    for (int i = 0; score_msg[i]; i++) {
        char c = score_msg[i];
        if (c >= 32 && c <= 90) {
            int idx = c - 32;
            for (int row = 0; row < 7; row++)
                for (int col = 0; col < 5; col++) {
                    bool on = (FONT_5x7[idx][col] >> row) & 1;
                    if (on)
                        pixel(sx + i * 6 + col, my + 30 + row);
                }
        }
    }
    render();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}

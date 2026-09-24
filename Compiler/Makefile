# ض استديو — بناء سريع بدون CMake (للتطوير السريع فقط)
# للاستخدام الإنتاجي استخدم: cmake -B build && cmake --build build

CXX      = g++
CXXFLAGS = -std=c++20 -Iinclude -Wall -Wextra -O2
SRC      = $(wildcard src/*.cpp)
BUILD    = build-make
BIN      = $(BUILD)/bin/daad-compiler

.PHONY: all clean test run

all: $(BIN)

$(BIN): $(SRC) $(wildcard include/Daad/*.hpp)
	@mkdir -p $(BUILD)/bin
	$(CXX) $(CXXFLAGS) $(SRC) -o $@

test:
	@echo "لا توجد اختبارات وحدة بعد. استخدم: bash Tests/real_regression.sh"
	@bash ../Tests/real_regression.sh 2>&1 | tail -n 20; exit $${PIPESTATUS[0]}

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(BUILD)

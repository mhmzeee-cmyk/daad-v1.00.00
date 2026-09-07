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

test: $(BIN)
	@cd .. && cmake -B build -DCMAKE_BUILD_TYPE=Release > /dev/null && \
	 cmake --build build -j$$(nproc) > /dev/null && ./build/bin/DaadTests

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(BUILD)

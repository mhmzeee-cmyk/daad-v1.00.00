# Daad Studio - Handover Guide

Welcome to the Daad Studio project! This guide provides an overview of the architecture, how to maintain and extend the compiler, and instructions for building and running the project on different platforms.

## 1. Project Architecture Overview

The Daad Studio project is structured as follows:

-   **Core Engine:** (`src/`, `include/Daad/`) Contains the compiler's fundamental components: Lexer, Parser, AST, CodeGen, Optimizer, Runtime, and Stdlib. It's designed for cross-platform compatibility using standard C++20.
-   **Frontend IDE:** (`frontend/`) Built with Qt5 (QML for UI, C++ for backend logic), providing a user-friendly interface for editing, compiling, and debugging Daad code. It uses `CompilerWrapper` for thread-safe interaction with the core engine.
-   **VS Code Extension:** (`vscode-extension/`) Enhances the developer experience with syntax highlighting, snippets, and an integrated compiler command.
-   **Build System:** (`CMakeLists.txt`, `frontend/CMakeLists.txt`, `cmake/platforms/`) Manages cross-platform builds and dependencies. Qt5 and GTest are integrated.
-   **CI/CD Pipeline:** (`.github/workflows/ci-cd.yml`) Automates code quality checks, builds, testing, and packaging for all target platforms upon code changes.
-   **Documentation:** (`docs/`) Contains crucial handover documents, including this guide, licensing information, and security measures.

## 2. Maintaining and Extending the Compiler

### 2.1 Compiler Core (`src/`, `include/Daad/`)

-   **Lexer & Parser:** Implemented using recursive descent. Extend by adding new tokens and grammar rules to `src/Lexer.cpp`, `include/Daad/Token.hpp`, `include/Daad/Parser.hpp`, and `src/Parser.cpp`.
-   **AST:** The Abstract Syntax Tree is designed for extensibility. Add new AST nodes in `include/Daad/AST.hpp` and implement their `accept()` methods. Ensure all Visitors (CodeGen, Optimizer, Diagnostics) are updated.
-   **CodeGen & Optimizer:** These are implemented using the Visitor pattern. To add new language features, extend the `ASTVisitor` interface and implement the corresponding `visit()` methods in `CodeGenVisitor.cpp` and `OptimizerVisitor.cpp`.
-   **Runtime & Stdlib:** Located in `include/Daad/Runtime/` and `stdlib/`. Extend by adding new platform-agnostic functions or wrappers for platform-specific APIs.

### 2.2 Frontend IDE (`frontend/`)

-   **QML:** UI elements are defined in `.qml` files. Extend by adding new components or modifying existing ones.
-   **C++ Backend:** `CompilerWrapper.hpp/cpp` handles communication between QML and the core engine. Extend by adding new signals/slots or improving thread management.
-   **Syntax Highlighting:** Modify `SyntaxHighlighter.cpp` to add or update highlighting rules.

### 2.3 VS Code Extension (`vscode-extension/`)

-   **TypeScript:** The extension logic is in `src/extension.ts`. Extend by adding new commands, features, or improving existing ones.
-   **TextMate Grammar:** Modify `syntaxes/daad.tmLanguage.json` for syntax highlighting rules.
-   **Snippets:** Add new code snippets in `snippets/daad.code-snippets`.

## 3. Building and Running

### 3.1 Build System (CMake)

-   **Core Engine:** Navigate to the project root and run:
    ```bash
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build .
    ctest --test-dir build
    ```
-   **Frontend IDE:** Navigate to `frontend/` and run:
    ```bash
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/your/Qt5/installation
    cmake --build .
    ./DaadStudioIDE
    ```
    *(Note: Adjust `CMAKE_PREFIX_PATH` for your Qt5 installation.)*

### 3.2 Platform-Specific Builds

-   **Windows:** Use the `frontend/build/DaadStudioIDE.exe` (after running `windeployqt`).
-   **macOS:** Use the `frontend/build/DaadStudioIDE.app` bundle.
-   **Linux:** Use the `build/DaadStudioIDE` executable.
-   **Android/iOS:** Build via CI/CD artifacts or adapt CMake for direct builds using NDK/Xcode.
-   **VS Code Extension:** Install the `daad-language.vsix` artifact into VS Code.

## 4. CI/CD Pipeline

The GitHub Actions pipeline (`.github/workflows/ci-cd.yml`) automates quality checks, builds across all platforms, testing, and packaging. It's configured to run on pushes to `main`/`develop` branches and on release events.

## 5. Next Steps & Recommendations

-   Implement full language features (error handling, control flow, OOP, templates).
-   Enhance the Stdlib with more Arabic-wrapped functions.
-   Improve the Optimizer with more sophisticated analysis (e.g., dead code elimination).
-   Refine the UI/UX of the IDE.

This guide provides a starting point. Continuous development and testing are key to maintaining a high-quality product.
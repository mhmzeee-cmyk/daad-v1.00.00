# Daad Studio - License Hints

This document provides guidance on licensing considerations for the Daad Studio project. It is crucial for a commercial product to ensure all components are properly licensed, especially when distributing to customers or selling intellectual property.

## 1. Core Engine (Daad Compiler)

-   **License:** The core compiler components (`src/`, `include/Daad/`) are developed in-house and are intended to be proprietary intellectual property of Daad Studio. They should be distributed under a commercial license agreement with the buyer.
-   **Dependencies:** The core engine relies on the C++20 standard library and GoogleTest. C++ standard library is generally available and does not impose licensing restrictions on distributed software. GoogleTest is licensed under the Apache 2.0 license, which is permissive and generally compatible with commercial use, but requires attribution.

## 2. Frontend IDE (Qt5)

-   **License:** The Daad Studio IDE frontend uses Qt5.
    -   **Qt Licensing:** Qt offers both Commercial and Open Source (LGPLv3) licenses. If Daad Studio is distributed as a closed-source commercial product, a Qt Commercial License is likely required. If the Daad Studio IDE itself were to be open-sourced under a compatible license, the LGPLv3 might be applicable.
    -   **Recommendation:** Due to the commercial nature and intended sale of IP, a Qt Commercial License is the safest and most appropriate choice for the IDE's distribution to customers. This avoids complexities related to LGPL's copyleft provisions.
-   **Dependencies:** Qt Quick, QML, Core, Network are used. Ensure compliance with Qt's licensing terms for each component used.

## 3. VS Code Extension

-   **License:** The VS Code extension is typically distributed under a permissive open-source license (e.g., MIT License) unless specific commercial terms are intended for the extension itself.
-   **Dependencies:** Node.js, TypeScript, VS Code API, `vsce` for packaging. These have their own licensing terms (typically permissive).

## 4. CI/CD Tools & Dependencies

-   **GitHub Actions:** Free for public repositories, with usage limits for private repos. Usage is governed by GitHub's terms.
-   **Build Tools:** CMake, clang-format, clang-tidy, Node.js, npm, vsce, 7z, Qt deploy tools. These are standard developer tools with permissive licenses or are part of the development environment.

## 5. Important Considerations for Commercial Sale:

-   **Due Diligence:** Before selling the IP, conduct thorough due diligence on all third-party components. Confirm licenses explicitly permit commercial redistribution and modification as needed.
-   **Legal Counsel:** It is **highly recommended** to consult with legal counsel specializing in intellectual property and software licensing. They can provide definitive advice tailored to the specific transaction and jurisdiction.
-   **Attribution:** Ensure all required attributions (e.g., for GoogleTest, potentially Qt if using LGPL) are included in the distributed product's documentation or "About" sections as per license terms.
-   **Proprietary Code:** Clearly demarcate proprietary code from open-source dependencies in all documentation and internal structures.

This guide is for informational purposes only and does not constitute legal advice. Always consult with qualified legal professionals.
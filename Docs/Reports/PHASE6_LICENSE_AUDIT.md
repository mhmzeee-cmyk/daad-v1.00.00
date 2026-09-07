# PHASE6_LICENSE_AUDIT.md — legal TODO, no opinions invented

- `Daad-Compiler/LICENSE`: EXISTS but 0 bytes → RELEASE BLOCKER / LEGAL TODO (R1). Owner must choose text; I choose nothing.
- Third-party (inventory only): Qt 5.15 (LGPL/GPL/commercial tri-license — packager must confirm linking mode), Electron 33 (MIT + Chromium deps), Prisma 5.22 (Apache-2.0), Node pkgs (mostly MIT/ISC — full SBOM via `npm ls` at packaging), GoogleTest (BSD, test-only fetch), VSCode API (product EULA applies to host, ext code is owner's), system libc/Qt (distro packages).
- No license texts bundled for deps → packaging must add attributions where required.

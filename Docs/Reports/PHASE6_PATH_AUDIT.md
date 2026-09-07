# PHASE6_PATH_AUDIT.md — portability verdicts

- PORTABLE: generated C++ includes (`Daad/...`, `stdlib/...` via single `-I<Daad-Compiler>` — proven incl. relocated /tmp copy); Electron `__dirname`-anchored loads; bat `%~dp0`; QML `qrc:/`; relative frontend assets (52 refs, 0 missing).
- One absolute path in a COMMENT (`dhad-libraries.js:4` `C:\Projects\...`) — harmless, INFORMATIONAL.
- ENVIRONMENT-DEPENDENT (by design, documented): Electron `../Dhad-Studio-Full/...` layout (dev-tree layout; packager must flatten or adjust); VSCode→`daad-compiler` on PATH (user installs); server→`DATABASE_URL` file path; frontend→API base (localhost:3000 default, same-host-relative on :3000).
- BROKEN: none remaining (Phase-2 fixed the 4 Electron refs; icon.ico asset still absent → packaging TODO).
- vfat: no exec bits (release process must chmod ELF/sh); Arabic UTF-8 filenames work (proven end-to-end).

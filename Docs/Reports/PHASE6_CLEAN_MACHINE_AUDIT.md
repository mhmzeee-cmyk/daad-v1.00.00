# PHASE6_CLEAN_MACHINE_AUDIT.md — what breaks without dev assumptions

Proven in /tmp (relocated tree + `env -i`): compiler cmake+build PASS, e2e prints 5 → compiler needs ONLY g++/cmake + sources (CLEAN PASS).
Server needs: node18 + `npm install` (network for registry+prisma engines) + env + `db push`; without them: BLOCKED (expected, documented steps exist).
Frontend needs: static host + API base reachable (localhost default) + fonts CDN for full fidelity (offline gap).
Electron needs: Full tree + installed server deps (BLOCKED otherwise).
Bridge needs: Qt5 runtime. VSCode ext needs: tsc build + daad-compiler on PATH. CPU sims need: nothing (libc) / Qt5 (GUI).
/tmp/dhad-phase6-clean + /tmp/dhad-phase6-relocated used; nothing installed into project.

# PHASE45_BRIDGE_REGRESSION.md — rebuild + offscreen + API (unchanged sources)

cmake 0, build 0 (DhadBridge ELF); offscreen 10s survival (exit 124, no crash); qrc resources embedded (Phase-3 evidence stands).
API re-verified: login 200, onboard 201 with UNIQUE pair, request-otp 200, wrong-otp 401.
No Bridge file modified in Phase 4 → no regression possible; verdict unchanged PARTIALLY (GUI interaction needs display).

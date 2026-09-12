# PHASE45_SERVER_REGRESSION.md — full sweep (:3200, patched routes)

Health/ready/live 200 · prisma validate/generate/push 0 · register/login/logout per AUTH doc.
Endpoints: student/profile,challenges,courses,leaderboard,workspace/* 200 · teacher/students,analytics 200 (teacher) · RBAC 403 ×4 (student→teacher/reports/security; teacher→student-only) · no-auth 401 · bad-challenge 404 · malformed 400 · teacher-register w/o auth 401.
Evaluator: correct→passed 100/xp100; repeat→alreadyPassed; wrong-id→404.
OTP: request 200, wrong-code 401, no-leak in logs.
Onboard (admin): 201, activationCodes unique pair re-verified (582374≠253438).
Verdict: SERVER WORKS; no Phase-3 behavior lost.

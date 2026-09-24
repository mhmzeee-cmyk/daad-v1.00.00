// ====================================================
// Dhad Studio - CD Pipeline Tests
// ====================================================
// Validates deployment readiness and production config
// Run: node tests/cd-pipeline.js
// ====================================================

const fs = require("fs");
const path = require("path");
const { execSync } = require("child_process");

const ROOT = path.resolve(__dirname, "..");
const SERVER_ROOT = path.join(ROOT, "server");
const FRONTEND_ROOT = path.join(ROOT, "frontend-web");
const DESKTOP_ROOT = path.join(ROOT, "desktop-app");

let passed = 0;
let failed = 0;
let total = 0;
let warnings = 0;

function assert(condition, testName, severity = "error") {
  total++;
  if (condition) {
    passed++;
    console.log(`  ✓ ${testName}`);
  } else {
    if (severity === "warning") {
      warnings++;
      console.log(`  ⚠ ${testName} (warning)`);
    } else {
      failed++;
      console.log(`  ✗ ${testName}`);
    }
  }
}

function fileExists(filePath) {
  return fs.existsSync(path.join(ROOT, filePath));
}

function readJSON(filePath) {
  try {
    return JSON.parse(fs.readFileSync(path.join(ROOT, filePath), "utf8"));
  } catch {
    return null;
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test 1: Project Structure                                                   │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function testProjectStructure() {
  console.log("\n== 1. Project Structure ==");

  // Core directories
  assert(fileExists("server"), "server/ directory exists");
  assert(fileExists("frontend-web"), "frontend-web/ directory exists");
  assert(fileExists("desktop-app"), "desktop-app/ directory exists");
  assert(fileExists("bridge"), "bridge/ directory exists");
  assert(fileExists("tests"), "tests/ directory exists");

  // No build artifacts
  assert(!fileExists("build"), "No build/ artifacts");
  assert(!fileExists("bridge/build"), "No bridge/build/ artifacts");
  assert(!fileExists("desktop-app/node_modules"), "No desktop-app/node_modules/");

  // No temp files
  assert(!fileExists("tests/check-classroom.js"), "No temp test files");
  assert(!fileExists("tests/check-classrooms.js"), "No temp test files");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test 2: Server Package Configuration                                        │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function testServerPackage() {
  console.log("\n== 2. Server Package Configuration ==");

  const pkg = readJSON("server/package.json");
  assert(pkg !== null, "server/package.json is valid JSON");
  if (!pkg) return;

  assert(pkg.name === "dhad-studio-server", "Package name is correct");
  assert(pkg.version, "Package has version");
  assert(pkg.scripts?.start, "Has start script");
  assert(pkg.scripts?.dev, "Has dev script");
  assert(pkg.scripts?.["db:push"], "Has db:push script");

  // Required dependencies
  const requiredDeps = [
    "express", "@prisma/client", "jsonwebtoken", "bcryptjs",
    "helmet", "compression", "express-rate-limit", "winston", "dotenv"
  ];
  for (const dep of requiredDeps) {
    assert(pkg.dependencies?.[dep], `Has dependency: ${dep}`);
  }

  // No unused dependencies
  assert(!pkg.dependencies?.morgan, "No unused morgan dependency");
  assert(!pkg.dependencies?.["@google/generative-ai"], "No unused @google/generative-ai");
  assert(!pkg.dependencies?.cors, "No unused cors (handled manually)");

  // Node engine
  assert(pkg.engines?.node, "Has node engine requirement");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test 3: Server Source Code Quality                                          │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function testServerCodeQuality() {
  console.log("\n== 3. Server Source Code Quality ==");

  // Core files exist
  assert(fileExists("server/src/index.js"), "Main entry point exists");
  assert(fileExists("server/src/router/index.js"), "Router exists");
  assert(fileExists("server/src/middlewares/auth.js"), "Auth middleware exists");
  assert(fileExists("server/src/middlewares/strictSecurity.js"), "Security middleware exists");
  assert(fileExists("server/src/middlewares/security.js"), "Security middleware exists");
  assert(fileExists("server/src/utils/logger.js"), "Logger exists");
  assert(fileExists("server/src/utils/prisma.js"), "Prisma singleton exists");

  // Controllers exist
  const controllers = [
    "authController.js", "otpController.js", "studentManagementController.js",
    "studentProfileController.js", "assessmentController.js", "analyticsController.js",
    "healthController.js", "onboardController.js"
  ];
  for (const ctrl of controllers) {
    assert(fileExists(`server/src/controllers/${ctrl}`), `Controller exists: ${ctrl}`);
  }

  // Routes exist
  const routes = ["auth.js", "otp.js", "studentManagement.js", "student.js", "analytics.js", "security.js", "onboard.js", "invitations.js"];
  for (const route of routes) {
    assert(fileExists(`server/src/routes/${route}`), `Route exists: ${route}`);
  }

  // No dead code files
  const deadFiles = [
    "server/src/routes/challenges.js", "server/src/routes/reports.js", "server/src/routes/schools.js",
    "server/src/controllers/challengeController.js", "server/src/controllers/reportsController.js",
    "server/src/controllers/schoolController.js", "server/src/services/aiFeedbackService.js",
    "server/src/middlewares/errorHandler.js", "server/src/middlewares/activityTracker.js",
    "server/src/config/db.js", "server/src/utils/secrets.js", "server/src/utils/webhookNotifier.js"
  ];
  for (const dead of deadFiles) {
    assert(!fileExists(dead), `No dead file: ${dead}`);
  }

  // No dead directories
  const deadDirs = ["server/src/judge", "server/src/lexer", "server/src/parser", "server/src/socket", "server/src/storage", "server/src/analytics", "server/src/themes", "server/src/sandbox"];
  for (const dead of deadDirs) {
    assert(!fileExists(dead), `No dead directory: ${dead}`);
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test 4: Security Configuration                                              │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function testSecurityConfig() {
  console.log("\n== 4. Security Configuration ==");

  // .env.example exists with all required vars
  assert(fileExists("server/.env.example"), ".env.example exists");

  const envExample = fs.readFileSync(path.join(SERVER_ROOT, ".env.example"), "utf8");
  assert(envExample.includes("JWT_SECRET"), ".env.example has JWT_SECRET");
  assert(envExample.includes("HMAC_SECRET"), ".env.example has HMAC_SECRET");
  assert(envExample.includes("BRIDGE_SECRET"), ".env.example has BRIDGE_SECRET");
  assert(envExample.includes("DATABASE_URL"), ".env.example has DATABASE_URL");

  // .gitignore includes .env
  const gitignore = fs.readFileSync(path.join(ROOT, ".gitignore"), "utf8");
  assert(gitignore.includes(".env"), ".gitignore includes .env");

  // Prisma schema exists
  assert(fileExists("server/prisma/schema.prisma"), "Prisma schema exists");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test 5: Frontend Structure                                                  │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function testFrontendStructure() {
  console.log("\n== 5. Frontend Structure ==");

  // Core files
  assert(fileExists("frontend-web/js/api.js"), "API client exists");
  assert(fileExists("frontend-web/css/style.css"), "Stylesheet exists");

  // All HTML pages
  const pages = [
    "index.html", "login.html", "register.html", "activate.html",
    "pages/student-dashboard.html", "pages/teacher-dashboard.html",
    "pages/challenges.html", "pages/leaderboard.html", "pages/roadmap.html",
    "pages/profile.html", "pages/achievements.html", "pages/classrooms.html",
    "pages/students.html", "pages/assessments.html", "pages/reports.html"
  ];
  for (const page of pages) {
    assert(fileExists(`frontend-web/${page}`), `Page exists: ${page}`);
  }

  // No broken API references
  const apiContent = fs.readFileSync(path.join(FRONTEND_ROOT, "js/api.js"), "utf8");
  assert(apiContent.includes("/api/v1/"), "API uses /api/v1/ prefix");
  assert(!apiContent.includes("/api/auth/login"), "No old /api/auth/ paths");
  assert(!apiContent.includes("/api/challenges"), "No non-existent /api/challenges path");
  assert(!apiContent.includes("/api/telemetry"), "No old telemetry path");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test 6: Desktop App Structure                                               │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function testDesktopApp() {
  console.log("\n== 6. Desktop App Structure ==");

  assert(fileExists("desktop-app/main.js"), "Electron main.js exists");
  assert(fileExists("desktop-app/preload.js"), "Preload script exists");
  assert(fileExists("desktop-app/splash.html"), "Splash screen exists");
  assert(fileExists("desktop-app/package.json"), "Package.json exists");

  const pkg = readJSON("desktop-app/package.json");
  assert(pkg?.name === "dhad-studio", "Desktop package name is correct");
  assert(pkg?.main === "main.js", "Main entry is main.js");

  // No duplicate server/frontend
  assert(!fileExists("desktop-app/server"), "No duplicate server/");
  assert(!fileExists("desktop-app/frontend-web"), "No duplicate frontend-web/");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test 7: Docker & Deployment Config                                          │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function testDeploymentConfig() {
  console.log("\n== 7. Deployment Configuration ==");

  // Docker
  assert(fileExists("Dockerfile") || fileExists("server/Dockerfile"), "Dockerfile exists");

  // Cloud configs
  assert(fileExists("render.yaml") || fileExists("server/render.yaml"), "Render config exists");

  // Scripts
  assert(fileExists("deploy.sh") || fileExists("server/deploy.sh"), "Deploy script exists");
  assert(fileExists("start.sh") || fileExists("server/start.sh"), "Start script exists");

  // Environment template
  assert(fileExists("template.env") || fileExists("server/template.env"), "Template env exists");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test 8: No Default Passwords in Code                                        │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function testNoDefaultPasswords() {
  console.log("\n== 8. No Default Passwords in Code ==");

  // Check server source files for default password patterns
  const srcDir = path.join(SERVER_ROOT, "src");
  const files = [
    "controllers/authController.js",
    "controllers/studentManagementController.js",
    "controllers/onboardController.js",
    "services/adminService.js",
  ];

  for (const file of files) {
    const content = fs.readFileSync(path.join(srcDir, file), "utf8");
    // Check for default password fallback pattern
    const hasDefault = content.includes('|| "123456"') || content.includes("|| '123456'");
    assert(!hasDefault, `No default password in ${file}`);
  }

  // Swagger should not have default password
  const swagger = fs.readFileSync(path.join(SERVER_ROOT, "src/config/swagger.js"), "utf8");
  assert(!swagger.includes("default: '123456'"), "Swagger has no default password");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test 9: Prisma Schema سلامة البيانات                                          │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function testPrismaSchema() {
  console.log("\n== 9. Prisma Schema ==");

  const schema = fs.readFileSync(path.join(SERVER_ROOT, "prisma/schema.prisma"), "utf8");

  // Required models
  const requiredModels = [
    "User", "School", "Classroom", "StudentProfile",
    "Submission", "Challenge", "Lesson",
    "Attendance", "ActivityLog", "LoginLog",
    "Assessment", "StudentAssessmentResult",
    "Achievement", "UserAchievement", "DailyXP",
    "Invitation", "SecurityAlert", "CloudWorkspace"
  ];

  for (const model of requiredModels) {
    assert(schema.includes(`model ${model}`), `Schema has model: ${model}`);
  }

  // No ambiguous relations (check for @relation with explicit fields)
  assert(!schema.includes("teacherId") || schema.includes("@relation"), "Relations are properly defined");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Test 10: Test Files                                                         │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function testTestFiles() {
  console.log("\n== 10. Test Files ==");

  assert(fileExists("tests/pentest.js"), "Penetration test exists");
  assert(fileExists("tests/integration.js"), "Integration test exists");
  assert(fileExists("tests/cd-pipeline.js"), "CD pipeline test exists");

  // No temp test files
  assert(!fileExists("server/check-classroom.js"), "No temp files in server/");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// │   Run All Tests                                                               │
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

function runAll() {
  console.log("==========================================================");
  console.log("  CD PIPELINE TESTS - Dhad Studio");
  console.log("  " + new Date().toISOString());
  console.log("==========================================================");

  testProjectStructure();
  testServerPackage();
  testServerCodeQuality();
  testSecurityConfig();
  testFrontendStructure();
  testDesktopApp();
  testDeploymentConfig();
  testNoDefaultPasswords();
  testPrismaSchema();
  testTestFiles();

  console.log("\n==========================================================");
  console.log(`  RESULTS: ${passed}/${total} passed, ${failed} failed, ${warnings} warnings`);
  console.log(`  Score: ${Math.round((passed / total) * 100)}%`);
  console.log("==========================================================");

  process.exit(failed > 0 ? 1 : 0);
}

runAll();

# Dhad Studio - Folder Structure

**Developer:** محمد Mahmoud الحموز

---

## Visual Structure

```
📁 dhad-studio/
│
├── 📁 frontend-web/                    # 🌐 Frontend Application
│   ├── 📄 index.html                   # Landing page
│   ├── 📄 welcome.html                 # Welcome/onboarding
│   ├── 📄 login.html                   # Login page
│   ├── 📄 register.html                # Registration
│   ├── 📄 activate.html                # Account activation
│   ├── 📄 book.html                    # Programming book
│   ├── 📄 icons.svg                    # SVG icon sprite
│   ├── 📄 package.json                 # Dependencies
│   │
│   ├── 📁 css/                         # 🎨 Stylesheets
│   │   ├── 📄 style.css               # Main styles
│   │   ├── 📄 themes.css              # 5 themes
│   │   └── 📄 animations.css          # Animations
│   │
│   ├── 📁 js/                          # ⚡ JavaScript
│   │   ├── 📄 api.js                   # API client
│   │   ├── 📄 auth.js                  # Auth guard
│   │   ├── 📄 dhad.js                  # Code transpiler
│   │   ├── 📄 sidebar.js              # Sidebar component
│   │   ├── 📄 toast.js                # Notifications
│   │   ├── 📄 theme-switcher.js       # Theme switching
│   │   ├── 📄 mobile-nav.js           # Mobile nav
│   │   └── 📁 lib/                    # Third-party
│   │       ├── 📄 jspdf.umd.min.js
│   │       └── 📄 html2canvas.min.js
│   │
│   └── 📁 pages/                       # 📄 Pages
│       ├── 📄 teacher-dashboard.html   # Teacher view
│       ├── 📄 student-dashboard.html   # Student view
│       ├── 📄 classrooms.html          # Classrooms
│       ├── 📄 students.html            # Students
│       ├── 📄 assessments.html         # Assessments
│       ├── 📄 challenges.html          # Challenges
│       ├── 📄 course-roadmap.html      # Course roadmap
│       ├── 📄 roadmap.html             # Visual roadmap
│       ├── 📄 leaderboard.html         # Leaderboard
│       ├── 📄 reports.html             # Reports
│       ├── 📄 achievements.html        # Achievements
│       ├── 📄 profile.html             # Profile
│       ├── 📄 settings.html            # Settings
│       ├── 📄 web-editor.html          # Code editor
│       └── 📄 additional-courses.html  # Extra courses
│
├── 📁 server/                          # 🖥️ Backend API
│   ├── 📄 package.json                 # Dependencies
│   ├── 📄 .env                         # Environment
│   ├── 📄 .env.example                 # Env template
│   ├── 📄 Dockerfile                   # Docker config
│   ├── 📄 render.yaml                  # Render deploy
│   │
│   ├── 📁 prisma/                      # 🗄️ Database
│   │   └── 📄 schema.prisma           # DB schema
│   │
│   └── 📁 src/                         # 📦 Source Code
│       ├── 📄 index.js                 # Entry point
│       │
│       ├── 📁 controllers/             # 🎮 Controllers
│       │   ├── 📄 authController.js
│       │   ├── 📄 analyticsController.js
│       │   ├── 📄 assessmentController.js
│       │   ├── 📄 challengeController.js
│       │   ├── 📄 courseController.js
│       │   ├── 📄 healthController.js
│       │   ├── 📄 onboardController.js
│       │   ├── 📄 otpController.js
│       │   ├── 📄 reportsController.js
│       │   ├── 📄 schoolController.js
│       │   ├── 📄 studentManagementController.js
│       │   └── 📄 studentProfileController.js
│       │
│       ├── 📁 middlewares/             # 🔒 Middlewares
│       │   ├── 📄 auth.js
│       │   ├── 📄 errorHandler.js
│       │   ├── 📄 security.js
│       │   └── 📄 strictSecurity.js
│       │
│       ├── 📁 routes/                  # 🛤️ Routes
│       │   ├── 📄 auth.js
│       │   ├── 📄 analytics.js
│       │   ├── 📄 challenge.js
│       │   ├── 📄 challenges.js
│       │   ├── 📄 courses.js
│       │   ├── 📄 invitations.js
│       │   ├── 📄 onboard.js
│       │   ├── 📄 otp.js
│       │   ├── 📄 reports.js
│       │   ├── 📄 schools.js
│       │   ├── 📄 security.js
│       │   ├── 📄 student.js
│       │   └── 📄 studentManagement.js
│       │
│       ├── 📁 services/                # 🔧 Services
│       │   └── 📄 adminService.js
│       │
│       └── 📁 utils/                   # 🛠️ Utilities
│           └── 📄 logger.js
│
├── 📁 assets/                          # 🖼️ Static Assets
├── 📁 config/                          # ⚙️ Configuration
├── 📁 docs/                            # 📚 Documentation
├── 📁 tests/                           # 🧪 Tests
├── 📁 examples/                        # 📝 Examples
├── 📁 desktop-app/                     # 🖥️ Electron App
├── 📁 vscode-extension/                # 📝 VS Code Extension
├── 📁 stdlib/                          # 📖 Standard Library
│
├── 📄 README.md                        # Project README
├── 📄 PROJECT_STRUCTURE.md             # This file
├── 📄 LICENSE                          # License
├── 📄 Dockerfile                       # Docker config
├── 📄 docker-compose.yml               # Docker Compose
├── 📄 CMakeLists.txt                   # CMake config
└── 📄 template.env                     # Env template
```

---

## Quick Navigation

### Frontend
| Path | Description |
|------|-------------|
| `frontend-web/index.html` | Landing page |
| `frontend-web/pages/` | All dashboard pages |
| `frontend-web/css/` | Stylesheets |
| `frontend-web/js/` | JavaScript files |

### Backend
| Path | Description |
|------|-------------|
| `server/src/index.js` | API entry point |
| `server/src/controllers/` | Business logic |
| `server/src/routes/` | API routes |
| `server/src/middlewares/` | Auth & security |
| `server/prisma/schema.prisma` | Database schema |

---

**Last Updated:** July 2026

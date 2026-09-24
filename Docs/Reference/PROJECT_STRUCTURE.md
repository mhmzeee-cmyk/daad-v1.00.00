# Dhad Studio - Project Structure Documentation

**Developer:** محمد محمود الحموز (Mohammed Mahmoud Al-Hamouz)

---

## Overview

Dhad Studio is an Arabic-first EdTech platform for teaching programming. It features a teacher dashboard, student management, gamification (XP, levels, streaks), AI-powered challenges, and a comprehensive programming book.

---

## Root Directory Structure

```
dhad-studio/
├── frontend-web/          # Frontend web application (HTML/CSS/JS)
├── server/                # Backend Node.js/Express API server
├── desktop-app/           # Electron desktop application
├── vscode-extension/      # VS Code extension
├── assets/                # Static assets (images, icons)
├── config/                # Configuration files
├── docs/                  # Documentation
├── examples/              # Example code
├── tests/                 # Test files
├── stdlib/                # Standard library data
├── include/               # C++ headers
├── src/                   # C++ source files
├── build/                 # Build output
├── installer/             # Installer scripts
├── bridge/                # Bridge modules
├── .github/               # GitHub workflows
├── Dockerfile             # Docker configuration
├── docker-compose.yml     # Docker Compose
├── CMakeLists.txt         # CMake build config
├── README.md              # Project README
└── template.env           # Environment template
```

---

## Frontend Structure (`frontend-web/`)

```
frontend-web/
├── index.html             # Landing page
├── welcome.html           # Welcome/onboarding page
├── login.html             # Login page
├── register.html          # Registration page
├── activate.html          # Account activation
├── book.html              # Programming book (PDF generation)
├── icons.svg              # SVG icon sprite sheet
├── package.json           # Frontend dependencies
│
├── css/
│   ├── style.css          # Main styles
│   ├── themes.css         # Theme definitions (5 themes)
│   └── animations.css     # Animations & effects
│
├── js/
│   ├── api.js             # API client & auth
│   ├── auth.js            # Auth utilities
│   ├── dhad.js            # Code editor (transpiler)
│   ├── sidebar.js         # Sidebar component
│   ├── toast.js           # Toast notifications
│   ├── theme-switcher.js  # Theme switching
│   ├── mobile-nav.js      # Mobile navigation
│   └── lib/               # Third-party libraries
│       ├── jspdf.umd.min.js
│       └── html2canvas.min.js
│
└── pages/
    ├── teacher-dashboard.html    # Teacher dashboard
    ├── student-dashboard.html    # Student dashboard
    ├── classrooms.html           # Classroom management
    ├── students.html             # Student management
    ├── assessments.html          # Assessment management
    ├── challenges.html           # Coding challenges
    ├── course-roadmap.html       # Course roadmap
    ├── roadmap.html              # Visual roadmap
    ├── leaderboard.html          # Leaderboard
    ├── reports.html              # Analytics reports
    ├── achievements.html         # Student achievements
    ├── profile.html              # User profile
    ├── settings.html             # Settings
    ├── web-editor.html           # Code editor
    └── additional-courses.html   # Additional courses
```

### CSS Files

| File | Description |
|------|-------------|
| `style.css` | Core styles, layout, components |
| `themes.css` | 5 themes: terminal, purple, dark, light, ocean |
| `animations.css` | Hover effects, transitions, page animations |

### JavaScript Files

| File | Description |
|------|-------------|
| `api.js` | API client, auth tokens, escapeHtml() |
| `auth.js` | Auth guard, role validation |
| `dhad.js` | JavaScript transpiler for code editor |
| `sidebar.js` | Dynamic sidebar rendering |
| `toast.js` | Toast notification system |
| `theme-switcher.js` | Theme switching logic |
| `mobile-nav.js` | Mobile navigation menu |

---

## Backend Structure (`server/`)

```
server/
├── package.json           # Node.js dependencies
├── .env                   # Environment variables
├── .env.example           # Environment template
├── Dockerfile             # Docker configuration
├── render.yaml            # Render deployment
├── prisma/                # Prisma ORM
│   └── schema.prisma      # Database schema
│
├── src/
│   ├── index.js           # Entry point
│   │
│   ├── config/
│   │   └── (configuration files)
│   │
│   ├── controllers/
│   │   ├── authController.js           # Authentication
│   │   ├── analyticsController.js      # Analytics & stats
│   │   ├── assessmentController.js     # Assessments
│   │   ├── challengeController.js      # Challenges
│   │   ├── courseController.js         # Courses
│   │   ├── healthController.js         # Health check
│   │   ├── onboardController.js        # Onboarding
│   │   ├── otpController.js            # OTP verification
│   │   ├── reportsController.js        # Reports
│   │   ├── schoolController.js         # School management
│   │   ├── studentManagementController.js  # Student CRUD
│   │   └── studentProfileController.js     # Student profile
│   │
│   ├── middlewares/
│   │   ├── auth.js                     # JWT authentication
│   │   ├── errorHandler.js             # Error handling
│   │   ├── security.js                 # Basic security
│   │   └── strictSecurity.js           # Advanced security
│   │
│   ├── routes/
│   │   ├── auth.js                     # Auth routes
│   │   ├── analytics.js                # Analytics routes
│   │   ├── challenge.js                # Challenge routes
│   │   ├── challenges.js               # Challenges list
│   │   ├── courses.js                  # Course routes
│   │   ├── invitations.js              # Invitation routes
│   │   ├── onboard.js                  # Onboarding routes
│   │   ├── otp.js                      # OTP routes
│   │   ├── reports.js                  # Reports routes
│   │   ├── schools.js                  # School routes
│   │   ├── security.js                 # Security routes
│   │   ├── student.js                  # Student routes
│   │   └── studentManagement.js        # Student management
│   │
│   ├── services/
│   │   └── adminService.js             # Admin utilities
│   │
│   └── utils/
│       └── logger.js                   # Winston logger
│
├── scripts/               # Utility scripts
└── pg/                    # PostgreSQL setup
```

---

## API Endpoints

### Authentication
| Method | Endpoint | Description |
|--------|----------|-------------|
| POST | `/auth/login` | User login |
| POST | `/auth/register` | User registration |
| POST | `/auth/refresh` | Refresh token |
| POST | `/auth/logout` | Logout |
| POST | `/auth/student-login` | Student login |

### Students
| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/students` | List students |
| POST | `/students` | Create student |
| PUT | `/students/:id` | Update student |
| DELETE | `/students/:id` | Delete student |
| POST | `/students/bulk` | Bulk create |

### Challenges
| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/challenges` | List challenges |
| POST | `/challenges/:id/submit` | Submit solution |

### Analytics
| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/analytics/dashboard` | Dashboard stats |
| GET | `/analytics/leaderboard` | Leaderboard |
| GET | `/analytics/roadmap` | Course roadmap |

---

## Themes

| Theme | Primary Color | Description |
|-------|---------------|-------------|
| `terminal` | #10b981 | Green terminal style |
| `purple` | #818cf8 | Purple classic |
| `dark` | #60a5fa | Dark blue |
| `light` | #2563eb | Light mode |
| `ocean` | #22d3ee | Ocean blue |

---

## Security Features

- JWT authentication with separate access/refresh secrets
- Rate limiting (in-memory)
- Account lockout after failed attempts
- Input sanitization
- CORS protection
- XSS prevention via escapeHtml()
- Password strength validation

---

## Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `DATABASE_URL` | PostgreSQL connection string | Required |
| `JWT_SECRET` | Access token secret | Required |
| `JWT_REFRESH_SECRET` | Refresh token secret | JWT_SECRET |
| `JWT_ACCESS_EXPIRES` | Access token expiry | 1h |
| `JWT_REFRESH_EXPIRES` | Refresh token expiry | 7d |
| `PORT` | Server port | 3000 |
| `NODE_ENV` | Environment | development |
| `ALLOWED_ORIGINS` | CORS origins | localhost |

---

## File Naming Conventions

- **HTML files:** kebab-case (`teacher-dashboard.html`)
- **JS files:** camelCase (`themeSwitcher.js`) or kebab-case (`mobile-nav.js`)
- **CSS files:** kebab-case (`animations.css`)
- **Controllers:** PascalCase (`authController.js`)
- **Routes:** camelCase (`studentManagement.js`)

---

## Developer Signature

All source files are signed with:
```
/* Developer: محمد محمود الحموز | Dhad Studio */
```

---

**Last Updated:** July 2026

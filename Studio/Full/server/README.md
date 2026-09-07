# Dhad Studio API Server

REST API backend for the Dhad Studio EdTech platform. Built with Node.js, Express, Prisma, and Supabase PostgreSQL.

## Quick Start

```bash
# Install dependencies + generate Prisma client
npm install

# Set environment variables (create .env file)
cp .env.example .env
# Edit .env with your values

# Start development server
npm run dev

# Start production server
npm start
```

## Environment Variables

| Variable | Required | Description | Example |
|---|---|---|---|
| `DATABASE_URL` | Yes | PostgreSQL connection string | `postgresql://user:pass@host:5432/db` |
| `JWT_SECRET` | Yes | Secret key for JWT signing | Random 32+ character string |
| `JWT_EXPIRES_IN` | No | Token expiry duration | `7d` (default) |
| `PORT` | No | Server port | `3000` (default) |
| `CORS_ORIGIN` | No | Allowed origins (comma-separated) | `https://frontend.vercel.app` |
| `NODE_ENV` | No | Environment mode | `production` |

## Generate a Secure JWT Secret

```bash
# Windows PowerShell
[Convert]::ToBase64String((1..32 | ForEach-Object { Get-Random -Minimum 0 -Maximum 256 })) -replace '[^a-zA-Z0-9]',''

# Linux/Mac
openssl rand -base64 32
```

## Database Setup

The server uses Prisma ORM with Supabase PostgreSQL. Tables are auto-synced on startup.

```bash
# Push schema to database (development)
npm run db:push

# Generate Prisma client
npx prisma generate

# Seed admin user
npm run db:seed
```

## API Endpoints

### Public
| Method | Endpoint | Description |
|---|---|---|
| `GET` | `/api/health` | Health check (includes DB ping) |
| `POST` | `/api/v1/auth/login` | Login, returns JWT |

### Authenticated (requires `Authorization: Bearer <token>`)
| Method | Endpoint | Access | Description |
|---|---|---|---|
| `POST` | `/api/v1/auth/register` | TEACHER, ADMIN | Create user account |
| `POST` | `/api/v1/auth/change-password` | Any | Change own password |
| `POST` | `/api/v1/auth/reset-password` | ADMIN | Reset user password |
| `GET` | `/api/v1/schools` | Any | List schools |
| `GET` | `/api/v1/schools/:id` | School member | Get school details |
| `PUT` | `/api/v1/schools/:id` | ADMIN | Update school |
| `DELETE` | `/api/v1/schools/:id` | ADMIN | Delete school |
| `GET` | `/api/v1/challenges` | Any | List challenges |
| `POST` | `/api/v1/challenges` | TEACHER | Create challenge |
| `POST` | `/api/v1/submit-solution` | STUDENT | Submit solution |
| `GET` | `/api/v1/submissions` | Any | List submissions |
| `POST` | `/api/v1/onboard-school` | ADMIN | Provision school |

## Deployment

### Render (Recommended)
1. Push to GitHub
2. Create new Web Service on [render.com](https://render.com)
3. Set environment variables in dashboard
4. Deploy

See `render.yaml` for configuration.

### Docker
```bash
docker build -t dhad-studio-api .
docker run -p 3000:3000 \
  -e DATABASE_URL="postgresql://..." \
  -e JWT_SECRET="your-secret" \
  dhad-studio-api
```

### Docker Compose
```bash
docker-compose up -d
```

## Default Credentials

After seeding, use these to login:
- **Admin:** `admin@dhadstudio.com` / `admin456`
- **Teacher:** `ahmed@alnoor.edu` / `123456`
- **Student:** `1098765401` / `123456`

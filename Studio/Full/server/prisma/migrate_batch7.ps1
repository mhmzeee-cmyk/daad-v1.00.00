# ===========================================================
# Batch 7 Migration Helper
# Run this script to apply the schema changes to Supabase
# ===========================================================

# Instructions:
# 1. Ensure DATABASE_URL is set in .env
# 2. Run: npx prisma migrate dev --name batch7_gamification_persistence
# 3. For production: npx prisma migrate deploy

Write-Host "🚀 Batch 7 - Database Schema Migration Helper" -ForegroundColor Cyan
Write-Host "=" * 60

# Check if .env exists
if (-not (Test-Path ".env")) {
    Write-Host "❌ .env file not found!" -ForegroundColor Red
    exit 1
}

# Check if prisma is installed
if (-not (Test-Path "node_modules/.prisma")) {
    Write-Host "📦 Installing dependencies..." -ForegroundColor Yellow
    npm install
}

Write-Host "✅ Prisma client found" -ForegroundColor Green

# Generate Prisma Client
Write-Host "🔧 Generating Prisma Client..." -ForegroundColor Yellow
npx prisma generate

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Prisma generate failed!" -ForegroundColor Red
    exit 1
}

Write-Host "✅ Prisma Client generated" -ForegroundColor Green

# Create migration
Write-Host "📊 Creating database migration..." -ForegroundColor Yellow
npx prisma migrate dev --name batch7_gamification_persistence

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Migration failed!" -ForegroundColor Red
    exit 1
}

Write-Host "✅ Migration created successfully" -ForegroundColor Green

# Seed achievements
Write-Host "🏆 Seeding achievements..." -ForegroundColor Yellow
node prisma/seed.js

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Seeding failed!" -ForegroundColor Red
    exit 1
}

Write-Host "✅ Seeding completed" -ForegroundColor Green

Write-Host "`n🎉 Batch 7 migration completed successfully!" -ForegroundColor Cyan
Write-Host "Database is ready for gamification persistence." -ForegroundColor Green

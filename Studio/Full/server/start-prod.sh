#!/bin/bash
# ============================================
# Dhad Studio - Production Startup Script
# Developer: محمد محمود الحموز | Dhad Studio
# ============================================

set -e

echo ""
echo "========================================"
echo "  Dhad Studio - Production Server"
echo "========================================"
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Check Node.js
if ! command -v node &> /dev/null; then
    echo -e "${RED}[ERROR] Node.js not found! Please install Node.js 18+${NC}"
    exit 1
fi

echo -e "${GREEN}[✓] Node.js $(node -v)${NC}"

# Check PM2
if ! command -v pm2 &> /dev/null; then
    echo -e "${YELLOW}[INFO] Installing PM2 globally...${NC}"
    npm install -g pm2
fi

echo -e "${GREEN}[✓] PM2 $(pm2 -v)${NC}"

# Install dependencies
echo -e "${YELLOW}[1/5] Installing dependencies...${NC}"
npm install --production

# Generate Prisma client
echo -e "${YELLOW}[2/5] Generating Prisma client...${NC}"
npx prisma generate

# Run migrations
echo -e "${YELLOW}[3/5] Running database migrations...${NC}"
npx prisma migrate deploy || echo -e "${YELLOW}[WARNING] Migrations may already be up to date${NC}"

# Create logs directory
echo -e "${YELLOW}[4/5] Creating directories...${NC}"
mkdir -p logs

# Stop existing PM2 process
pm2 delete dhad-studio 2>/dev/null || true

# Start with PM2
echo -e "${YELLOW}[5/5] Starting server with PM2...${NC}"
pm2 start ecosystem.config.js --env production
pm2 save

# Setup PM2 startup (auto-start on system boot)
pm2 startup 2>/dev/null || true

echo ""
echo "========================================"
echo -e "${GREEN}  Server started successfully!${NC}"
echo "========================================"
echo ""
echo "  Status:    pm2 status"
echo "  Logs:      pm2 logs dhad-studio"
echo "  Stop:      pm2 stop dhad-studio"
echo "  Restart:   pm2 restart dhad-studio"
echo "  Monitor:   pm2 monit"
echo ""
echo "  Health:    curl http://localhost:3000/health"
echo ""

pm2 status

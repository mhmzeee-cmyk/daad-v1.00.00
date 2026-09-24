# ============================================
# Dhad Studio - Docker Configuration
# Developer: محمد محمود الحموز | Dhad Studio
# ============================================

# Build stage
FROM node:20-alpine AS builder

WORKDIR /app

# Copy package files
COPY package*.json ./
COPY prisma ./prisma/

# Install dependencies
RUN npm ci --only=production

# Generate Prisma client
RUN npx prisma generate

# Production stage
FROM node:20-alpine AS production

# Create non-root user
RUN addgroup -g 1001 -S dhad && \
    adduser -S dhad -u 1001 -G dhad

WORKDIR /app

# Copy built files
COPY --from=builder /app/node_modules ./node_modules
COPY --from=builder /app/prisma ./prisma
COPY . .

# Generate Prisma client
RUN npx prisma generate

# Set ownership
RUN chown -R dhad:dhad /app

# Switch to non-root user
USER dhad

# Expose port
EXPOSE 3000

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
  CMD node -e "require('http').get('http://localhost:3000/health', (r) => { process.exit(r.statusCode === 200 ? 0 : 1); })"

# Start server
CMD ["node", "src/index.js"]

# ====================================================
# Dhad Studio - Multi-Stage Production Dockerfile
# ====================================================
# Build: docker build -t daad-studio-server .
# Run:   docker run -p 3000:3000 daad-studio-server
# ====================================================

# ── Stage 1: Dependencies ─────────────────────────────────────────────────────
FROM node:20-alpine AS deps

WORKDIR /app

# Copy package files first for better layer caching
COPY package.json package-lock.json ./
COPY prisma ./prisma/

# Install dependencies (including devDependencies for Prisma generate)
RUN npm ci --prefer-offline

# Generate Prisma Client
RUN npx prisma generate

# ── Stage 2: Production Build ─────────────────────────────────────────────────
FROM node:20-alpine AS builder

WORKDIR /app

# Copy dependencies from deps stage
COPY --from=deps /app/node_modules ./node_modules
COPY --from=deps /app/prisma ./prisma

# Copy application source
COPY . .

# Remove development files
RUN rm -rf scripts/ tests/ *.md .gitignore .dockerignore

# Prune to production dependencies only
RUN npm prune --production

# ── Stage 3: Production Image ─────────────────────────────────────────────────
FROM node:20-alpine AS production

# Security: Run as non-root user
RUN addgroup -g 1001 -S daadgroup && \
    adduser -S daaduser -u 1001 -G daadgroup

# Install dumb-init for proper signal handling
RUN apk add --no-cache dumb-init

# Set production environment
ENV NODE_ENV=production
ENV PORT=3000

WORKDIR /app

# Copy only necessary files from builder
COPY --from=builder --chown=daaduser:daadgroup /app/node_modules ./node_modules
COPY --from=builder --chown=daaduser:daadgroup /app/prisma ./prisma
COPY --from=builder --chown=daaduser:daadgroup /app/src ./src
COPY --from=builder --chown=daaduser:daadgroup /app/package.json ./

# Create logs directory
RUN mkdir -p /app/logs && chown -R daaduser:daadgroup /app/logs

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=40s --retries=3 \
  CMD wget --no-verbose --tries=1 --spider http://localhost:3000/health || exit 1

# Switch to non-root user
USER daaduser

# Expose port
EXPOSE 3000

# Use dumb-init to handle PID 1 responsibilities
ENTRYPOINT ["dumb-init", "--"]

# Start the server
CMD ["node", "src/index.js"]

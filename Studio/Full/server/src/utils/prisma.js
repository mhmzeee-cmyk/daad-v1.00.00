// Prisma Client singleton - one connection pool for the entire app
const { PrismaClient } = require("@prisma/client");
const { logger } = require("./logger");

const SLOW_QUERY_THRESHOLD_MS = 1000;

const prisma = new PrismaClient({
  log: process.env.NODE_ENV === "development"
    ? ["query", "error", "warn"]
    : [
        { level: "error", emit: "event" },
        { level: "warn", emit: "event" },
      ],
});

// Log slow queries in production
if (process.env.NODE_ENV !== "development") {
  prisma.$on("query", (e) => {
    if (e.duration > SLOW_QUERY_THRESHOLD_MS) {
      logger.warn("Slow query detected", {
        query: e.query?.substring(0, 200),
        duration: `${e.duration}ms`,
        params: e.params,
      });
    }
  });
}

// Enable SQLite WAL mode + performance pragmas (only for SQLite)
async function enableWalMode() {
  const url = process.env.DATABASE_URL || '';
  if (!url.startsWith('file:')) {
    // Not SQLite — skip WAL pragmas
    return;
  }
  try {
    // PRAGMA statements return result rows — must use $queryRawUnsafe, not $executeRawUnsafe
    await prisma.$queryRawUnsafe('PRAGMA journal_mode=WAL');
    await prisma.$queryRawUnsafe('PRAGMA busy_timeout=5000');
    await prisma.$queryRawUnsafe('PRAGMA cache_size=-64000');
    await prisma.$queryRawUnsafe('PRAGMA synchronous=NORMAL');
    logger.info('SQLite WAL mode + performance pragmas enabled');
  } catch (e) {
    logger.warn('Failed to enable WAL mode', { error: e.message });
  }
}

// Run on startup (non-blocking — errors are caught inside)
enableWalMode();

module.exports = prisma;
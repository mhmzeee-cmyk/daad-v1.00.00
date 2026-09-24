// ====================================================
// Dhad Studio - Redis Client & Cache Manager
// ====================================================
// Features: Connection pooling, Sorted Sets, TTL
// ====================================================

const Redis = require('ioredis');
const { logger } = require('./logger');

// ── Redis Configuration ──────────────────────────────────────────────────────
let redisAvailable = true;

const REDIS_CONFIG = {
  url: process.env.REDIS_URL || 'redis://localhost:6379',
  maxRetriesPerRequest: 3,
  retryStrategy(times) {
    if (times > 20) {
      redisAvailable = false;
      return null; // stop retrying after 20 attempts
    }
    return Math.min(times * 200, 5000);
  },
  enableReadyCheck: true,
  lazyConnect: true,
};

// ── Create Redis Client ──────────────────────────────────────────────────────
let redis = null;

const getRedisClient = () => {
  if (!redis) {
    redis = new Redis(REDIS_CONFIG.url, {
      maxRetriesPerRequest: REDIS_CONFIG.maxRetriesPerRequest,
      retryStrategy: REDIS_CONFIG.retryStrategy,
      lazyConnect: true,
    });

    redis.on('connect', () => {
      redisAvailable = true;
      // SECURITY: Never log Redis URL with credentials
      logger.info('✅ Redis connected');
    });

    redis.on('ready', () => {
      redisAvailable = true;
      logger.info('✅ Redis ready');
    });

    redis.on('error', (err) => {
      if (redisAvailable) {
        logger.error('❌ Redis error', { error: err.message });
      }
    });

    redis.on('close', () => {
      if (redisAvailable) {
        logger.warn('⚠️ Redis connection closed');
      }
    });
  }
  return redis;
};

// ── Leaderboard Cache Manager ────────────────────────────────────────────────
const LEADERBOARD_KEY = 'leaderboard:xp';
const LEADERBOARD_TTL = 300; // 5 minutes

const leaderboardCache = {
  /**
   * Get top N students from leaderboard
   */
  getTop: async (count = 50) => {
    const client = getRedisClient();
    try {
      // ZREVRANGE returns highest scores first
      const results = await client.zrevrange(LEADERBOARD_KEY, 0, count - 1, 'WITHSCORES');
      
      const leaderboard = [];
      for (let i = 0; i < results.length; i += 2) {
        leaderboard.push({
          studentId: results[i],
          totalXP: parseInt(results[i + 1], 10),
          rank: leaderboard.length + 1,
        });
      }
      
      return leaderboard;
    } catch (error) {
      logger.error('Redis leaderboard get failed', { error: error.message });
      return null;
    }
  },

  /**
   * Get a student's rank
   */
  getRank: async (studentId) => {
    const client = getRedisClient();
    try {
      // ZREVRANK returns 0-based rank (highest = 0)
      const rank = await client.zrevrank(LEADERBOARD_KEY, studentId);
      if (rank === null) return null;
      return rank + 1; // Convert to 1-based
    } catch (error) {
      logger.error('Redis rank get failed', { error: error.message });
      return null;
    }
  },

  /**
   * Get student's score
   */
  getScore: async (studentId) => {
    const client = getRedisClient();
    try {
      const score = await client.zscore(LEADERBOARD_KEY, studentId);
      return score ? parseInt(score, 10) : null;
    } catch (error) {
      logger.error('Redis score get failed', { error: error.message });
      return null;
    }
  },

  /**
   * Update a student's XP in the leaderboard
   */
  updateScore: async (studentId, xp) => {
    const client = getRedisClient();
    try {
      await client.zadd(LEADERBOARD_KEY, xp, studentId);
      logger.debug('Redis leaderboard updated', { studentId, xp });
      return true;
    } catch (error) {
      logger.error('Redis leaderboard update failed', { error: error.message });
      return false;
    }
  },

  /**
   * Increment student's XP
   */
  incrementScore: async (studentId, delta) => {
    const client = getRedisClient();
    try {
      const newScore = await client.zincrby(LEADERBOARD_KEY, delta, studentId);
      logger.debug('Redis leaderboard incremented', { studentId, delta, newScore });
      return parseInt(newScore, 10);
    } catch (error) {
      logger.error('Redis leaderboard increment failed', { error: error.message });
      return null;
    }
  },

  /**
   * Remove a student from leaderboard
   */
  remove: async (studentId) => {
    const client = getRedisClient();
    try {
      await client.zrem(LEADERBOARD_KEY, studentId);
      return true;
    } catch (error) {
      logger.error('Redis leaderboard remove failed', { error: error.message });
      return false;
    }
  },

  /**
   * Get total number of students in leaderboard
   */
  getCount: async () => {
    const client = getRedisClient();
    try {
      return await client.zcard(LEADERBOARD_KEY);
    } catch (error) {
      logger.error('Redis leaderboard count failed', { error: error.message });
      return 0;
    }
  },

  /**
   * Bulk update from database (for cache warming)
   */
  bulkUpdate: async (students) => {
    const client = getRedisClient();
    try {
      const pipeline = client.pipeline();
      students.forEach(({ studentId, totalXP }) => {
        pipeline.zadd(LEADERBOARD_KEY, totalXP, studentId);
      });
      await pipeline.exec();
      logger.info('Redis leaderboard bulk updated', { count: students.length });
      return true;
    } catch (error) {
      logger.error('Redis leaderboard bulk update failed', { error: error.message });
      return false;
    }
  },

  /**
   * Clear the leaderboard cache
   */
  clear: async () => {
    const client = getRedisClient();
    try {
      await client.del(LEADERBOARD_KEY);
      return true;
    } catch (error) {
      logger.error('Redis leaderboard clear failed', { error: error.message });
      return false;
    }
  },
};

// ── Failure Counter (for AI Feedback) ────────────────────────────────────────
const FAILURE_PREFIX = 'failures:';
const FAILURE_TTL = 86400; // 24 hours

const failureCounter = {
  /**
   * Increment failure count for a student+exercise
   */
  increment: async (studentId, exerciseId) => {
    const client = getRedisClient();
    const key = `${FAILURE_PREFIX}${studentId}:${exerciseId}`;
    try {
      const count = await client.incr(key);
      await client.expire(key, FAILURE_TTL);
      return count;
    } catch (error) {
      logger.error('Redis failure increment failed', { error: error.message });
      return 0;
    }
  },

  /**
   * Get current failure count
   */
  getCount: async (studentId, exerciseId) => {
    const client = getRedisClient();
    const key = `${FAILURE_PREFIX}${studentId}:${exerciseId}`;
    try {
      const count = await client.get(key);
      return count ? parseInt(count, 10) : 0;
    } catch (error) {
      logger.error('Redis failure get failed', { error: error.message });
      return 0;
    }
  },

  /**
   * Reset failure count (on success)
   */
  reset: async (studentId, exerciseId) => {
    const client = getRedisClient();
    const key = `${FAILURE_PREFIX}${studentId}:${exerciseId}`;
    try {
      await client.del(key);
      return true;
    } catch (error) {
      logger.error('Redis failure reset failed', { error: error.message });
      return false;
    }
  },
};

// ── General Cache Helpers ────────────────────────────────────────────────────
const cache = {
  /**
   * Get value with JSON parsing
   */
  get: async (key) => {
    const client = getRedisClient();
    try {
      const value = await client.get(key);
      return value ? JSON.parse(value) : null;
    } catch (error) {
      logger.error('Redis get failed', { key, error: error.message });
      return null;
    }
  },

  /**
   * Set value with TTL
   */
  set: async (key, value, ttlSeconds = 300) => {
    const client = getRedisClient();
    try {
      await client.setex(key, ttlSeconds, JSON.stringify(value));
      return true;
    } catch (error) {
      logger.error('Redis set failed', { key, error: error.message });
      return false;
    }
  },

  /**
   * Delete key
   */
  del: async (key) => {
    const client = getRedisClient();
    try {
      await client.del(key);
      return true;
    } catch (error) {
      logger.error('Redis del failed', { key, error: error.message });
      return false;
    }
  },

  /**
   * Check if key exists
   */
  exists: async (key) => {
    const client = getRedisClient();
    try {
      return (await client.exists(key)) === 1;
    } catch (error) {
      logger.error('Redis exists failed', { key, error: error.message });
      return false;
    }
  },
};

// ── Connection Management ────────────────────────────────────────────────────
const connectRedis = async () => {
  if (!process.env.REDIS_URL) {
    logger.info('ℹ️ Redis not configured, running without cache');
    redisAvailable = false;
    return false;
  }
  const client = getRedisClient();
  try {
    await client.connect();
    return true;
  } catch (error) {
    redisAvailable = false;
    logger.info('ℹ️ Redis unavailable, running without cache');
    return false;
  }
};

const disconnectRedis = async () => {
  if (redis) {
    try {
      await redis.quit();
    } catch (e) {
      // Ignore quit errors on already-broken connections
    }
    redis = null;
  }
};

const isRedisAvailable = () => redisAvailable;

// ── Auto-reconnect when Redis comes back ────────────────────────────────────
const _reconnectInterval = setInterval(() => {
  if (!redisAvailable && redis && process.env.REDIS_URL) {
    redis.connect().catch(() => {});
  }
}, 30000);
_reconnectInterval.unref();

module.exports = {
  getRedisClient,
  connectRedis,
  disconnectRedis,
  isRedisAvailable,
  leaderboardCache,
  failureCounter,
  cache,
};

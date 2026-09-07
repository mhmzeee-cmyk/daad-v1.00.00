// ====================================================
// Dhad Studio - Cache Middleware
// ====================================================
// تخزين مؤقت للبيانات المتكررة باستخدام Redis
// يقلل الضغط على قاعدة البيانات بنسبة 60-80%
// ====================================================

const { getRedisClient, isRedisAvailable } = require('../utils/redis');
const { logger } = require('../utils/logger');

// ── In-Memory Fallback Cache ────────────────────────────────────────────────
const MEMORY_CACHE_MAX = 5000;
const memoryCache = new Map();

// ── Cache Middleware Factory ────────────────────────────────────────────────
function createCacheMiddleware(options = {}) {
  const {
    ttl = 300,           // 5 minutes default
    keyPrefix = 'cache:',
    skipOnError = true,  // Continue without cache on error
    condition = null,    // Function to check if request should be cached
  } = options;

  return async (req, res, next) => {
    // Skip caching for non-GET requests
    if (req.method !== 'GET') {
      return next();
    }

    // Skip if condition not met
    if (condition && !condition(req)) {
      return next();
    }

    const cacheKey = `${keyPrefix}${req.originalUrl}`;
    const client = isRedisAvailable() ? getRedisClient() : null;

    try {
      let cachedData = null;

      if (client) {
        // Try Redis
        const data = await client.get(cacheKey);
        if (data) {
          cachedData = JSON.parse(data);
        }
      } else {
        // Fallback to memory
        const entry = memoryCache.get(cacheKey);
        if (entry && Date.now() - entry.timestamp < ttl * 1000) {
          cachedData = entry.data;
        } else {
          memoryCache.delete(cacheKey);
        }
      }

      if (cachedData) {
        res.setHeader('X-Cache', 'HIT');
        return res.json(cachedData);
      }

      // Cache miss - intercept response
      res.setHeader('X-Cache', 'MISS');
      
      const originalJson = res.json.bind(res);
      res.json = async (body) => {
        // Cache the response
        try {
          if (client) {
            await client.setex(cacheKey, ttl, JSON.stringify(body));
          } else {
            memoryCache.set(cacheKey, { data: body, timestamp: Date.now() });
            // Evict oldest entries if over limit
            if (memoryCache.size > MEMORY_CACHE_MAX) {
              const keysToDelete = Array.from(memoryCache.keys()).slice(0, Math.floor(MEMORY_CACHE_MAX / 4));
              keysToDelete.forEach(k => memoryCache.delete(k));
            }
          }
        } catch (error) {
          logger.error('Cache set failed', { key: cacheKey, error: error.message });
        }
        
        return originalJson(body);
      };

      next();
    } catch (error) {
      logger.error('Cache middleware error', { error: error.message });
      if (skipOnError) {
        return next();
      }
      next(error);
    }
  };
}

// ── Pre-configured Cache Instances ──────────────────────────────────────────
const caches = {
  // Leaderboard cache: 2 minutes
  leaderboard: createCacheMiddleware({
    ttl: 120,
    keyPrefix: 'cache:leaderboard:',
  }),

  // Student profile: 5 minutes
  studentProfile: createCacheMiddleware({
    ttl: 300,
    keyPrefix: 'cache:student:',
  }),

  // Classroom data: 5 minutes
  classroom: createCacheMiddleware({
    ttl: 300,
    keyPrefix: 'cache:classroom:',
  }),

  // Challenge list: 10 minutes
  challenges: createCacheMiddleware({
    ttl: 600,
    keyPrefix: 'cache:challenges:',
  }),

  // Course roadmap: 10 minutes
  roadmap: createCacheMiddleware({
    ttl: 600,
    keyPrefix: 'cache:roadmap:',
  }),

  // Course list: 15 minutes
  courses: createCacheMiddleware({
    ttl: 900,
    keyPrefix: 'cache:courses:',
  }),

  // School data: 30 minutes
  school: createCacheMiddleware({
    ttl: 1800,
    keyPrefix: 'cache:school:',
  }),
};

// ── Cache Invalidation Helpers ──────────────────────────────────────────────
const cacheInvalidator = {
  /**
   * Clear all cache for a specific pattern
   */
  clearPattern: async (pattern) => {
    if (!isRedisAvailable()) {
      // Clear memory cache
      for (const key of memoryCache.keys()) {
        if (key.includes(pattern)) {
          memoryCache.delete(key);
        }
      }
      return true;
    }

    try {
      const client = getRedisClient();
      const matchPattern = `cache:${pattern}*`;
      let cursor = '0';
      let totalDeleted = 0;
      
      do {
        const result = await client.scan(cursor, 'MATCH', matchPattern, 'COUNT', 100);
        cursor = result[0];
        const keys = result[1];
        if (keys.length > 0) {
          await client.del(...keys);
          totalDeleted += keys.length;
        }
      } while (cursor !== '0');
      
      if (totalDeleted > 0) {
        logger.info('Cache cleared', { pattern, count: totalDeleted });
      }
      return true;
    } catch (error) {
      logger.error('Cache clear failed', { pattern, error: error.message });
      return false;
    }
  },

  /**
   * Clear all cache
   */
  clearAll: async () => {
    if (!isRedisAvailable()) {
      memoryCache.clear();
      return true;
    }

    try {
      const client = getRedisClient();
      let cursor = '0';
      let totalDeleted = 0;
      
      do {
        const result = await client.scan(cursor, 'MATCH', 'cache:*', 'COUNT', 100);
        cursor = result[0];
        const keys = result[1];
        if (keys.length > 0) {
          await client.del(...keys);
          totalDeleted += keys.length;
        }
      } while (cursor !== '0');
      
      if (totalDeleted > 0) {
        logger.info('All cache cleared', { count: totalDeleted });
      }
      return true;
    } catch (error) {
      logger.error('Cache clear all failed', { error: error.message });
      return false;
    }
  },

  /**
   * Clear cache for specific student
   */
  clearStudent: async (studentId) => {
    return cacheInvalidator.clearPattern(`student:${studentId}`);
  },

  /**
   * Clear cache for specific classroom
   */
  clearClassroom: async (classroomId) => {
    return cacheInvalidator.clearPattern(`classroom:${classroomId}`);
  },
};

module.exports = {
  createCacheMiddleware,
  caches,
  cacheInvalidator,
};

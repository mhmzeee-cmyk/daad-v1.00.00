// ====================================================
// Dhad Studio - Redis Session Store
// ====================================================
// إدارة الجلسات باستخدام Redis بدلاً من الذاكرة
// يدعم 10,000+ جلسة متزامنة
// ====================================================

const { getRedisClient, isRedisAvailable } = require('../utils/redis');
const { logger } = require('../utils/logger');

const SESSION_PREFIX = 'session:';
const SESSION_TTL = 24 * 60 * 60; // 24 hours

// ── In-Memory Fallback ─────────────────────────────────────────────────────
const memorySessions = new Map();

// ── Session Store ───────────────────────────────────────────────────────────
const sessionStore = {
  /**
   * Create a new session
   */
  create: async (userId, sessionData) => {
    const sessionId = generateSessionId();
    const key = `${SESSION_PREFIX}${sessionId}`;

    try {
      if (isRedisAvailable()) {
        const client = getRedisClient();
        await client.setex(key, SESSION_TTL, JSON.stringify({
          userId,
          ...sessionData,
          createdAt: Date.now(),
        }));

        // Track user's sessions
        const userKey = `${SESSION_PREFIX}user:${userId}`;
        await client.sadd(userKey, sessionId);
        await client.expire(userKey, SESSION_TTL);
      } else {
        memorySessions.set(sessionId, {
          userId,
          ...sessionData,
          createdAt: Date.now(),
        });
      }

      logger.info('Session created', { sessionId, userId });
      return sessionId;
    } catch (error) {
      logger.error('Session create failed', { error: error.message });
      return null;
    }
  },

  /**
   * Get session data
   */
  get: async (sessionId) => {
    const key = `${SESSION_PREFIX}${sessionId}`;

    try {
      if (isRedisAvailable()) {
        const client = getRedisClient();
        const data = await client.get(key);
        if (!data) return null;

        const session = JSON.parse(data);
        
        // Extend TTL on access
        await client.expire(key, SESSION_TTL);
        
        return session;
      } else {
        return memorySessions.get(sessionId) || null;
      }
    } catch (error) {
      logger.error('Session get failed', { error: error.message });
      return null;
    }
  },

  /**
   * Update session data
   */
  update: async (sessionId, updates) => {
    const key = `${SESSION_PREFIX}${sessionId}`;

    try {
      if (isRedisAvailable()) {
        const client = getRedisClient();
        const data = await client.get(key);
        if (!data) return false;

        const session = { ...JSON.parse(data), ...updates };
        await client.setex(key, SESSION_TTL, JSON.stringify(session));
      } else {
        const session = memorySessions.get(sessionId);
        if (!session) return false;
        memorySessions.set(sessionId, { ...session, ...updates });
      }

      return true;
    } catch (error) {
      logger.error('Session update failed', { error: error.message });
      return false;
    }
  },

  /**
   * Delete a session
   */
  delete: async (sessionId) => {
    const key = `${SESSION_PREFIX}${sessionId}`;

    try {
      if (isRedisAvailable()) {
        const client = getRedisClient();
        
        // Get session to find userId
        const data = await client.get(key);
        if (data) {
          const session = JSON.parse(data);
          const userKey = `${SESSION_PREFIX}user:${session.userId}`;
          await client.srem(userKey, sessionId);
        }
        
        await client.del(key);
      } else {
        memorySessions.delete(sessionId);
      }

      return true;
    } catch (error) {
      logger.error('Session delete failed', { error: error.message });
      return false;
    }
  },

  /**
   * Get all sessions for a user
   */
  getUserSessions: async (userId) => {
    const userKey = `${SESSION_PREFIX}user:${userId}`;

    try {
      if (isRedisAvailable()) {
        const client = getRedisClient();
        const sessionIds = await client.smembers(userKey);
        
        const sessions = [];
        for (const sessionId of sessionIds) {
          const session = await sessionStore.get(sessionId);
          if (session) {
            sessions.push({ sessionId, ...session });
          } else {
            // Clean up stale reference
            await client.srem(userKey, sessionId);
          }
        }
        
        return sessions;
      } else {
        const sessions = [];
        for (const [id, data] of memorySessions.entries()) {
          if (data.userId === userId) {
            sessions.push({ sessionId: id, ...data });
          }
        }
        return sessions;
      }
    } catch (error) {
      logger.error('Session getUserSessions failed', { error: error.message });
      return [];
    }
  },

  /**
   * Enforce max sessions per user
   */
  enforceMaxSessions: async (userId, maxSessions = 3) => {
    try {
      const sessions = await sessionStore.getUserSessions(userId);
      
      if (sessions.length >= maxSessions) {
        // Sort by creation date, keep newest
        sessions.sort((a, b) => (b.createdAt || 0) - (a.createdAt || 0));
        
        // Delete oldest sessions
        const toDelete = sessions.slice(maxSessions - 1);
        for (const session of toDelete) {
          await sessionStore.delete(session.sessionId);
          logger.info('Session deleted (max enforced)', { sessionId: session.sessionId, userId });
        }
      }
      
      return true;
    } catch (error) {
      logger.error('Session enforceMaxSessions failed', { error: error.message });
      return false;
    }
  },

  /**
   * Clear all sessions (for logout everywhere)
   */
  clearAll: async (userId) => {
    const userKey = `${SESSION_PREFIX}user:${userId}`;

    try {
      if (isRedisAvailable()) {
        const client = getRedisClient();
        const sessionIds = await client.smembers(userKey);
        
        for (const sessionId of sessionIds) {
          await client.del(`${SESSION_PREFIX}${sessionId}`);
        }
        
        await client.del(userKey);
      } else {
        for (const [id, data] of memorySessions.entries()) {
          if (data.userId === userId) {
            memorySessions.delete(id);
          }
        }
      }

      return true;
    } catch (error) {
      logger.error('Session clearAll failed', { error: error.message });
      return false;
    }
  },
};

// ── In-Memory Cleanup (prevents unbounded growth without Redis) ──────────────
const MEMORY_SESSION_MAX = 10000;
const MEMORY_SESSION_TTL = 24 * 60 * 60 * 1000; // 24h

const _cleanupInterval = setInterval(() => {
  if (isRedisAvailable()) return; // Redis handles its own expiry

  const now = Date.now();

  // Evict expired sessions
  for (const [id, data] of memorySessions.entries()) {
    if (now - (data.createdAt || 0) > MEMORY_SESSION_TTL) {
      memorySessions.delete(id);
    }
  }

  // Hard cap: if still over limit, evict oldest 10%
  if (memorySessions.size > MEMORY_SESSION_MAX) {
    const entries = [...memorySessions.entries()];
    const sorted = entries.sort((a, b) => (a[1].createdAt || 0) - (b[1].createdAt || 0));
    const evictCount = Math.floor(sorted.length * 0.1);
    for (let i = 0; i < evictCount; i++) {
      memorySessions.delete(sorted[i][0]);
    }
  }
}, 5 * 60 * 1000); // Every 5 minutes
_cleanupInterval.unref();

// ── Helpers ─────────────────────────────────────────────────────────────────
function generateSessionId() {
  return require('crypto').randomBytes(32).toString('hex');
}

module.exports = sessionStore;

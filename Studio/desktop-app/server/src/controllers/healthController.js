// ====================================================
// Dhad Studio - Health Monitoring Endpoint
// ====================================================
// Features: DB ping, WebSocket status, system metrics
// NOTE: Detailed health check requires admin authentication
// ====================================================

const os = require('os');
const prisma = require("../utils/prisma");

// Package version for health reporting
let pkgVersion = 'unknown';
try {
  pkgVersion = require('../../package.json').version || 'unknown';
} catch (_) { /* ignore */ }

// Store WebSocket server reference
let wsServer = null;

/**
 * Set WebSocket server reference for health checks
 */
const setWebSocketServer = (ws) => {
  wsServer = ws;
};

/**
 * GET /health - System Health Check
 * Returns minimal info by default
 * Returns detailed info only when X-Health-Key header matches HEALTH_SECRET
 */
const healthCheck = async (req, res) => {
  // Security: Only return detailed info with valid health key
  const healthKey = req.headers['x-health-key'];
  const isDetailed = healthKey && process.env.HEALTH_SECRET && healthKey === process.env.HEALTH_SECRET;

  if (!isDetailed) {
    // Basic response: minimal info safe for load balancers
    return res.status(200).json({
      status: 'healthy',
      timestamp: new Date().toISOString(),
      uptime: process.uptime(),
    });
  }

  const startTime = Date.now();
  const health = {
    status: 'healthy',
    timestamp: new Date().toISOString(),
    uptime: process.uptime(),
  };

  try {
    // ── Database Health ─────────────────────────────────────────────────────
    const dbStart = Date.now();
    try {
      await prisma.$queryRaw`SELECT 1`;
      health.database = {
        status: 'connected',
        latency: `${Date.now() - dbStart}ms`,
      };
    } catch (dbError) {
      health.database = {
        status: 'disconnected',
        latency: `${Date.now() - dbStart}ms`,
      };
      health.status = 'degraded';
    }

    // ── WebSocket Health ────────────────────────────────────────────────────
    if (wsServer) {
      health.websocket = {
        status: 'active',
        connections: wsServer.clients?.size || 0,
      };
    } else {
      health.websocket = {
        status: 'not_initialized',
        connections: 0,
      };
    }

    // ── Memory Usage (sanitized - no exact numbers) ─────────────────────────
    const memUsage = process.memoryUsage();
    const memPercentage = Math.round((memUsage.heapUsed / memUsage.heapTotal) * 100);
    health.memory = {
      status: memPercentage > 90 ? 'critical' : memPercentage > 70 ? 'warning' : 'ok',
      percentage: `${memPercentage}%`,
    };

    // ── Response Time ───────────────────────────────────────────────────────
    health.responseTime = `${Date.now() - startTime}ms`;

    // Determine overall status
    if (health.database.status === 'disconnected') {
      health.status = 'degraded';
      res.status(503);
    } else {
      res.status(200);
    }

    res.json(health);

  } catch (error) {
    health.status = 'unhealthy';
    health.responseTime = `${Date.now() - startTime}ms`;
    res.status(503).json(health);
  }
};

/**
 * GET /health/ready - Readiness Check
 * Simple check for load balancers
 */
const readinessCheck = async (req, res) => {
  try {
    await prisma.$queryRaw`SELECT 1`;
    res.status(200).json({ status: 'ready' });
  } catch (error) {
    res.status(503).json({ status: 'not ready', error: 'Service dependencies not available' });
  }
};

/**
 * GET /health/live - Liveness Check
 * Simple check for container orchestration
 */
const livenessCheck = (req, res) => {
  res.status(200).json({ status: 'alive' });
};

module.exports = {
  healthCheck,
  readinessCheck,
  livenessCheck,
  setWebSocketServer,
};

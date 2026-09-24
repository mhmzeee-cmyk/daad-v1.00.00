/* Dhad Studio - Cluster Mode Entry Point */
// Forks one worker per CPU core. Each worker runs src/index.js
// H8: Without Redis, run single worker to prevent shared-nothing rate limit bypass

const cluster = require('cluster');
const os = require('os');

let logger;
try { logger = require('./utils/logger').logger; } catch (_) { logger = console; }

// H8: Without Redis, all workers have independent in-memory stores
// (N workers × limit = N× actual limit). Force single worker.
const hasRedis = !!process.env.REDIS_URL;
const NUM_WORKERS = process.env.WEB_CONCURRENCY || (hasRedis ? os.cpus().length : 1);

if (!hasRedis && os.cpus().length > 1) {
  logger.warn(`[CLUSTER] No REDIS_URL — forcing single worker (hasRedis=${hasRedis})`);
}

if (cluster.isPrimary) {
  logger.info(`[CLUSTER] Primary ${process.pid} — starting ${NUM_WORKERS} worker(s)`);

  for (let i = 0; i < NUM_WORKERS; i++) {
    cluster.fork();
  }

  let ready = 0;
  cluster.on('message', (worker, msg) => {
    if (msg === 'ready') {
      ready++;
      if (ready === NUM_WORKERS) {
        logger.info(`[CLUSTER] All ${NUM_WORKERS} workers ready.`);
      }
    }
  });

  cluster.on('exit', (worker, code, signal) => {
    logger.error(`[CLUSTER] Worker ${worker.process.pid} died (${signal || code}). Restarting...`);
    cluster.fork();
  });

  process.on('SIGTERM', () => {
    for (const id in cluster.workers) cluster.workers[id].process.kill('SIGTERM');
  });
  process.on('SIGINT', () => {
    for (const id in cluster.workers) cluster.workers[id].process.kill('SIGINT');
  });

} else {
  // Signal to index.js that it should start the server
  process.env.DHAD_START_SERVER = '1';
  require('./index');
}

/* Dhad Studio - Cluster Mode Entry Point */
// Forks one worker per CPU core. Each worker runs src/index.js

const cluster = require('cluster');
const os = require('os');

let logger;
try { logger = require('./utils/logger').logger; } catch (_) { logger = console; }

const NUM_WORKERS = process.env.WEB_CONCURRENCY || os.cpus().length;

if (cluster.isPrimary) {
  logger.info(`[CLUSTER] Primary ${process.pid} — starting ${NUM_WORKERS} workers`);

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

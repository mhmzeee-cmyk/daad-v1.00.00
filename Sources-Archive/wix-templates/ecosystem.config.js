/**
 * Dhad Studio — PM2 process definition (minimal).
 * Used by: npm run prod / prod:stop / prod:restart / prod:logs / prod:status
 * Notes:
 *  - Plain entry point (src/index.js); use src/cluster.js only if multi-core
 *    without pm2 is desired (see package.json: start:cluster).
 *  - All runtime values come from environment (.env). Nothing secret here.
 *  - NODE_ENV=production is enforced for the prod env; never run prod
 *    without DATABASE_URL, JWT_SECRET, HMAC_SECRET, ALLOWED_ORIGINS.
 */
module.exports = {
  apps: [
    {
      name: 'dhad-studio',
      script: './src/index.js',
      instances: 1,
      exec_mode: 'fork',
      watch: false,
      max_memory_restart: '512M',
      env_production: {
        NODE_ENV: 'production',
      },
    },
  ],
};

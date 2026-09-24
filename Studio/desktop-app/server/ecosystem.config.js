module.exports = {
  apps: [
    {
      name: 'dhad-studio',
      script: 'src/index.js',
      cwd: __dirname,
      instances: 1, // Single instance — SQLite does not support concurrent writers. Use 'max' + exec_mode: 'cluster' ONLY with PostgreSQL.
      exec_mode: 'fork',
      autorestart: true,
      watch: false,
      max_memory_restart: '512M',
      env: {
        NODE_ENV: 'production',
        PORT: 3000,
      },
      env_development: {
        NODE_ENV: 'development',
        PORT: 3000,
      },
      // Logging
      error_file: 'logs/error.log',
      out_file: 'logs/out.log',
      log_date_format: 'YYYY-MM-DD HH:mm:ss Z',
      merge_logs: true,
      // Restart strategies
      restart_delay: 3000,
      max_restarts: 10,
      min_uptime: '10s',
      // Graceful shutdown
      kill_timeout: 5000,
      listen_timeout: 10000,
      // Cluster mode
      node_args: '--max-old-space-size=512',
    },
  ],
};

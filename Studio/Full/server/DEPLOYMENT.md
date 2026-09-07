# ============================================
# Dhad Studio - Production Deployment Guide
# Developer: محمد محمود الحموز | Dhad Studio
# ============================================

## Quick Start (Local Production)

```bash
# 1. Install dependencies
npm install

# 2. Setup database
npx prisma generate
npx prisma migrate deploy

# 3. Start with PM2
npm run prod

# 4. Check status
npm run prod:status
```

## Docker Deployment

```bash
# 1. Build and start
docker-compose up -d

# 2. Check logs
docker logs dhad-server

# 3. Stop
docker-compose down
```

## Cloud Deployment Options

### Option 1: Render (Recommended for beginners)
1. Connect GitHub repository
2. Set environment variables
3. Deploy automatically

### Option 2: Railway
1. Connect GitHub repository
2. Add PostgreSQL and Redis
3. Set environment variables

### Option 3: DigitalOcean App Platform
1. Connect GitHub repository
2. Configure build settings
3. Add managed databases

### Option 4: AWS/GCP/Azure
1. Use Docker deployment
2. Setup managed PostgreSQL and Redis
3. Configure load balancer

## Environment Variables

```env
# Required
DATABASE_URL=postgresql://user:password@host:5432/dbname
JWT_SECRET=your-secret-key-min-32-chars
JWT_REFRESH_SECRET=another-secret-key-min-32-chars
HMAC_SECRET=your-hmac-secret

# Optional
REDIS_URL=redis://host:6379
PORT=3000
NODE_ENV=production
ALLOWED_ORIGINS=https://yourdomain.com
```

## Production Checklist

- [ ] PostgreSQL database configured
- [ ] Redis configured (optional but recommended)
- [ ] Environment variables set
- [ ] SSL/TLS configured
- [ ] Domain name configured
- [ ] Backup strategy in place
- [ ] Monitoring configured
- [ ] Logs aggregation setup
- [ ] CDN for static assets

## Performance Tuning

### Node.js
- Use Node.js 20+ LTS
- Set `--max-old-space-size=512`
- Enable cluster mode with PM2

### PostgreSQL
- Connection pooling (pgBouncer)
- Index optimization
- Regular vacuuming

### Redis
- Set maxmemory policy
- Monitor memory usage
- Use connection pooling

### Nginx
- Enable gzip/brotli
- Cache static assets
- Rate limiting
- Load balancing

## Monitoring

### Health Check
```bash
curl http://localhost:3000/health
```

### PM2 Monitoring
```bash
pm2 monit
pm2 logs dhad-studio
```

### Docker Monitoring
```bash
docker stats
docker logs -f dhad-server
```

## Troubleshooting

### Server won't start
1. Check Node.js version: `node -v`
2. Check dependencies: `npm install`
3. Check database: `npx prisma db push`
4. Check logs: `pm2 logs`

### High memory usage
1. Check for memory leaks: `node --inspect`
2. Restart PM2: `pm2 restart dhad-studio`
3. Increase memory limit: `--max-old-space-size=1024`

### Database connection issues
1. Check DATABASE_URL
2. Check PostgreSQL is running
3. Check firewall rules
4. Check connection pooling

## Scaling

### Horizontal Scaling
- Use PM2 cluster mode
- Add load balancer
- Use session store (Redis)

### Vertical Scaling
- Increase CPU cores
- Increase memory
- Optimize queries

## Security

- Keep dependencies updated
- Use strong secrets
- Enable rate limiting
- Monitor for attacks
- Regular security audits

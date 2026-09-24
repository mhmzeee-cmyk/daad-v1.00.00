# 🚀 Dhad Studio - AWS Production Deployment Guide

> **Version:** 1.0.0 (Batch 12)  
> **Last Updated:** January 2025  
> **Infrastructure:** AWS ECS Fargate + RDS PostgreSQL + ElastiCache Redis

---

## 📋 Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Prerequisites](#prerequisites)
3. [AWS Infrastructure Setup](#aws-infrastructure-setup)
4. [Secrets Management](#secrets-management)
5. [Docker Build & ECR Push](#docker-build--ecr-push)
6. [ECS Fargate Deployment](#ecs-fargate-deployment)
7. [Database Migration (RDS)](#database-migration-rds)
8. [CI/CD Pipeline](#cicd-pipeline)
9. [Health Checks & Auto-Scaling](#health-checks--auto-scaling)
10. [Monitoring & Logging](#monitoring--logging)
11. [Troubleshooting](#troubleshooting)

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              AWS CLOUD                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│    ┌──────────────┐      ┌──────────────┐      ┌──────────────┐            │
│    │   Route 53   │─────▶│  CloudFront  │─────▶│   S3 (React) │            │
│    │    (DNS)     │      │    (CDN)     │      │   Frontend   │            │
│    └──────────────┘      └──────┬───────┘      └──────────────┘            │
│                                 │                                           │
│                                 ▼                                           │
│                        ┌──────────────┐                                     │
│                        │     ALB      │                                     │
│                        │  (HTTPS:443) │                                     │
│                        └──────┬───────┘                                     │
│                               │                                             │
│              ┌────────────────┼────────────────┐                            │
│              ▼                                 ▼                            │
│    ┌──────────────────┐             ┌──────────────────┐                   │
│    │   ECS Fargate    │             │   ECS Fargate    │                   │
│    │   Task (daad-api)│             │   Task (daad-api)│                   │
│    │   Port: 3000     │             │   Port: 3000     │                   │
│    └────────┬─────────┘             └────────┬─────────┘                   │
│             │                                 │                             │
│             └─────────────┬───────────────────┘                             │
│                           │                                                 │
│         ┌─────────────────┼─────────────────┐                              │
│         ▼                 ▼                 ▼                              │
│  ┌────────────┐   ┌────────────┐   ┌────────────────┐                     │
│  │    RDS     │   │ ElastiCache│   │    Secrets     │                     │
│  │ PostgreSQL │   │   Redis    │   │    Manager     │                     │
│  │  (db.t3)   │   │  (cache)   │   │  (credentials) │                     │
│  └────────────┘   └────────────┘   └────────────────┘                     │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Prerequisites

### AWS Account Setup
```bash
# Configure AWS CLI
aws configure
# AWS Access Key ID: AKIA...
# AWS Secret Access Key: ...
# Default region name: me-south-1
# Default output format: json
```

### Required Tools
| Tool | Version | Purpose |
|------|---------|---------|
| AWS CLI | >= 2.x | AWS operations |
| Docker | >= 24.x | Container build |
| Node.js | >= 18.x | Local development |
| Prisma | >= 5.x | Database migrations |

---

## AWS Infrastructure Setup

### 1. VPC & Networking

Create a dedicated VPC for Dhad Studio:

```bash
# Create VPC
VPC_ID=$(aws ec2 create-vpc --cidr-block 10.0.0.0/16 \
  --query 'Vpc.VpcId' --output text)

# Create public subnets (for ALB)
PUBLIC_SUBNET_1=$(aws ec2 create-subnet --vpc-id $VPC_ID \
  --cidr-block 10.0.1.0/24 --availability-zone me-south-1a \
  --query 'Subnet.SubnetId' --output text)

PUBLIC_SUBNET_2=$(aws ec2 create-subnet --vpc-id $VPC_ID \
  --cidr-block 10.0.2.0/24 --availability-zone me-south-1b \
  --query 'Subnet.SubnetId' --output text)

# Create private subnets (for ECS & RDS)
PRIVATE_SUBNET_1=$(aws ec2 create-subnet --vpc-id $VPC_ID \
  --cidr-block 10.0.3.0/24 --availability-zone me-south-1a \
  --query 'Subnet.SubnetId' --output text)

PRIVATE_SUBNET_2=$(aws ec2 create-subnet --vpc-id $VPC_ID \
  --cidr-block 10.0.4.0/24 --availability-zone me-south-1b \
  --query 'Subnet.SubnetId' --output text)
```

### 2. Security Groups

```bash
# ALB Security Group (allows 80/443 from internet)
ALB_SG=$(aws ec2 create-security-group \
  --group-name daad-studio-alb-sg \
  --description "ALB Security Group" \
  --vpc-id $VPC_ID \
  --query 'GroupId' --output text)

aws ec2 authorize-security-group-ingress --group-id $ALB_SG --protocol tcp --port 80 --cidr 0.0.0.0/0
aws ec2 authorize-security-group-ingress --group-id $ALB_SG --protocol tcp --port 443 --cidr 0.0.0.0/0

# ECS Security Group (allows only from ALB)
ECS_SG=$(aws ec2 create-security-group \
  --group-name daad-studio-ecs-sg \
  --description "ECS Security Group" \
  --vpc-id $VPC_ID \
  --query 'GroupId' --output text)

aws ec2 authorize-security-group-ingress --group-id $ECS_SG --protocol tcp --port 3000 --source-group $ALB_SG

# RDS Security Group (allows only from ECS)
RDS_SG=$(aws ec2 create-security-group \
  --group-name daad-studio-rds-sg \
  --description "RDS Security Group" \
  --vpc-id $VPC_ID \
  --query 'GroupId' --output text)

aws ec2 authorize-security-group-ingress --group-id $RDS_SG --protocol tcp --port 5432 --source-group $ECS_SG

# Redis Security Group (allows only from ECS)
REDIS_SG=$(aws ec2 create-security-group \
  --group-name daad-studio-redis-sg \
  --description "Redis Security Group" \
  --vpc-id $VPC_ID \
  --query 'GroupId' --output text)

aws ec2 authorize-security-group-ingress --group-id $REDIS_SG --protocol tcp --port 6379 --source-group $ECS_SG
```

### 3. RDS PostgreSQL

```bash
# Create DB Subnet Group
aws rds create-db-subnet-group \
  --db-subnet-group-name daad-studio-db-subnet \
  --db-subnet-group-description "Dhad Studio RDS Subnets" \
  --subnet-ids $PRIVATE_SUBNET_1 $PRIVATE_SUBNET_2

# Create RDS Instance
aws rds create-db-instance \
  --db-instance-identifier daad-studio-db \
  --db-instance-class db.t3.medium \
  --engine postgres \
  --engine-version 15.4 \
  --master-username daad_admin \
  --master-user-password "$(openssl rand -base64 24)" \
  --allocated-storage 20 \
  --max-allocated-storage 100 \
  --storage-type gp3 \
  --storage-encrypted \
  --multi-az \
  --publicly-accessible false \
  --vpc-security-group-ids $RDS_SG \
  --db-subnet-group-name daad-studio-db-subnet \
  --backup-retention-period 7 \
  --preferred-backup-window 03:00-04:00 \
  --tags Key=Project,Value=DhadStudio
```

### 4. ElastiCache Redis

```bash
# Create Cache Subnet Group
aws elasticache create-cache-subnet-group \
  --cache-subnet-group-name daad-studio-redis-subnet \
  --cache-subnet-group-description "Dhad Studio Redis Subnets" \
  --subnet-ids $PRIVATE_SUBNET_1 $PRIVATE_SUBNET_2

# Create Redis Replication Group
aws elasticache create-replication-group \
  --replication-group-id daad-studio-redis \
  --replication-group-description "Dhad Studio Redis Cache" \
  --engine redis \
  --engine-version 7.0 \
  --cache-node-type cache.t3.micro \
  --num-cache-clusters 2 \
  --automatic-failover-enabled \
  --at-rest-encryption-enabled \
  --transit-encryption-enabled \
  --cache-subnet-group-name daad-studio-redis-subnet \
  --security-group-ids $REDIS_SG
```

---

## Secrets Management

All sensitive credentials are stored in AWS Secrets Manager and injected at runtime.

### Create Secrets

```bash
# DATABASE_URL (RDS connection with SSL)
aws secretsmanager create-secret \
  --name daad-studio/production/database-url \
  --description "PostgreSQL connection string for RDS" \
  --secret-string "postgresql://daad_admin:PASSWORD@daad-studio-db.xxxxx.me-south-1.rds.amazonaws.com:5432/daad_studio?sslmode=require"

# JWT_SECRET
aws secretsmanager create-secret \
  --name daad-studio/production/jwt-secret \
  --description "JWT signing secret" \
  --secret-string "$(openssl rand -base64 32)"

# GEMINI_API_KEY
aws secretsmanager create-secret \
  --name daad-studio/production/gemini-api-key \
  --description "Google Gemini API key for AI feedback" \
  --secret-string "YOUR_GEMINI_API_KEY"

# REDIS_URL
aws secretsmanager create-secret \
  --name daad-studio/production/redis-url \
  --description "ElastiCache Redis connection" \
  --secret-string "redis://daad-studio-redis.xxxxx.cache.amazonaws.com:6379"

# WEBHOOK_DISCORD_URL
aws secretsmanager create-secret \
  --name daad-studio/production/webhook-discord \
  --description "Discord webhook for security alerts" \
  --secret-string "https://discord.com/api/webhooks/YOUR_WEBHOOK"
```

### Secrets Reference Table

| Secret Name | Environment Variable | Description |
|-------------|---------------------|-------------|
| `daad-studio/production/database-url` | `DATABASE_URL` | PostgreSQL connection with SSL |
| `daad-studio/production/jwt-secret` | `JWT_SECRET` | JWT signing key |
| `daad-studio/production/gemini-api-key` | `GEMINI_API_KEY` | Google Gemini AI key |
| `daad-studio/production/redis-url` | `REDIS_URL` | ElastiCache Redis URL |
| `daad-studio/production/webhook-discord` | `WEBHOOK_DISCORD_URL` | Security alert webhook |

---

## Docker Build & ECR Push

### 1. Create ECR Repository

```bash
aws ecr create-repository \
  --repository-name daad-studio \
  --image-scanning-configuration scanOnPush=true \
  --encryption-configuration encryptionType=AES256
```

### 2. Build Docker Image

Our `Dockerfile` (Batch 10) uses multi-stage build for optimal security:

```bash
# Navigate to server directory
cd server

# Build the image
docker build -t daad-studio:latest ..

# Verify the image
docker images daad-studio
```

**Dockerfile Stages:**
| Stage | Purpose | Size Reduction |
|-------|---------|----------------|
| `deps` | Install dependencies + Prisma generate | - |
| `builder` | Copy source, prune to production | ~40% |
| `production` | Final minimal image with dumb-init | ~85% total |

### 3. Push to ECR

```bash
# Get ECR login token
aws ecr get-login-password --region me-south-1 | \
  docker login --username AWS --password-stdin ACCOUNT_ID.dkr.ecr.me-south-1.amazonaws.com

# Tag the image
docker tag daad-studio:latest \
  ACCOUNT_ID.dkr.ecr.me-south-1.amazonaws.com/daad-studio:latest

# Push to ECR
docker push ACCOUNT_ID.dkr.ecr.me-south-1.amazonaws.com/daad-studio:latest
```

---

## ECS Fargate Deployment

### 1. Create ECS Cluster

```bash
aws ecs create-cluster \
  --cluster-name daad-studio-cluster \
  --capacity-providers FARGATE FARGATE_SPOT \
  --default-capacity-provider-strategy \
    capacityProvider=FARGATE,weight=1 \
    capacityProvider=FARGATE_SPOT,weight=3
```

### 2. Register Task Definition

Update `aws/task-definition.json` with your AWS Account ID, then:

```bash
aws ecs register-task-definition --cli-input-json file://aws/task-definition.json
```

**Task Definition Key Settings:**
- **CPU:** 512 units (0.5 vCPU)
- **Memory:** 1024 MB
- **Network Mode:** awsvpc (required for Fargate)
- **Logs:** CloudWatch Logs at `/ecs/daad-studio-api`

### 3. Create Target Group

```bash
aws elbv2 create-target-group \
  --name daad-studio-tg \
  --protocol HTTP \
  --port 3000 \
  --vpc-id $VPC_ID \
  --target-type ip \
  --health-check-protocol HTTP \
  --health-check-path /health \
  --health-check-interval-seconds 30 \
  --health-check-timeout-seconds 10 \
  --healthy-threshold-count 3 \
  --unhealthy-threshold-count 3
```

### 4. Create ALB & Listeners

```bash
# Create ALB
ALB_ARN=$(aws elbv2 create-load-balancer \
  --name daad-studio-api-alb \
  --subnets $PUBLIC_SUBNET_1 $PUBLIC_SUBNET_2 \
  --security-groups $ALB_SG \
  --scheme internet-facing \
  --type application \
  --query 'LoadBalancers[0].LoadBalancerArn' --output text)

# Create HTTPS Listener (requires ACM certificate)
aws elbv2 create-listener \
  --load-balancer-arn $ALB_ARN \
  --protocol HTTPS \
  --port 443 \
  --certificates CertificateArn=arn:aws:acm:me-south-1:ACCOUNT_ID:certificate/CERT_ID \
  --default-actions Type=forward,TargetGroupArn=arn:aws:elasticloadbalancing:me-south-1:ACCOUNT_ID:targetgroup/daad-studio-tg/xxxxx

# Create HTTP Listener (redirect to HTTPS)
aws elbv2 create-listener \
  --load-balancer-arn $ALB_ARN \
  --protocol HTTP \
  --port 80 \
  --default-actions Type=redirect,RedirectConfig={Protocol=HTTPS,Port=443,StatusCode=HTTP_301}
```

### 5. Create ECS Service

```bash
aws ecs create-service \
  --cluster daad-studio-cluster \
  --service-name daad-studio-api-service \
  --task-definition daad-studio-api \
  --desired-count 2 \
  --launch-type FARGATE \
  --network-configuration "awsvpcConfiguration={
    subnets=[$PRIVATE_SUBNET_1,$PRIVATE_SUBNET_2],
    securityGroups=[$ECS_SG],
    assignPublicIp=DISABLED
  }" \
  --load-balancers "targetGroupArn=arn:aws:elasticloadbalancing:me-south-1:ACCOUNT_ID:targetgroup/daad-studio-tg/xxxxx,containerName=daad-api,containerPort=3000" \
  --deployment-configuration "maximumPercent=200,minimumHealthyPercent=100"
```

---

## Database Migration (RDS)

### Method 1: One-Time ECS Task (Recommended)

```bash
# Run Prisma Migrate as a one-off task
aws ecs run-task \
  --cluster daad-studio-cluster \
  --task-definition daad-studio-api \
  --launch-type FARGATE \
  --network-configuration "awsvpcConfiguration={
    subnets=[$PRIVATE_SUBNET_1],
    securityGroups=[$ECS_SG],
    assignPublicIp=DISABLED
  }" \
  --overrides '{
    "containerOverrides": [{
      "name": "daad-api",
      "command": ["npx", "prisma", "migrate", "deploy"],
      "environment": [
        {"name": "NODE_ENV", "value": "production"}
      ]
    }]
  }'
```

### Method 2: AWS CLI with ECS Exec

```bash
# Get task ID
TASK_ID=$(aws ecs list-tasks --cluster daad-studio-cluster --query 'taskArns[0]' --output text)

# Execute command in running container
aws ecs execute-command \
  --cluster daad-studio-cluster \
  --task $TASK_ID \
  --container daad-api \
  --interactive \
  --command "/bin/sh"

# Inside container:
npx prisma migrate deploy
exit
```

### Seed Initial Data

```bash
aws ecs run-task \
  --cluster daad-studio-cluster \
  --task-definition daad-studio-api \
  --launch-type FARGATE \
  --network-configuration "awsvpcConfiguration={
    subnets=[$PRIVATE_SUBNET_1],
    securityGroups=[$ECS_SG],
    assignPublicIp=DISABLED
  }" \
  --overrides '{
    "containerOverrides": [{
      "name": "daad-api",
      "command": ["node", "prisma/seed.js"]
    }]
  }'
```

---

## CI/CD Pipeline

### AWS CodeBuild Configuration

Our `aws/buildspec.yml` automates the build-deploy pipeline:

```yaml
# Simplified workflow:
# 1. Install dependencies (npm ci)
# 2. Generate Prisma Client
# 3. Run tests
# 4. Build Docker image
# 5. Push to ECR
# 6. Update ECS Service
```

### GitHub Actions Alternative

```yaml
# .github/workflows/deploy.yml
name: Deploy to AWS

on:
  push:
    branches: [main]

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Configure AWS credentials
        uses: aws-actions/configure-aws-credentials@v2
        with:
          aws-access-key-id: ${{ secrets.AWS_ACCESS_KEY_ID }}
          aws-secret-access-key: ${{ secrets.AWS_SECRET_ACCESS_KEY }}
          aws-region: me-south-1
      
      - name: Login to ECR
        id: login-ecr
        uses: aws-actions/amazon-ecr-login@v1
      
      - name: Build, tag, and push image
        env:
          ECR_REGISTRY: ${{ steps.login-ecr.outputs.registry }}
          ECR_REPOSITORY: daad-studio
          IMAGE_TAG: ${{ github.sha }}
        run: |
          docker build -t $ECR_REGISTRY/$ECR_REPOSITORY:$IMAGE_TAG .
          docker tag $ECR_REGISTRY/$ECR_REPOSITORY:$IMAGE_TAG $ECR_REGISTRY/$ECR_REPOSITORY:latest
          docker push $ECR_REGISTRY/$ECR_REPOSITORY:$IMAGE_TAG
          docker push $ECR_REGISTRY/$ECR_REPOSITORY:latest
      
      - name: Update ECS Service
        run: |
          aws ecs update-service \
            --cluster daad-studio-cluster \
            --service daad-studio-api-service \
            --force-new-deployment
```

### Deployment Commands

```bash
# Full deployment sequence
./scripts/deploy.sh

# Or manually:
# 1. Build
docker build -t daad-studio:latest .

# 2. Tag
docker tag daad-studio:latest ACCOUNT_ID.dkr.ecr.me-south-1.amazonaws.com/daad-studio:latest

# 3. Push
docker push ACCOUNT_ID.dkr.ecr.me-south-1.amazonaws.com/daad-studio:latest

# 4. Deploy
aws ecs update-service --cluster daad-studio-cluster --service daad-studio-api-service --force-new-deployment
```

---

## Health Checks & Auto-Scaling

### Health Endpoint Integration

Our `healthController.js` (Batch 10) provides three health endpoints:

| Endpoint | Purpose | ALB Usage |
|----------|---------|-----------|
| `GET /health` | Full system metrics | Deep health check |
| `GET /health/ready` | Readiness probe | Target group health |
| `GET /health/live` | Liveness probe | Container health |

### ALB Health Check Configuration

```json
{
  "healthCheck": {
    "enabled": true,
    "protocol": "HTTP",
    "port": "traffic-port",
    "path": "/health",
    "healthyThresholdCount": 3,
    "unhealthyThresholdCount": 3,
    "timeoutSeconds": 10,
    "intervalSeconds": 30,
    "matcher": {
      "HttpCode": "200"
    }
  }
}
```

**Health Response Example:**
```json
{
  "status": "healthy",
  "database": {
    "status": "connected",
    "latency": "15ms"
  },
  "memory": {
    "heapUsed": "45MB",
    "percentage": "35%"
  },
  "uptime": 3600
}
```

### Auto-Scaling Configuration

```bash
# Register scalable target
aws application-autoscaling register-scalable-target \
  --service-namespace ecs \
  --scalable-dimension ecs:service:DesiredCount \
  --resource-id service/daad-studio-cluster/daad-studio-api-service \
  --min-capacity 2 \
  --max-capacity 10

# CPU-based scaling policy
aws application-autoscaling put-scaling-policy \
  --service-namespace ecs \
  --scalable-dimension ecs:service:DesiredCount \
  --resource-id service/daad-studio-cluster/daad-studio-api-service \
  --policy-name daad-studio-cpu-scaling \
  --policy-type TargetTrackingScaling \
  --target-tracking-scaling-policy-configuration '{
    "TargetValue": 70.0,
    "PredefinedMetricSpecification": {
      "PredefinedMetricType": "ECSServiceAverageCPUUtilization"
    },
    "ScaleInCooldown": 300,
    "ScaleOutCooldown": 60
  }'

# Memory-based scaling policy
aws application-autoscaling put-scaling-policy \
  --service-namespace ecs \
  --scalable-dimension ecs:service:DesiredCount \
  --resource-id service/daad-studio-cluster/daad-studio-api-service \
  --policy-name daad-studio-memory-scaling \
  --policy-type TargetTrackingScaling \
  --target-tracking-scaling-policy-configuration '{
    "TargetValue": 75.0,
    "PredefinedMetricSpecification": {
      "PredefinedMetricType": "ECSServiceAverageMemoryUtilization"
    },
    "ScaleInCooldown": 300,
    "ScaleOutCooldown": 60
  }'
```

---

## Monitoring & Logging

### CloudWatch Logs

Our Winston logger (`src/utils/logger.js`) writes to:

| Log Group | Content |
|-----------|---------|
| `/ecs/daad-studio-api` | Application logs |
| `logs/combined.log` | All logs |
| `logs/error.log` | Errors only |
| `logs/security-alerts.log` | Anti-Cheat alerts |

### View Logs

```bash
# Stream live logs
aws logs tail /ecs/daad-studio-api --follow

# Search for errors
aws logs filter-log-events \
  --log-group-name /ecs/daad-studio-api \
  --filter-pattern "ERROR"
```

### CloudWatch Alarms

```bash
# High CPU Alarm
aws cloudwatch put-metric-alarm \
  --alarm-name daad-studio-high-cpu \
  --alarm-description "ECS CPU > 80%" \
  --metric-name CPUUtilization \
  --namespace AWS/ECS \
  --statistic Average \
  --period 300 \
  --threshold 80 \
  --comparison-operator GreaterThanThreshold \
  --dimensions Name=ClusterName,Value=daad-studio-cluster Name=ServiceName,Value=daad-studio-api-service

# Database Connection Alarm
aws cloudwatch put-metric-alarm \
  --alarm-name daad-studio-db-connection \
  --alarm-description "RDS connections > 50" \
  --metric-name DatabaseConnections \
  --namespace AWS/RDS \
  --statistic Average \
  --period 300 \
  --threshold 50 \
  --comparison-operator GreaterThanThreshold \
  --dimensions Name=DBInstanceIdentifier,Value=daad-studio-db
```

---

## Troubleshooting

### Issue: Task Fails to Start

```bash
# Check task stopped reason
aws ecs describe-tasks \
  --cluster daad-studio-cluster \
  --tasks TASK_ID \
  --query 'tasks[0].containers[0].reason'

# Common causes:
# - Missing secrets in Secrets Manager
# - RDS security group not allowing ECS
# - Invalid DATABASE_URL format
```

### Issue: Health Check Fails

```bash
# Test health endpoint manually
aws ecs execute-command \
  --cluster daad-studio-cluster \
  --task TASK_ID \
  --container daad-api \
  --interactive \
  --command "wget -qO- http://localhost:3000/health"

# Check logs
aws logs get-log-events \
  --log-group-name /ecs/daad-studio-api \
  --log-stream-name LOG_STREAM \
  --limit 50
```

### Issue: Database Connection Fails

```bash
# Verify RDS is accessible from ECS
aws ecs execute-command \
  --cluster daad-studio-cluster \
  --task TASK_ID \
  --container daad-api \
  --interactive \
  --command "psql $DATABASE_URL -c 'SELECT 1'"

# Check RDS status
aws rds describe-db-instances \
  --db-instance-identifier daad-studio-db \
  --query 'DBInstances[0].DBInstanceStatus'
```

### Issue: Redis Connection Fails

```bash
# Check ElastiCache status
aws elasticache describe-replication-groups \
  --replication-group-id daad-studio-redis

# Test connection from ECS
aws ecs execute-command \
  --cluster daad-studio-cluster \
  --task TASK_ID \
  --container daad-api \
  --interactive \
  --command "redis-cli -h daad-studio-redis.xxxxx.cache.amazonaws.com ping"
```

---

## Cost Estimation

| Service | Configuration | Monthly Cost |
|---------|---------------|--------------|
| ECS Fargate | 2 tasks × 0.5 vCPU × 1GB | $35-50 |
| RDS PostgreSQL | db.t3.medium, Multi-AZ | $60-80 |
| ElastiCache Redis | cache.t3.micro, 2 nodes | $15-25 |
| ALB | Application Load Balancer | $20-30 |
| CloudFront | 100GB transfer | $10-20 |
| ECR | Container registry | $1-5 |
| Secrets Manager | 5 secrets | $5 |
| CloudWatch | Logs & Alarms | $10-20 |
| **Total** | | **$156-235/month** |

---

## Quick Reference Commands

```bash
# Deploy new version
./scripts/deploy.sh

# Check service status
aws ecs describe-services --cluster daad-studio-cluster --services daad-studio-api-service

# Force new deployment
aws ecs update-service --cluster daad-studio-cluster --service daad-studio-api-service --force-new-deployment

# View logs
aws logs tail /ecs/daad-studio-api --follow

# Scale manually
aws ecs update-service --cluster daad-studio-cluster --service daad-studio-api-service --desired-count 4

# Run database migration
aws ecs run-task --cluster daad-studio-cluster --task-definition daad-studio-api --launch-type FARGATE --overrides '{"containerOverrides":[{"name":"daad-api","command":["npx","prisma","migrate","deploy"]}]}'
```

---

## Support

- **Health Check:** `https://api.daad.studio/health`
- **API Documentation:** `https://api.daad.studio/api-docs`
- **CloudWatch Logs:** `/ecs/daad-studio-api`

---

**Built with Batch 10 Dockerfile + Winston Logging + Redis Cache + Gemini AI**

// ====================================================
// Dhad Studio - Swagger/OpenAPI Configuration
// ====================================================
// Docs: http://localhost:3000/api-docs
// ====================================================

const swaggerJSDoc = require('swagger-jsdoc');
const swaggerUi = require('swagger-ui-express');
const { logger } = require('../utils/logger');

// ── Swagger Definition ───────────────────────────────────────────────────────
const swaggerDefinition = {
  openapi: '3.0.3',
  info: {
    title: 'Dhad Studio API',
    version: '1.0.0',
    description: `
# Dhad Studio - EdTech Platform API

Dhad Studio is a comprehensive coding education platform with:
- **Client-side code execution** (no server-side code running)
- **Telemetry-based assessment** with HMAC signature validation
- **Real-time gamification** (XP, levels, streaks, achievements)
- **Teacher analytics** with classroom management
- **High-stakes exam engine** with lockdown mode
- **Anti-cheat security** with suspicious activity detection

## Authentication
All protected endpoints require a JWT token in the Authorization header:
\`\`\`
Authorization: Bearer <your-jwt-token>
\`\`\`

## MessagePack Support
Telemetry ingestion endpoints support both JSON and MessagePack binary encoding.
For MessagePack, set \`Content-Type: application/msgpack\` and send binary payload.

## Rate Limits
- Telemetry submissions: 20 requests/minute
- Analytics endpoints: 60 requests/minute
- Student endpoints: 30 requests/minute
- Auth endpoints: 10 requests/15 minutes
    `,
    contact: {
      name: 'Dhad Studio Support',
      email: 'support@daad.studio',
    },
    license: {
      name: 'MIT',
      url: 'https://opensource.org/licenses/MIT',
    },
  },
  servers: [
    {
      url: 'http://localhost:3000',
      description: 'Development server',
    },
    {
      url: 'https://api.daad.studio',
      description: 'Production server',
    },
  ],
  components: {
    securitySchemes: {
      bearerAuth: {
        type: 'http',
        scheme: 'bearer',
        bearerFormat: 'JWT',
        description: 'JWT token obtained from /api/v1/auth/login',
      },
    },
    schemas: {
      Error: {
        type: 'object',
        properties: {
          error: { type: 'string', description: 'Error type' },
          message: { type: 'string', description: 'Error message' },
          timestamp: { type: 'string', format: 'date-time' },
        },
      },
      SuccessResponse: {
        type: 'object',
        properties: {
          success: { type: 'boolean', example: true },
          message: { type: 'string' },
        },
      },
      LoginRequest: {
        type: 'object',
        required: ['username', 'password'],
        properties: {
          username: { type: 'string', description: 'Email (teacher) or National ID (student)' },
          password: { type: 'string', minLength: 6 },
        },
      },
      LoginResponse: {
        type: 'object',
        properties: {
          accessToken: { type: 'string', description: 'JWT access token' },
          refreshToken: { type: 'string' },
          expiresIn: { type: 'number', example: 604800 },
          userId: { type: 'string' },
          username: { type: 'string' },
          role: { type: 'string', enum: ['STUDENT', 'TEACHER', 'ADMIN'] },
          profile: { type: 'object' },
        },
      },
      RegisterRequest: {
        type: 'object',
        required: ['name', 'role'],
        properties: {
          name: { type: 'string', maxLength: 100 },
          email: { type: 'string', format: 'email', description: 'Required for teachers/admins' },
          nationalId: { type: 'string', description: 'Required for students' },
          role: { type: 'string', enum: ['STUDENT', 'TEACHER', 'ADMIN'] },
          schoolId: { type: 'string' },
          password: { type: 'string', minLength: 8, description: 'Strong password required (8+ chars, uppercase, lowercase, number)' },
        },
      },
      TelemetryPayload: {
        type: 'object',
        required: ['exerciseId', 'challengeId', 'studentId', 'code', 'status'],
        description: 'Client-side execution telemetry. Supports MessagePack binary encoding.',
        properties: {
          exerciseId: { type: 'string', example: 'exercise_challenge_tier1_1' },
          challengeId: { type: 'string', example: 'challenge_tier1_1' },
          studentId: { type: 'string' },
          code: { type: 'string', description: 'Source code executed client-side' },
          language: { type: 'string', default: 'daad' },
          status: { type: 'string', enum: ['PASS', 'FAIL', 'PASSED', 'FAILED'] },
          score: { type: 'number', minimum: 0, maximum: 100 },
          executionTime: { type: 'number', description: 'Execution time in milliseconds' },
          syntaxAccuracy: { type: 'number', minimum: 0, maximum: 100 },
          output: { type: 'string' },
          expectedOutput: { type: 'string' },
        },
      },
      TelemetryHeaders: {
        type: 'object',
        description: 'Required HMAC headers for telemetry',
        properties: {
          'x-telemetry-signature': { type: 'string', description: 'HMAC SHA-256 signature' },
          'x-telemetry-timestamp': { type: 'string', description: 'Unix timestamp in milliseconds' },
          'x-session-secret': { type: 'string', description: 'Session secret for HMAC' },
        },
      },
      Assessment: {
        type: 'object',
        properties: {
          id: { type: 'string' },
          classroomId: { type: 'string' },
          title: { type: 'string' },
          description: { type: 'string' },
          allowedTime: { type: 'number', description: 'Time in minutes' },
          startTime: { type: 'string', format: 'date-time' },
          endTime: { type: 'string', format: 'date-time' },
          totalPoints: { type: 'number' },
          passingScore: { type: 'number' },
          challengeIds: { type: 'array', items: { type: 'string' } },
          lockdownMode: { type: 'boolean' },
        },
      },
      StudentProfile: {
        type: 'object',
        properties: {
          totalXP: { type: 'number' },
          currentLevel: { type: 'number' },
          currentStreak: { type: 'number' },
          longestStreak: { type: 'number' },
          achievements: { type: 'array', items: { type: 'object' } },
        },
      },
      SecurityAlert: {
        type: 'object',
        properties: {
          id: { type: 'string' },
          studentId: { type: 'string' },
          schoolId: { type: 'string' },
          alertType: { type: 'string', enum: ['SUSPICIOUS_SUBMISSION', 'HMAC_FAILURE', 'RATE_LIMIT'] },
          severity: { type: 'string', enum: ['LOW', 'MEDIUM', 'HIGH', 'CRITICAL'] },
          details: { type: 'object' },
          resolved: { type: 'boolean' },
          createdAt: { type: 'string', format: 'date-time' },
        },
      },
    },
  },
  security: [
    { bearerAuth: [] },
  ],
};

// ── Swagger Options ──────────────────────────────────────────────────────────
const swaggerOptions = {
  definition: swaggerDefinition,
  apis: [
    './src/routes/*.js',
    './src/controllers/*.js',
    './src/index.js',
  ],
};

const swaggerSpec = swaggerJSDoc(swaggerOptions);

// ── Swagger UI Setup ─────────────────────────────────────────────────────────
const setupSwagger = (app) => {
  const swaggerUiOptions = {
    customCss: '.swagger-ui .topbar { display: none }',
    customSiteTitle: 'Dhad Studio API Documentation',
    swaggerOptions: {
      persistAuthorization: true,
      displayRequestDuration: true,
      filter: true,
      tryItOutEnabled: true,
    },
  };

  app.use('/api-docs', swaggerUi.serve, swaggerUi.setup(swaggerSpec, swaggerUiOptions));
  
  // Serve raw spec
  app.get('/api-docs.json', (req, res) => {
    res.setHeader('Content-Type', 'application/json');
    res.send(swaggerSpec);
  });

  logger.info('Swagger docs available at /api-docs');
};

module.exports = { swaggerSpec, setupSwagger };

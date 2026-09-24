module.exports = {
  testEnvironment: 'node',
  roots: ['<rootDir>/tests/unit', '<rootDir>/tests/integration'],
  testMatch: ['**/*.test.js'],
  collectCoverage: true,
  coverageDirectory: 'coverage',
  coverageReporters: ['text', 'text-summary'],
  verbose: true,
  forceExit: true,
  clearMocks: true,
  resetModules: true,
  setupFiles: [],
  testTimeout: 15000,
};

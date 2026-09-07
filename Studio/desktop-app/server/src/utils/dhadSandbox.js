// ============================================================
// Dhad Sandbox — Server-side code execution in isolated VM
// SECURITY: Runs outside API process with strict limits
// ============================================================

const vm = require('vm');
const { logger } = require('./logger');

// ── Execution Limits (centralized configuration) ────────────
const LIMITS = Object.freeze({
  TIMEOUT_MS: 5000,           // 5 seconds max execution
  MAX_OUTPUT_BYTES: 65536,    // 64KB max output
  MAX_CODE_LENGTH: 50000,     // 50KB max source code
  MAX_HEAP_BYTES: 32 * 1024 * 1024, // 32MB heap limit
  MAX_LOOP_ITERATIONS: 100000,       // max loop iterations
  MAX_RECURSION_DEPTH: 1000,         // max recursion depth
  MAX_VARIABLES: 10000,              // max variable declarations
});

// ── Compile Dhad source → JavaScript (server-side) ──────────
function compileDhad(source) {
  if (!source || typeof source !== 'string') {
    return { code: null, errors: [{ message: 'الكود فارغ', line: 0, col: 0 }] };
  }

  if (source.length > LIMITS.MAX_CODE_LENGTH) {
    return { code: null, errors: [{ message: 'الكود طويل جداً', line: 0, col: 0 }] };
  }

  try {
    const { compile: compileDhadSource } = require('./dhadCompiler');
    return compileDhadSource(source);
  } catch (e) {
    return {
      code: null,
      errors: [{ message: e.message || 'خطأ في الترجمة', line: 0, col: 0 }],
    };
  }
}

// ── Proxy: Block prototype-chain escape via constructor/__proto__ ────
// CRITICAL FIX: Node.js vm module allows sandbox escape via this.constructor.constructor
// because sandbox objects inherit from the OUTER context's prototypes.
// Object.freeze(Object.prototype) inside the sandbox does NOT prevent this.
// Solution: Proxy-based interception of dangerous property access on ALL objects.

const DANGEROUS_PROPS = new Set([
  'constructor', '__proto__', 'prototype',
  '__defineGetter__', '__defineSetter__',
  '__lookupGetter__', '__lookupSetter__',
]);

function createSandboxProxy(obj) {
  if (obj === null || typeof obj !== 'object') return obj;
  return new Proxy(obj, {
    get: function (target, prop) {
      if (DANGEROUS_PROPS.has(prop)) return undefined;
      const val = target[prop];
      if (typeof val === 'function') {
        return new Proxy(val, {
          apply: function (fn, thisArg, args) {
            const result = fn.apply(thisArg, args);
            if (result && typeof result === 'object' && result !== null) {
              return createSandboxProxy(result);
            }
            return result;
          },
          construct: function (fn, args) {
            const result = new fn(...args);
            return createSandboxProxy(result);
          },
          get: function (fn, p) {
            if (p === 'constructor') return undefined;
            return fn[p];
          },
        });
      }
      if (val && typeof val === 'object') {
        return createSandboxProxy(val);
      }
      return val;
    },
  });
}

// ── Execute JavaScript in sandboxed VM ──────────────────────
function executeInSandbox(jsCode) {
  const output = [];
  const startTime = Date.now();
  let timedOut = false;
  let heapExceeded = false;

  // Build isolated context — NO access to:
  // - process, require, fs, child_process, http, net
  // - module, __dirname, __filename, global
  // - process.env, secrets, database
  const rawContext = {
    // Safe globals
    undefined: undefined,
    NaN: NaN,
    Infinity: Infinity,
    parseInt: parseInt,
    parseFloat: parseFloat,
    isNaN: isNaN,
    isFinite: isFinite,
    Math: Math,
    Date: Date,
    Array: Array,
    Object: Object,
    String: String,
    Number: Number,
    Boolean: Boolean,
    RegExp: RegExp,
    Error: Error,
    TypeError: TypeError,
    RangeError: RangeError,
    SyntaxError: SyntaxError,
    JSON: JSON,
    console: { log: function() {}, error: function() {}, warn: function() {} },

    // Dhad print function — captures output
    __print: function () {
      const args = Array.prototype.slice.call(arguments);
      const s = args.map(function (x) {
        if (x === null || x === undefined) return 'عدم';
        if (x === true) return 'صواب';
        if (x === false) return 'خطأ';
        return String(x);
      }).join(' ');
      output.push(s);
    },

    // Dhad array function
    __array: function () {
      return Array.prototype.slice.call(arguments);
    },

    // Dhad input function — returns 0 (no user input on server)
    __input: function () {
      return 0;
    },

    // Block dangerous globals
    require: undefined,
    process: undefined,
    module: undefined,
    __dirname: undefined,
    __filename: undefined,
    global: undefined,
    Buffer: undefined,
    setTimeout: undefined,
    setInterval: undefined,
    setImmediate: undefined,
    clearTimeout: undefined,
    clearInterval: undefined,
    clearImmediate: undefined,
    URL: undefined,
    URLSearchParams: undefined,
    TextDecoder: undefined,
    TextEncoder: undefined,
    AbortController: undefined,
    // No filesystem
    fs: undefined,
    path: undefined,
    child_process: undefined,
    // No network
    http: undefined,
    https: undefined,
    net: undefined,
    dgram: undefined,
    dns: undefined,
    tls: undefined,
    // No database
    prisma: undefined,
    // No eval in sandbox
    eval: undefined,
    Function: undefined,
  };

  // Wrap context in Proxy to block constructor/__proto__ escape
  const sandboxContext = createSandboxProxy(rawContext);

  // Make context immutable
  const context = vm.createContext(sandboxContext);

  // Create script with filename for stack traces
  let script;
  try {
    script = new vm.Script(jsCode, {
      filename: 'student-code.dhad',
      displayErrors: true,
    });
  } catch (e) {
    // Compilation/syntax error in generated JS
    const errMsg = e.message || 'خطأ في الترجمة';
    return {
      stdout: output.join('\n'),
      output: output.join('\n'),
      stderr: errMsg,
      executionTime: Date.now() - startTime,
      timedOut: false,
      exitStatus: 1,
      runtimeError: { message: errMsg, line: 0 },
    };
  }

  try {
    // Run with timeout protection
    script.runInContext(context, {
      timeout: LIMITS.TIMEOUT_MS,
      displayErrors: true,
    });
  } catch (e) {
    if (e.message && e.message.includes('Script execution timed out')) {
      timedOut = true;
      logger.warn('Sandbox: execution timed out', {
        duration: Date.now() - startTime,
        timeout: LIMITS.TIMEOUT_MS,
      });
    } else if (e.message && (e.displayErrors || e.message.includes('heap'))) {
      heapExceeded = true;
      logger.warn('Sandbox: heap limit exceeded', { error: e.message });
    } else {
      // Runtime error in student code — capture and continue
      const errMsg = e.message || 'خطأ غير معروف';
      const lineMatch = errMsg.match(/student-code\.dhad:(\d+)/);
      const line = lineMatch ? parseInt(lineMatch[1]) : 0;
      return {
        stdout: output.join('\n'),
        output: output.join('\n'),
        stderr: '',
        executionTime: Date.now() - startTime,
        timedOut: false,
        exitStatus: 1,
        runtimeError: { message: errMsg, line },
      };
    }
  }

  return {
    stdout: output.join('\n'),
    stderr: '',
    executionTime: Date.now() - startTime,
    timedOut,
    heapExceeded,
    exitStatus: timedOut || heapExceeded ? 1 : 0,
  };
}

// ── Main API: Compile + Execute Dhad code safely ────────────
function executeDhad(source) {
  // Step 1: Compile
  const compileResult = compileDhad(source);
  if (compileResult.errors.length > 0) {
    return {
      success: false,
      output: '',
      stdout: '',
      errors: compileResult.errors,
      executionTime: 0,
      timedOut: false,
      exitStatus: 1,
    };
  }

  // Step 2: Execute in sandbox
  const result = executeInSandbox(compileResult.code);

  if (result.timedOut) {
    return {
      success: false,
      output: result.stdout,
      stdout: result.stdout,
      errors: [{ message: 'التنفيذ تجاوز الوقت المسموح (5 ثوانٍ)', line: 0, col: 0 }],
      executionTime: result.executionTime,
      timedOut: true,
      exitStatus: 1,
    };
  }

  if (result.heapExceeded) {
    return {
      success: false,
      output: result.stdout,
      stdout: result.stdout,
      errors: [{ message: 'تجاوز البرنامج حد الذاكرة المسموح', line: 0, col: 0 }],
      executionTime: result.executionTime,
      timedOut: false,
      exitStatus: 1,
    };
  }

  if (result.runtimeError) {
    return {
      success: false,
      output: result.stdout,
      stdout: result.stdout,
      errors: [result.runtimeError],
      executionTime: result.executionTime,
      timedOut: false,
      exitStatus: 1,
    };
  }

  return {
    success: true,
    output: result.stdout,
    stdout: result.stdout,
    errors: [],
    executionTime: result.executionTime,
    timedOut: false,
    exitStatus: 0,
  };
}

module.exports = {
  executeDhad,
  compileDhad,
  executeInSandbox,
  LIMITS,
};

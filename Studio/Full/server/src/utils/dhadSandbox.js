// ============================================================
// Dhad Sandbox — Server-side code execution in isolated VM
// SECURITY: Runs outside API process with strict limits
// ============================================================

const vm = require('vm');
const { Worker } = require('worker_threads');
const path = require('path');
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
  // C2 (2026-09-11): block reflection that yields live prototypes
  'getPrototypeOf',
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
            // C2: block constructor walks + reflection on functions.
            // NOTE: 'prototype' must NOT be blocked here — V8 Proxy
            // invariants require the real value for non-configurable
            // 'prototype' (e.g. `Array.prototype.slice` used by __array).
            if (p === 'constructor' || p === 'getPrototypeOf') return undefined;
            if (typeof p === 'string' && p !== 'prototype' && DANGEROUS_PROPS.has(p)) return undefined;
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
// Item-2: input queue shared by sync + worker paths. Values come from the
// challenge inputs JSON. Coercion mirrors the browser prompt() path
// (dhad.js): null/empty → 0, numeric strings → numbers, else raw value.
function coerceInputValue(raw) {
  if (raw === undefined || raw === null) return 0;
  if (typeof raw === 'number') return raw;
  const num = Number(raw);
  return isNaN(num) ? raw : num;
}

function executeInSandbox(jsCode, inputs) {
  const output = [];
  const inputQueue = Array.isArray(inputs) ? inputs.slice(0, 1000) : [];
  const startTime = Date.now();
  let timedOut = false;
  let heapExceeded = false;

  // C2 (2026-09-11): output accounting lives outside the context literal
  let outputBytes = 0;
  let outputTruncated = false;
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

    // Dhad print function — captures output (C2: hard output cap enforced)
    __print: function () {
      if (outputTruncated) return;
      const args = Array.prototype.slice.call(arguments);
      const s = args.map(function (x) {
        if (x === null || x === undefined) return 'عدم';
        if (x === true) return 'صواب';
        if (x === false) return 'خطأ';
        return String(x);
      }).join(' ');
      outputBytes += Buffer.byteLength(s, 'utf8') + 1;
      if (outputBytes > LIMITS.MAX_OUTPUT_BYTES) {
        outputTruncated = true;
        output.push('…[تم اقتطاع المخرجات: تجاوزت الحد المسموح]');
        logger.warn('Sandbox: output cap exceeded');
        return;
      }
      output.push(s);
    },

    // Dhad array function
    __array: function () {
      return Array.prototype.slice.call(arguments);
    },

    // Dhad input function — pops the challenge inputs queue (item 2);
    // an exhausted queue keeps the legacy 0 default.
    __input: function () {
      if (inputQueue.length === 0) return 0;
      return coerceInputValue(inputQueue.shift());
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

  // Item-2 extra: generated CallExpr emits ادخل() directly (var a = ادخل())
  // — alias to the same queue-backed __input so both spellings share the queue.
  rawContext['ادخل'] = rawContext.__input;

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
function compileErrorResult(compileResult) {
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

// Shared shaping for BOTH execution backends (in-process VM and worker).
// Keeps user-visible results identical whichever backend ran the code.
function shapeExecResult(execResult) {
  const result = execResult;

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

function executeDhad(source, inputs) {
  // Step 1: Compile
  const compileResult = compileDhad(source);
  if (compileResult.errors.length > 0) {
    return compileErrorResult(compileResult);
  }

  // Step 2: Execute in sandbox (in-process VM — same shaping as isolated path)
  return shapeExecResult(executeInSandbox(compileResult.code, inputs));
}

// ── C2-full: isolated worker execution ─────────────────────────────────────
// Spawns dhadWorker.js in a dedicated thread with V8 heap caps. The worker
// owns NO secrets/DB/handles; a vm escape still lands in the throwaway
// thread, which the parent kills on timeout. Fail-closed: any spawn or
// protocol failure returns an error result — never silent in-process exec.
const WORKER_TIMEOUT_GRACE_MS = 1000;

function normalizeWorkerResult(msg) {
  const m = (msg && typeof msg === 'object') ? msg : {};
  return {
    stdout: typeof m.stdout === 'string' ? m.stdout : '',
    stderr: typeof m.stderr === 'string' ? m.stderr : '',
    executionTime: typeof m.executionTime === 'number' ? m.executionTime : 0,
    timedOut: m.timedOut === true,
    heapExceeded: m.heapExceeded === true,
    exitStatus: typeof m.exitStatus === 'number' ? m.exitStatus : 1,
    runtimeError: m.runtimeError || undefined,
  };
}

function executeIsolated(jsCode, inputs) {
  return new Promise((resolve) => {
    let settled = false;
    const done = (result) => {
      if (!settled) {
        settled = true;
        resolve(result);
      }
    };

    let worker;
    try {
      worker = new Worker(path.join(__dirname, 'dhadWorker.js'), {
        workerData: {
          code: jsCode,
          timeoutMs: LIMITS.TIMEOUT_MS,
          maxOutputBytes: LIMITS.MAX_OUTPUT_BYTES,
          inputs: Array.isArray(inputs) ? inputs.slice(0, 1000) : [],
        },
        resourceLimits: {
          maxOldGenerationSizeMb: 64,
          codeRangeSizeMb: 32,
          stackSizeMb: 4,
        },
      });
    } catch (e) {
      done({
        stdout: '', stderr: '', executionTime: 0,
        timedOut: false, heapExceeded: false, exitStatus: 1,
        runtimeError: { message: 'تعذر إنشاء بيئة التنفيذ المعزولة', line: 0 },
      });
      return;
    }

    const killTimer = setTimeout(() => {
      try {
        const p = worker.terminate();
        if (p && typeof p.catch === 'function') p.catch(() => {});
      } catch (_) {}
      done({
        stdout: '', stderr: '', executionTime: LIMITS.TIMEOUT_MS,
        timedOut: true, heapExceeded: false, exitStatus: 1,
      });
    }, LIMITS.TIMEOUT_MS + WORKER_TIMEOUT_GRACE_MS);
    if (killTimer && typeof killTimer.unref === 'function') killTimer.unref();

    worker.on('message', (msg) => {
      clearTimeout(killTimer);
      done(normalizeWorkerResult(msg));
    });
    worker.on('error', (err) => {
      clearTimeout(killTimer);
      const text = String((err && err.message) || err || '');
      const oom = /memory|heap|allocation failed|out of memory/i.test(text);
      done({
        stdout: '', stderr: '', executionTime: 0,
        timedOut: false, heapExceeded: oom, exitStatus: 1,
        runtimeError: oom ? undefined : { message: 'انهار عامل التنفيذ', line: 0 },
      });
    });
    worker.on('exit', (code) => {
      clearTimeout(killTimer);
      if (code !== 0) {
        // Exited without posting a result (e.g., V8 OOM kill) → memory exhaustion.
        done({
          stdout: '', stderr: '', executionTime: 0,
          timedOut: false, heapExceeded: true, exitStatus: 1,
        });
      }
    });
  });
}

async function executeDhadIsolated(source, inputs) {
  // Step 1: Compile (pure string transform — safe in-process)
  const compileResult = compileDhad(source);
  if (compileResult.errors.length > 0) {
    return compileErrorResult(compileResult);
  }

  // Step 2: Execute in the isolated worker thread
  return shapeExecResult(await executeIsolated(compileResult.code, inputs));
}

module.exports = {
  executeDhad,
  compileDhad,
  executeInSandbox,
  executeIsolated,
  executeDhadIsolated,
  LIMITS,
};

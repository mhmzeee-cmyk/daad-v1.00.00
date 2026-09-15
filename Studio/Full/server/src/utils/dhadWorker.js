// ============================================================
// Dhad Worker — throwaway execution host for student code (C2-full)
// ------------------------------------------------------------
// Runs UNTRUSTED JS inside vm INSIDE a dedicated worker thread with
// V8 heap caps (set by the parent via resourceLimits). Design notes:
// - Self-contained: only worker_threads + vm. No logger, no DB,
//   no secrets, no app modules — nothing valuable to steal here.
// - Even a full vm escape lands in THIS throwaway worker, which the
//   parent kills on timeout; the OS reclaims the thread.
// - stdout is byte-capped; result shape matches executeInSandbox().
// ============================================================
'use strict';

const { parentPort, workerData } = require('worker_threads');
const vm = require('vm');

const TIMEOUT_MS = workerData.timeoutMs || 5000;
const MAX_OUTPUT_BYTES = workerData.maxOutputBytes || 65536;
const CODE = workerData.code || '';
const INPUTS = Array.isArray(workerData.inputs) ? workerData.inputs.slice(0, 1000) : [];

// Item-2: same coercion as the browser prompt() path (dhad.js) and dhadSandbox.
function coerceInputValue(raw) {
  if (raw === undefined || raw === null) return 0;
  if (typeof raw === 'number') return raw;
  const num = Number(raw);
  return isNaN(num) ? raw : num;
}

const DANGEROUS_PROPS = new Set([
  'constructor', '__proto__', 'prototype',
  '__defineGetter__', '__defineSetter__',
  '__lookupGetter__', '__lookupSetter__',
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
            // NOTE: 'prototype' must NOT be blocked (V8 Proxy invariants
            // require the real value, e.g. Array.prototype.slice).
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

function run() {
  const output = [];
  const startTime = Date.now();
  let outputBytes = 0;
  let outputTruncated = false;

  const rawContext = {
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
    console: { log: function () {}, error: function () {}, warn: function () {} },

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
      if (outputBytes > MAX_OUTPUT_BYTES) {
        outputTruncated = true;
        output.push('…[تم اقتطاع المخرجات: تجاوزت الحد المسموح]');
        return;
      }
      output.push(s);
    },

    __array: function () {
      return Array.prototype.slice.call(arguments);
    },

    __input: function () {
      if (INPUTS.length === 0) return 0;
      return coerceInputValue(INPUTS.shift());
    },

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
    fs: undefined,
    path: undefined,
    child_process: undefined,
    http: undefined,
    https: undefined,
    net: undefined,
    dgram: undefined,
    dns: undefined,
    tls: undefined,
    prisma: undefined,
    eval: undefined,
    Function: undefined,
  };

  // Item-2 extra: generated CallExpr emits ادخل() directly (var a = ادخل())
  // — alias to the same queue-backed __input so both spellings share the queue.
  rawContext['ادخل'] = rawContext.__input;

  const sandboxContext = createSandboxProxy(rawContext);
  const context = vm.createContext(sandboxContext);

  let script;
  try {
    script = new vm.Script(CODE, { filename: 'student-code.dhad', displayErrors: true });
  } catch (e) {
    const errMsg = (e && e.message) || 'خطأ في الترجمة';
    return {
      stdout: output.join('\n'), stderr: errMsg,
      executionTime: Date.now() - startTime, timedOut: false,
      heapExceeded: false, exitStatus: 1,
      runtimeError: { message: errMsg, line: 0 },
    };
  }

  try {
    script.runInContext(context, { timeout: TIMEOUT_MS, displayErrors: true });
  } catch (e) {
    const msg = (e && e.message) || '';
    if (msg.includes('Script execution timed out')) {
      return {
        stdout: output.join('\n'), stderr: '', executionTime: Date.now() - startTime,
        timedOut: true, heapExceeded: false, exitStatus: 1,
      };
    }
    const lineMatch = msg.match(/student-code\.dhad:(\d+)/);
    return {
      stdout: output.join('\n'), stderr: '', executionTime: Date.now() - startTime,
      timedOut: false, heapExceeded: false, exitStatus: 1,
      runtimeError: { message: msg || 'خطأ غير معروف', line: lineMatch ? parseInt(lineMatch[1]) : 0 },
    };
  }

  return {
    stdout: output.join('\n'), stderr: '', executionTime: Date.now() - startTime,
    timedOut: false, heapExceeded: false, exitStatus: 0,
  };
}

try {
  parentPort.postMessage(run());
} catch (e) {
  try {
    parentPort.postMessage({
      stdout: '', stderr: '', executionTime: 0,
      timedOut: false, heapExceeded: false, exitStatus: 1,
      runtimeError: { message: 'انهار عامل التنفيذ', line: 0 },
    });
  } catch (_) {}
}

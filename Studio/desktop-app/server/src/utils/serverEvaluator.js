/**
 * ServerEvaluator — محرك تقييم السيرفر (Phase 2)
 *
 * المبدأ: السيرفر هو مصدر الحقيقة الوحيد.
 * ينفذ الكود عبر Sandbox ثم يقيّم النتيجة.
 *
 * المدخلات: code + challenge (من DB) + executionResult (من Sandbox)
 * المخرجات: { output, passed, score, syntaxScore, performanceScore, codeErrors, hardcodingFlags, suspiciousFlags }
 */

const { verifyCode } = require("./codeVerifier");

// ═══════════════════════════════════════════════════════════════
// تطبيع المخرجات — يطابق SmartEvaluator تماماً
// ═══════════════════════════════════════════════════════════════
function normalize(s) {
  return (s || "")
    .replace(/\s+/g, " ")
    .replace(/[\u0610-\u061A\u064B-\u065F\u0670\u06D6-\u06DC\u06DF-\u06E4\u06E7\u06E8\u06EA-\u06ED]/g, "")
    .replace(/[\u200F\u200E\u2066-\u2069]/g, "")
    .trim();
}

// ═══════════════════════════════════════════════════════════════
// كشف النشاط المشبوه — مدمج في ServerEvaluator (مُحسّن)
// ═══════════════════════════════════════════════════════════════
function detectSuspiciousActivity(code, challenge, executionResult) {
  const flags = [];
  const codeLen = (code || "").trim().length;
  const tier = challenge.tier || 1;
  const difficulty = challenge.difficulty || "BEGINNER";
  const trimmed = (code || "").trim();

  // 1. كود قصير جداً لطباعة مباشرة (جميع المستويات)
  if (codeLen > 0 && codeLen < 30) {
    flags.push({
      type: "very_short_code",
      detail: `الكود ${codeLen} حرف فقط — قد يكون طباعة مباشرة`,
      severity: tier <= 1 ? "LOW" : "MEDIUM",
    });
  }

  // 2. تنفيذ أسرع من المتوقع بشكل مستحيل
  if (executionResult && executionResult.executionTime < 5 && tier >= 2) {
    flags.push({
      type: "impossibly_fast",
      detail: `التنفيذ خلال ${executionResult.executionTime}ms — أسرع من المتوقع`,
      severity: "HIGH",
    });
  }

  // 3. الكود يحتوي على print فقط بدون حسابات (جميع المستويات)
  const onlyPrint = /^(?:print|طباعة)\s*\(\s*["'].*?["']\s*\)\s*;?\s*$/;
  if (onlyPrint.test(trimmed)) {
    flags.push({
      type: "direct_output",
      detail: "الكود يطبع نصاً مباشراً بدون أي حسابات",
      severity: tier <= 1 ? "MEDIUM" : "HIGH",
    });
  }

  // 4. لا يوجد متغيرات ولا شروط ولا حلقات
  const hasVar = /(?:صحيح|نص|عشري|منطقي)\s+\w+/.test(trimmed);
  const hasIf = /(?:if|إذا|اذا)/.test(trimmed);
  const hasLoop = /(?:for|while|طالما|كل)/.test(trimmed);
  const hasMath = /[\+\-\*\/\%]/.test(trimmed);
  const hasFunction = /(?:دالة|function)/.test(trimmed);
  const hasClass = /(?:صنف|class)/.test(trimmed);
  if (!hasVar && !hasIf && !hasLoop && !hasMath && !hasFunction && !hasClass) {
    flags.push({
      type: "no_computation",
      detail: "الكود لا يحتوي على متغيرات أو شروط أو حسابات أو دوال",
      severity: tier <= 1 ? "LOW" : "MEDIUM",
    });
  }

  // 5. تنفيذ تجاوز الوقت (timeout)
  if (executionResult && executionResult.timedOut) {
    flags.push({
      type: "execution_timeout",
      detail: "الكود تجاوز الوقت المسموح — حلقة لا نهائية أو كود غير فعال",
      severity: "HIGH",
    });
  }

  // 6. كشف نمط المتغير + طباعة مباشرة بدون حسابات
  const varPrintPattern = /^(?:صحيح|عشري)\s+\w+\s*=\s*[\d.]+\s*\n\s*(?:طباعة|print)\s*\(/;
  if (varPrintPattern.test(trimmed) && !hasMath && !hasIf && !hasLoop) {
    flags.push({
      type: "trivial_variable",
      detail: "متغير مُعرّف بقيمة ثابتة ثم طباعته مباشرة — قد يكون تحايلاً",
      severity: tier <= 1 ? "MEDIUM" : "HIGH",
    });
  }

  // 7. كشف التمويه: عمليات لا تغير النتيجة (مثل + 0، * 1)
  const trivialOps = /[\+\-\*]\s*0\s*(?:\)|;|\n|$)|[\*\/]\s*1\s*(?:\)|;|\n|$)/;
  if (trivialOps.test(trimmed) && codeLen < 60) {
    flags.push({
      type: "trivial_operations",
      detail: "عمليات حسابية لا تغير النتيجة (مثل + 0 أو * 1)",
      severity: "MEDIUM",
    });
  }

  return {
    suspicious: flags.length > 0,
    flags,
    maxSeverity: flags.length > 0
      ? flags.reduce(
          (max, f) =>
            ["LOW", "MEDIUM", "HIGH", "CRITICAL"].indexOf(f.severity) >
            ["LOW", "MEDIUM", "HIGH", "CRITICAL"].indexOf(max)
              ? f.severity
              : max,
          "LOW"
        )
      : null,
  };
}

// ═══════════════════════════════════════════════════════════════
// استخراج قيم المتغيرات — يتتبع ما تم تعيينه لمتغيرات
// ═══════════════════════════════════════════════════════════════
function extractVariableValues(code) {
  const values = [];
  const lines = (code || "").split("\n");

  for (const line of lines) {
    const trimmed = line.trim();

    // صحيح/عشري اسم = رقم
    const numMatch = trimmed.match(/^(?:صحيح|عشري)\s+(\S+)\s*=\s*(.+)$/);
    if (numMatch) {
      values.push({ name: numMatch[1], value: numMatch[2].trim() });
      continue;
    }

    // نص اسم = "قيمة"
    const strMatch = trimmed.match(/^نص\s+(\S+)\s*=\s*["'](.+?)["']$/);
    if (strMatch) {
      values.push({ name: strMatch[1], value: strMatch[2].trim() });
    }
  }

  return values;
}

// ═══════════════════════════════════════════════════════════════
// تبسيط تعبير رياضي — ي evaluate عبارات بسيطة بشكل آمن
// ═══════════════════════════════════════════════════════════════
function evaluateSimpleExpression(expr) {
  try {
    const clean = expr.replace(/[\s\t]/g, "");
    if (!/^[\d\.\+\-\*\/\%\(\)]+$/.test(clean)) {
      return null;
    }

    const tokenPattern = /\d+\.?\d*|[+\-*/%()]/g;
    const tokens = clean.match(tokenPattern);
    if (!tokens) return null;

    let pos = 0;
    function parseExpression() {
      let result = parseTerm();
      while (pos < tokens.length && (tokens[pos] === "+" || tokens[pos] === "-")) {
        const op = tokens[pos++];
        const right = parseTerm();
        result = op === "+" ? result + right : result - right;
      }
      return result;
    }

    function parseTerm() {
      let result = parseFactor();
      while (pos < tokens.length && (tokens[pos] === "*" || tokens[pos] === "/" || tokens[pos] === "%")) {
        const op = tokens[pos++];
        const right = parseFactor();
        if (op === "*") result *= right;
        else if (op === "/") result = right !== 0 ? result / right : NaN;
        else result %= right;
      }
      return result;
    }

    function parseFactor() {
      if (tokens[pos] === "(") {
        pos++;
        const result = parseExpression();
        if (tokens[pos] === ")") pos++;
        return result;
      }
      if (tokens[pos] === "-") {
        pos++;
        return -parseFactor();
      }
      return parseFloat(tokens[pos++]);
    }

    const result = parseExpression();
    if (typeof result === "number" && isFinite(result)) {
      return String(result);
    }
  } catch (e) {}
  return null;
}

// ═══════════════════════════════════════════════════════════════
// كشف Hardcoding على السيرفر (مُحسّن)
// ═══════════════════════════════════════════════════════════════
function detectHardcoding(code, expectedOutput, tier) {
  const reasons = [];
  if (!expectedOutput || expectedOutput.length === 0) {
    return { suspicious: false, reasons: [] };
  }

  // Tier 1-2: Skip hardcoding detection — basic challenges have simple solutions
  if (tier && tier <= 2) {
    return { suspicious: false, reasons: [] };
  }

  const codeStr = code || "";
  const normalizedExpected = normalize(expectedOutput);

  // 1. كشف print("output") مباشرة
  const printRegex = /(?:print|طباعة)\s*\(\s*["'](.+?)["']\s*\)/g;
  let match;
  const hardcodedOutputs = [];
  while ((match = printRegex.exec(codeStr)) !== null) {
    hardcodedOutputs.push(normalize(match[1]));
  }
  if (
    hardcodedOutputs.length > 0 &&
    hardcodedOutputs.every((o) => o === normalizedExpected)
  ) {
    reasons.push("hardcoded_direct");
  }

  // 2. كشف المتغيرات التي تحمل الإخراج المطلوب مباشرة
  const varValues = extractVariableValues(codeStr);
  for (const v of varValues) {
    const normalizedVarValue = normalize(v.value);
    // إذا كان قيمة المتغير = الإخراج المطلوب (رقم أو نص)
    if (normalizedVarValue === normalizedExpected) {
      // تحقق أن الكود يطبع هذا المتغير
      const printsVar = new RegExp(
        "(?:print|طباعة)\\s*\\(\\s*" + v.name + "\\s*\\)"
      ).test(codeStr);
      if (printsVar) {
        reasons.push("hardcoded_variable");
      }
    }
    // إذا كانت القيمة عملية حسابية بسيطة تعطي النتيجة المطلوبة
    if (typeof v.value === "string" && /[\+\-\*\/\%]/.test(v.value)) {
      const computed = evaluateSimpleExpression(v.value);
      if (computed === normalizedExpected) {
        const printsVar = new RegExp(
          "(?:print|طباعة)\\s*\\(\\s*" + v.name + "\\s*\\)"
        ).test(codeStr);
        if (printsVar) {
          reasons.push("computed_trivial");
        }
      }
    }
  }

  // 3. الكود قصير جداً للإخراجات الطويلة
  if (expectedOutput.split(" ").length > 2 && codeStr.trim().length < 20) {
    reasons.push("too_short");
  }

  // 4. إخراج رقمي مباشر بدون حسابات
  const trimmed = codeStr.trim();
  if (trimmed.length < 40) {
    const onlyPrint = /^(?:print|طباعة)\s*\(\s*["']?[\d.]+["']?\s*\)\s*$/;
    if (onlyPrint.test(trimmed)) {
      reasons.push("numeric_hardcode");
    }
  }

  // 5. غياب المتغيرات الحسابية — يطبع فقط بدون منطق
  const hasMath = /[\+\-\*\/\%]/.test(codeStr);
  const hasIf = /(?:if|إذا|اذا)/.test(codeStr);
  const hasLoop = /(?:for|while|كل|طالما)/.test(codeStr);
  const varCount = (codeStr.match(/(?:صحيح|نص|عشري|منطقي)\s+\w+/g) || []).length;

  const printRegex2 = /(?:print|طباعة)\s*\(\s*(.+?)\s*\)/g;
  let printCount = 0;
  while (printRegex2.exec(codeStr) !== null) printCount++;

  if (printCount === 1 && !hasMath && !hasIf && !hasLoop && varCount === 0) {
    reasons.push("no_computation");
  }

  return {
    suspicious: reasons.length > 0,
    reasons: reasons,
  };
}

// ═══════════════════════════════════════════════════════════════
// تقييم الإخراج (数值 0-60) — يستقبل نتيجة تنفيذ السيرفر
// ═══════════════════════════════════════════════════════════════
function evaluateOutput(actualOutput, expectedOutput, hasDynamicOutput, dynamicOutput, studentName) {
  if (!expectedOutput && !hasDynamicOutput) {
    return { match: false, score: 0, reason: "لا يوجد مخرجات متوقعة للمقارنة" };
  }

  let finalExpected = expectedOutput;
  if (hasDynamicOutput && dynamicOutput) {
    finalExpected = normalize(
      dynamicOutput.replace(/\{\{student\.name\}\}/g, studentName || "")
    );
  } else {
    finalExpected = normalize(expectedOutput);
  }

  const actual = normalize(actualOutput);
  const match = actual === finalExpected;

  return {
    match: match,
    score: match ? 60 : 0,
  };
}

// ═══════════════════════════════════════════════════════════════
// تقييم بنية الكود (数值 0-25)
// ═══════════════════════════════════════════════════════════════
function evaluateCodeStructure(code, requirementsJson) {
  const codeCheck = verifyCode(code, requirementsJson);
  const stats = codeCheck.stats || {};

  let score = 25;
  if (!codeCheck.valid) {
    const penalty = Math.min(codeCheck.errors.length * 5, 25);
    score = 25 - penalty;
  }

  return {
    valid: codeCheck.valid,
    errors: codeCheck.errors,
    score: Math.max(0, score),
    stats: stats,
  };
}

// ═══════════════════════════════════════════════════════════════
// تقييم سلامة الحل (数值 0-15) — مُحسّن
// ═══════════════════════════════════════════════════════════════
function evaluateIntegrity(code, expectedOutput, tier) {
  const hardcodingCheck = detectHardcoding(code, expectedOutput, tier);

  let score = 15;
  if (hardcodingCheck.suspicious) {
    // خصم أكبر للأسباب الأخلاقية
    const severityMap = {
      hardcoded_direct: 15,    // خصم كامل — غش مباشر
      hardcoded_variable: 12,  // خصم كبير — متغير بالقيمة المطلوبة
      computed_trivial: 10,    // خصم متوسط — عملية لا改变了لا شيء
      numeric_hardcode: 15,    // خصم كامل
      too_short: 5,            // خصم بسيط
      no_computation: 8,       // خصم متوسط
    };
    let totalPenalty = 0;
    for (const reason of hardcodingCheck.reasons) {
      totalPenalty += severityMap[reason] || 5;
    }
    score = Math.max(0, 15 - Math.min(totalPenalty, 15));
  }

  return {
    suspicious: hardcodingCheck.suspicious,
    reasons: hardcodingCheck.reasons,
    score: score,
  };
}

// ═══════════════════════════════════════════════════════════════
// الدالة الرئيسية — تقييم شامل على السيرفر (Phase 2)
// تستقبل نتيجة تنفيذ Sandbox وتقوم بالتقييم
// ═══════════════════════════════════════════════════════════════
function evaluate(code, challenge, studentName, executionResult) {
  const codeStr = code || "";
  const codeErrors = [];

  // ── فحص أساسي ──
  if (codeStr.trim().length === 0) {
    codeErrors.push("الكود لا يمكن أن يكون فارغاً");
  }

  const hasPrint = /print\s*\(/.test(codeStr) || /طباعة\s*\(/.test(codeStr);
  if (!hasPrint && codeStr.trim().length > 0) {
    codeErrors.push("الكود يجب أن يحتوي على أمر طباعة (print)");
  }

  // ── أخطاء التنفيذ (من Sandbox) ──
  if (executionResult) {
    if (executionResult.errors && executionResult.errors.length > 0) {
      executionResult.errors.forEach((e) => {
        codeErrors.push(e.message || "خطأ في التنفيذ");
      });
    }
    if (executionResult.timedOut) {
      codeErrors.push("التنفيذ تجاوز الوقت المسموح (5 ثوانٍ)");
    }
  }

  // ── تقييم الإخراج (0-60) — يعتمد على نتيجة Sandbox فقط ──
  const actualOutput = executionResult ? executionResult.stdout || "" : "";
  const outputResult = evaluateOutput(
    actualOutput,
    challenge.expectedOutput || "",
    !!(challenge.dynamicOutput && challenge.dynamicOutput.trim()),
    challenge.dynamicOutput || "",
    studentName
  );

  // ── تقييم بنية الكود (0-25) ──
  const structureResult = evaluateCodeStructure(codeStr, challenge.requirements);

  // ── تقييم السلامة (0-15) ──
  const integrityResult = evaluateIntegrity(codeStr, challenge.expectedOutput || "", challenge.tier || 1);

  // ── كشف النشاط المشبوه ──
  const suspiciousResult = detectSuspiciousActivity(codeStr, challenge, executionResult);

  // ── النتيجة الإجمالية ──
  const score = outputResult.score + structureResult.score + integrityResult.score;

  // ── هل نجح؟ ──
  const passed =
    score >= 60 &&
    codeErrors.length === 0 &&
    structureResult.valid &&
    !integrityResult.suspicious &&
    suspiciousResult.maxSeverity !== "HIGH" &&
    suspiciousResult.maxSeverity !== "CRITICAL" &&
    executionResult && executionResult.success;

  return {
    passed: passed,
    score: Math.min(100, Math.max(0, score)),
    syntaxScore: structureResult.score * 4,
    performanceScore: passed ? 100 : Math.min(100, score),
    codeErrors: codeErrors.concat(structureResult.errors),
    outputMatch: outputResult.match,
    hardcodingFlags: integrityResult.reasons,
    suspiciousFlags: suspiciousResult.flags || [],
    suspiciousMaxSeverity: suspiciousResult.maxSeverity,
    executionTime: executionResult ? executionResult.executionTime : 0,
    stats: structureResult.stats,
  };
}

module.exports = {
  normalize,
  detectHardcoding,
  detectSuspiciousActivity,
  evaluateOutput,
  evaluateCodeStructure,
  evaluateIntegrity,
  evaluate,
};

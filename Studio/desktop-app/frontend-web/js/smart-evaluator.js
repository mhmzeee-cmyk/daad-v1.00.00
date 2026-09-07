/**
 * SmartEvaluator — محرك التقييم الذكي في المتصفح
 * 
 * المبدأ: المتصفح يُقيّم بذكاء، السيرفر يُسجّل بخفة.
 * الأمان: السيرفر يتحقق دائماً — لا يثق بالعميل أبداً.
 * 
 * الدور:
 * 1. تنفيذ الكود (DhadInterpreter)
 * 2. تطبيع المخرجات
 * 3. فحص بنية الكود (المتغيرات، الطباعة، الأسطر)
 * 4. كشف Hardcoding المحسّن
 * 5. كشف النشاط المشبوه
 * 6. حساب النتيجة التعلمية
 * 7. إرسال بيانات مُهيكلة للسيرفر
 */

var SmartEvaluator = (function() {
  'use strict';

  // ═══════════════════════════════════════════════════════════════
  // تطبيع المخرجات — يجب أن يطابق السيرفر تماماً
  // ═══════════════════════════════════════════════════════════════
  function normalize(s) {
    return (s || '')
      .replace(/\s+/g, ' ')
      .replace(/[\u0610-\u061A\u064B-\u065F\u0670\u06D6-\u06DC\u06DF-\u06E4\u06E7\u06E8\u06EA-\u06ED]/g, '')
      .replace(/[\u200F\u200E\u2066-\u2069]/g, '')
      .trim();
  }

  // ═══════════════════════════════════════════════════════════════
  // إزالة التعليقات والأسطر — لفحص بنية الكود
  // ═══════════════════════════════════════════════════════════════
  function stripCommentsAndStrings(code) {
    var result = '';
    var i = 0;
    while (i < code.length) {
      if (code[i] === '/' && code[i + 1] === '/') {
        while (i < code.length && code[i] !== '\n') i++;
        result += '\n';
        continue;
      }
      if (code[i] === '#') {
        while (i < code.length && code[i] !== '\n') i++;
        result += '\n';
        continue;
      }
      if (code[i] === '/' && code[i + 1] === '*') {
        i += 2;
        while (i < code.length && !(code[i] === '*' && code[i + 1] === '/')) i++;
        i += 2;
        result += '  ';
        continue;
      }
      if (code[i] === '"') {
        i++;
        while (i < code.length && code[i] !== '"') {
          if (code[i] === '\\') i++;
          i++;
        }
        i++;
        result += '""';
        continue;
      }
      if (code[i] === "'") {
        i++;
        while (i < code.length && code[i] !== "'") {
          if (code[i] === '\\') i++;
          i++;
        }
        i++;
        result += "''";
        continue;
      }
      result += code[i];
      i++;
    }
    return result;
  }

  // ═══════════════════════════════════════════════════════════════
  // فحص بنية الكود — منقّل من codeVerifier.js مع تحسينات
  // ═══════════════════════════════════════════════════════════════
  function verifyCodeStructure(code, requirementsJson) {
    var errors = [];
    var requirements = [];
    try {
      requirements = JSON.parse(requirementsJson || '[]');
    } catch (e) {
      return { valid: true, errors: [], stats: {} };
    }
    if (!Array.isArray(requirements) || requirements.length === 0) {
      return { valid: true, errors: [], stats: {} };
    }

    var isBoundary = function(ch) {
      return !ch || /[\s(){}\[\];،؛\n\r]/.test(ch);
    };

    var cleanCode = stripCommentsAndStrings(code);

    // عد المتغيرات
    var allTypes = ['صحيح', 'نص', 'عشري', 'منطقي', 'حرف', 'ثابت', 'فراغ', 'تلقائي'];
    var varDeclarations = 0;
    for (var t = 0; t < allTypes.length; t++) {
      var idx = 0;
      var keyword = allTypes[t];
      while ((idx = cleanCode.indexOf(keyword, idx)) !== -1) {
        var before = cleanCode[idx - 1];
        var after = cleanCode.slice(idx + keyword.length).match(/^\s+[\p{L}\p{N}_]+/u);
        if (isBoundary(before) && after) {
          varDeclarations++;
        }
        idx += keyword.length;
      }
    }

    // عد جمل الطباعة
    var printStatements = 0;
    var idx2 = 0;
    while ((idx2 = cleanCode.indexOf('طباعة', idx2)) !== -1) {
      var before2 = cleanCode[idx2 - 1];
      if (isBoundary(before2)) printStatements++;
      idx2 += 7;
    }
    // طباعة إنجليزية
    var printRegex = /\bprint\s*\(/g;
    var engPrints = (cleanCode.match(printRegex) || []).length;
    printStatements += engPrints;

    // عد الأسطر غير الفارغة
    var lines = code.split('\n').filter(function(l) {
      var trimmed = l.trim();
      return trimmed && !trimmed.startsWith('//') && !trimmed.startsWith('/*');
    }).length;

    // فحص كل متطلب
    for (var r = 0; r < requirements.length; r++) {
      var req = requirements[r];
      switch (req.type) {
        case 'minVariables':
          if (varDeclarations < (req.count || 1)) {
            errors.push(req.message || 'يجب استخدام متغير واحد على الأقل');
          }
          break;
        case 'minPrints':
          if (printStatements < (req.count || 1)) {
            errors.push(req.message || 'يجب استخدام أمر طباعة واحد على الأقل');
          }
          break;
        case 'mustContain':
          var values = req.values || [];
          for (var v = 0; v < values.length; v++) {
            if (cleanCode.indexOf(values[v]) === -1) {
              errors.push(req.message || 'الكود يجب أن يحتوي على: ' + values[v]);
            }
          }
          break;
        case 'mustNotContain':
          var forbidden = req.values || [];
          for (var f = 0; f < forbidden.length; f++) {
            if (cleanCode.indexOf(forbidden[f]) !== -1) {
              errors.push(req.message || 'لا يُسمح باستخدام: ' + forbidden[f]);
            }
          }
          break;
        case 'minLines':
          if (lines < (req.count || 1)) {
            errors.push(req.message || 'يجب كتابة ' + (req.count || 1) + ' أسطر على الأقل');
          }
          break;
        case 'must_use_keyword':
          var kw = req.keyword;
          if (kw) {
            var found = false;
            var kidx = 0;
            while ((kidx = cleanCode.indexOf(kw, kidx)) !== -1) {
              var kBefore = cleanCode[kidx - 1];
              var kAfter = cleanCode[kidx + kw.length];
              if (isBoundary(kBefore) && isBoundary(kAfter)) {
                found = true;
                break;
              }
              kidx += kw.length;
            }
            if (!found) {
              errors.push(req.message || 'يجب استخدام كلمة المفتاحية: ' + kw);
            }
          }
          break;
      }
    }

    return {
      valid: errors.length === 0,
      errors: errors,
      stats: { variables: varDeclarations, prints: printStatements, lines: lines }
    };
  }

  // ═══════════════════════════════════════════════════════════════
  // كشف Hardcoding المحسّن — يكشف التجميع والمتغيرات المضللة
  // ═══════════════════════════════════════════════════════════════
  function detectHardcoding(code, expectedOutput, normalizeFn, tier) {
    var reasons = [];
    if (!expectedOutput || expectedOutput.length === 0) return { suspicious: false, reasons: [] };

    var norm = normalizeFn || normalize;
    var codeStr = code || '';
    var difficulty = tier || 1;

    // ══════════════════════════════════════════════════════════════
    // للمبتدئين (tier 1-2): لا نُنذر على حلول بسيطة صحيحة
    // لأن print("مرحبا") هو الحل المتوقع تحديداً
    // ══════════════════════════════════════════════════════════════

    // 1. كشف print("output") مباشرة — حتى مع تجميع
    var printRegex = /(?:print|طباعة)\s*\(\s*(.+?)\s*\)/g;
    var match;
    var printOutputs = [];
    while ((match = printRegex.exec(codeStr)) !== null) {
      var arg = match[1].trim();
      // تحليل النصية فقط (قد يكون متغير)
      if (/^["']/.test(arg)) {
        printOutputs.push(norm(arg.replace(/^["']|["']$/g, '')));
      } else if (/\+/.test(arg)) {
        // تجميع: "أ" + "ب" — نحاول تجميعه
        var parts = arg.split('+').map(function(p) {
          return p.trim().replace(/^["']|["']$/g, '');
        });
        if (parts.every(function(p) { return /^["']/.test(p.trim()) || p.trim() === ''; })) {
          printOutputs.push(norm(parts.join('')));
        }
      }
    }

    // إذا جميع المخرجات = الإخراج المتوقع
    if (printOutputs.length > 0 && printOutputs.every(function(o) { return o === norm(expectedOutput); })) {
      // ═══════════════════════════════════════════════════════════
      // للمبتدئين: print("الإخراج المتوقع") حل صحيح وليس غش
      // فقط نُنذر إذا الكود أطول من 100 حرف (تجميع معقد)
      // ═══════════════════════════════════════════════════════════
      if (difficulty >= 3 && codeStr.trim().length > 100) {
        reasons.push('hardcoded_direct');
      }
    }

    // 2. الكود قصير جداً للإخراجات الطويلة — فقط للمستويات المتقدمة
    if (difficulty >= 3 && expectedOutput.split(' ').length > 2 && codeStr.trim().length < 20) {
      reasons.push('too_short');
    }

    // 3. كشف كود يحتوي على رقم الإخراج فقط بدون منطق — فقط للمستويات المتقدمة
    if (difficulty >= 3) {
      var trimmed = codeStr.trim();
      if (trimmed.length < 40) {
        var onlyPrint = /^(?:print|طباعة)\s*\(\s*["']?[\d.]+["']?\s*\)\s*$/;
        if (onlyPrint.test(trimmed)) {
          reasons.push('numeric_hardcode');
        }
      }
    }

    // 4. كشف غياب المتغيرات الحسابية — الكود يطبع فقط بدون تعقيد
    // (مُلغي — لا يُستخدم في التقييم النهائي)

    return {
      suspicious: reasons.length > 0,
      reasons: reasons
    };
  }

  // ═══════════════════════════════════════════════════════════════
  // كشف النشاط المشبوه
  // ═══════════════════════════════════════════════════════════════
  function detectSuspiciousActivity(code, executionTime, tier) {
    var flags = [];

    // كود قصير جداً (أقل من 5 أحرف — لا يمكن أن يكون كود صالح)
    if (code.trim().length < 5) {
      flags.push('extremely_short_code');
    }

    return { suspicious: flags.length > 0, flags: flags };
  }

  // ═══════════════════════════════════════════════════════════════
  // حساب النتيجة التعلمية (0-100)
  // ═══════════════════════════════════════════════════════════════
  function calculateScore(codeCheck, hardcodingCheck, outputMatch) {
    var score = 0;

    // نتيجة الإخراج (50 نقطة)
    if (outputMatch) score += 50;

    // نتيجة بنية الكود (30 نقطة)
    if (codeCheck.valid) score += 30;
    else {
      // جزئي: كل خطأ ينقص 10 نقاط
      var errorPenalty = Math.min(codeCheck.errors.length * 10, 30);
      score += (30 - errorPenalty);
    }

    // نتيجة عدم Hardcoding (20 نقطة)
    if (!hardcodingCheck.suspicious) score += 20;
    else {
      // خصم حسب السبب
      var penalty = hardcodingCheck.reasons.length * 10;
      score += Math.max(0, 20 - penalty);
    }

    return Math.max(0, Math.min(100, score));
  }

  // ═══════════════════════════════════════════════════════════════
  // الدالة الرئيسية — تقييم شامل
  // ═══════════════════════════════════════════════════════════════
  function evaluate(code, challenge, studentOutput) {
    var startTime = Date.now();

    // 1. تنفيذ الكود
    var execResult = DhadInterpreter.execute(code);
    var execTime = Date.now() - startTime;

    // 2. تطبيع المخرجات
    var actualOutput = normalize(execResult.output);
    var expectedOutput = normalize(challenge.expectedOutput || '');

    // 3. فحص أن الكود يحتوي على print
    var hasPrint = /print\s*\(/.test(code) || /طباعة\s*\(/.test(code);
    var codeErrors = [];

    if (!hasPrint && code.trim().length > 0) {
      codeErrors.push('الكود يجب أن يحتوي على أمر طباعة (print)');
    }
    if (code.trim().length === 0) {
      codeErrors.push('الكود لا يمكن أن يكون فارغاً');
    }

    // 4. فحص بنية الكود
    var codeCheck = verifyCodeStructure(code, challenge.requirements);
    if (!codeCheck.valid) {
      codeErrors = codeErrors.concat(codeCheck.errors);
    }

    // 5. كشف Hardcoding
    var hardcodingCheck = detectHardcoding(code, challenge.expectedOutput || '', null, challenge.tier || 1);

    // 6. كشف النشاط المشبوه
    var suspiciousCheck = detectSuspiciousActivity(code, execTime, challenge.tier || 0);

    // 7. مطابقة الإخراج
    var hasExpectedOutput = challenge.expectedOutput && challenge.expectedOutput.trim().length > 0;
    var hasDynamicOutput = challenge.dynamicOutput && challenge.dynamicOutput.trim().length > 0;
    var outputMatch = false;

    if (hasDynamicOutput) {
      var dynamicExpected = normalize(
        challenge.dynamicOutput.replace(/\{\{student\.name\}\}/g, '').replace(/\s+/g, ' ').trim()
      );
      outputMatch = (actualOutput === dynamicExpected);
    } else if (hasExpectedOutput) {
      outputMatch = (actualOutput === expectedOutput);
    } else {
      outputMatch = true; // لا يوجد إخراج متوقع — يُقبل أي إخراج
    }

    // 8. حساب النتيجة
    var score = calculateScore(codeCheck, hardcodingCheck, outputMatch);

    // 9. تحديد النتيجة النهائية
    var passed = outputMatch && codeCheck.valid && codeErrors.length === 0 && !hardcodingCheck.suspicious;

    // إذا الكود يحتوي على أخطاء تنفيذ
    if (execResult.errors && execResult.errors.length > 0) {
      passed = false;
      codeErrors.push('أخطاء في التنفيذ: ' + execResult.errors.join(', '));
    }

    // 10. تنظيف الأخطاء المكررة
    codeErrors = codeErrors.filter(function(e, i, arr) { return arr.indexOf(e) === i; });

    return {
      // النتيجة
      passed: passed,
      score: score,
      codeErrors: codeErrors,

      // المخرجات
      actualOutput: actualOutput,
      executionTime: execTime,

      // إحصائيات الكود
      stats: codeCheck.stats || {},

      // كشف الخداع
      hardcoding: hardcodingCheck,
      suspicious: suspiciousCheck,

      // للسيرفر — بيانات مُهيكلة
      meta: {
        codeLength: code.length,
        linesCount: code.split('\n').length,
        hasPrint: hasPrint,
        variablesCount: codeCheck.stats ? codeCheck.stats.variables : 0,
        printsCount: codeCheck.stats ? codeCheck.stats.prints : 0,
      }
    };
  }

  // ═══════════════════════════════════════════════════════════════
  // الواجهة العامة
  // ═══════════════════════════════════════════════════════════════
  return {
    normalize: normalize,
    verifyCodeStructure: verifyCodeStructure,
    detectHardcoding: detectHardcoding,
    detectSuspiciousActivity: detectSuspiciousActivity,
    calculateScore: calculateScore,
    evaluate: evaluate
  };
})();

if (typeof module !== 'undefined' && module.exports) {
  module.exports = SmartEvaluator;
}

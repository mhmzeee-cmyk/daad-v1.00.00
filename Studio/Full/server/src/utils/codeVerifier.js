/**
 * Code Verification System
 * Verifies student code meets challenge requirements
 * Requirements format (JSON array):
 * [
 *   { "type": "minVariables", "count": 1, "message": "يجب استخدام متغير واحد على الأقل" },
 *   { "type": "minPrints", "count": 1, "message": "يجب استخدام أمر طباعة واحد على الأقل" },
 *   { "type": "mustContain", "values": ["نص"], "message": "يجب استخدام نوع البيانات نص" },
 *   { "type": "mustNotContain", "values": ["console.log"], "message": "لا تستخدم console.log directly" },
 *   { "type": "minLines", "count": 2, "message": "يجب كتابة سطرين على الأقل" }
 * ]
 */

function verifyCode(code, requirementsJson) {
  const errors = [];
  let requirements = [];

  try {
    requirements = JSON.parse(requirementsJson || "[]");
  } catch (e) {
    return { valid: true, errors: [] }; // Invalid JSON = no requirements
  }

  if (!Array.isArray(requirements) || requirements.length === 0) {
    return { valid: true, errors: [] };
  }

  // Arabic Unicode ranges for boundary detection
  const isArabic = (ch) => ch && /[\u0600-\u06FF\u0750-\u077F\uFB50-\uFDFF\uFE70-\uFEFE]/.test(ch);
  const isBoundary = (ch) => !ch || /[\s(){}\[\];،؛\n\r]/.test(ch);

  // Strip comments and string literals to prevent hiding keywords in them
  function stripCommentsAndStrings(code) {
    let result = '';
    let i = 0;
    while (i < code.length) {
      // Single-line comment //
      if (code[i] === '/' && code[i + 1] === '/') {
        while (i < code.length && code[i] !== '\n') i++;
        result += '\n';
        continue;
      }
      // Single-line comment # or ---
      if (code[i] === '#') {
        while (i < code.length && code[i] !== '\n') i++;
        result += '\n';
        continue;
      }
      // Multi-line comment /* */
      if (code[i] === '/' && code[i + 1] === '*') {
        i += 2;
        while (i < code.length && !(code[i] === '*' && code[i + 1] === '/')) i++;
        i += 2;
        result += '  ';
        continue;
      }
      // Double-quoted string
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
      // Single-quoted string
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

  // Use stripped code for keyword/variable detection
  const cleanCode = stripCommentsAndStrings(code);

  // Count variables (Arabic keyword + name) - manual boundary check since \b doesn't work with Arabic
  const allTypes = ['صحيح', 'نص', 'عشري', 'منطقي', 'حرف', 'ثابت', 'فراغ', 'تلقائي'];
  let varDeclarations = 0;
  for (const t of allTypes) {
    let idx = 0;
    while ((idx = cleanCode.indexOf(t, idx)) !== -1) {
      const before = cleanCode[idx - 1];
      const after = cleanCode.slice(idx + t.length).match(/^\s+[\p{L}\p{N}_]+/u);
      if (isBoundary(before) && after) {
        varDeclarations++;
      }
      idx += t.length;
    }
  }

  // Count print statements (طباعة)
  let printStatements = 0;
  {
    let idx = 0;
    while ((idx = cleanCode.indexOf('طباعة', idx)) !== -1) {
      const before = cleanCode[idx - 1];
      if (isBoundary(before)) printStatements++;
      idx += 7; // length of طباعة
    }
  }

  // Count lines (non-empty, non-comment)
  const lines = code.split('\n').filter(l => {
    const trimmed = l.trim();
    return trimmed && !trimmed.startsWith('//') && !trimmed.startsWith('/*');
  }).length;

  for (const req of requirements) {
    switch (req.type) {
      case "minVariables":
        if (varDeclarations < (req.count || 1)) {
          errors.push(req.message || "يجب استخدام متغير واحد على الأقل");
        }
        break;
      case "minPrints":
        if (printStatements < (req.count || 1)) {
          errors.push(req.message || "يجب استخدام أمر طباعة واحد على الأقل");
        }
        break;
      case "mustContain":
        const values = req.values || [];
        for (const v of values) {
          if (!cleanCode.includes(v)) {
            errors.push(req.message || "الكود يجب أن يحتوي على: " + v);
          }
        }
        break;
      case "mustNotContain":
        const forbidden = req.values || [];
        for (const v of forbidden) {
          if (cleanCode.includes(v)) {
            errors.push(req.message || "لا يُسمح باستخدام: " + v);
          }
        }
        break;
      case "minLines":
        if (lines < (req.count || 1)) {
          errors.push(req.message || "يجب كتابة " + (req.count || 1) + " أسطر على الأقل");
        }
        break;
      case "must_use_keyword":
        const keyword = req.keyword;
        if (keyword) {
          let found = false;
          let idx = 0;
          while ((idx = cleanCode.indexOf(keyword, idx)) !== -1) {
            const before = cleanCode[idx - 1];
            const after = cleanCode[idx + keyword.length];
            if (isBoundary(before) && isBoundary(after)) {
              found = true;
              break;
            }
            idx += keyword.length;
          }
          if (!found) {
            errors.push(req.message || "يجب استخدام كلمة المفتاحية: " + keyword);
          }
        }
        break;
    }
  }

  return { valid: errors.length === 0, errors };
}

module.exports = { verifyCode };

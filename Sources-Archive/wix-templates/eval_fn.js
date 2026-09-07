function evaluateSimpleExpression(expr) {
  try {
    // إزالة المسافة وال۩_Field characters
    const clean = expr.replace(/[\s\t]/g, "");
    // فحص أنه رقم فقط أو عملية بسيطة
    if (/^[\d\.\+\-\*\/\%\(\)]+$/.test(clean)) {
      // Safe shunting-yard calculator (no Function/eval)
      const precedence = { '+': 1, '-': 1, '*': 2, '/': 2, '%': 2 };
      const assoc = { '+': 'L', '-': 'L', '*': 'L', '/': 'L', '%': 'L' };

      // Tokenize
      const tokens = [];
      let i = 0;
      while (i < clean.length) {
        const ch = clean[i];
        if (ch === '(' || ch === ')') { tokens.push(ch); i++; }
        else if ('+-*/%'.includes(ch)) {
          // Handle unary minus/plus at start, after '(' or after operator
          if ((ch === '-' || ch === '+') &&
              (tokens.length === 0 ||
               tokens[tokens.length - 1] === '(' ||
               '+-*/%'.includes(tokens[tokens.length - 1]))) {
            let num = ch;
            i++;
            while (i < clean.length && (clean[i] === '.' || (clean[i] >= '0' && clean[i] <= '9'))) {
              num += clean[i]; i++;
            }
            if (num === '-' || num === '+') { return null; }
            const v = Number(num);
            if (isNaN(v)) { return null; }
            tokens.push({ v: v });
          } else {
            tokens.push(ch); i++;
          }
        }
        else if ((ch >= '0' && ch <= '9') || ch === '.') {
          let num = '';
          while (i < clean.length && (clean[i] === '.' || (clean[i] >= '0' && clean[i] <= '9'))) {
            num += clean[i]; i++;
          }
          if (num === '.' || num === '') { return null; }
          const v = Number(num);
          if (isNaN(v)) { return null; }
          tokens.push({ v: v });
        }
        else { return null; }
      }

      // Shunting-Yard to RPN
      const output = [];
      const ops = [];
      for (const tok of tokens) {
        if (typeof tok === 'object') { output.push(tok); }
        else if (tok === '(') { ops.push(tok); }
        else if (tok === ')') {
          while (ops.length && ops[ops.length - 1] !== '(') output.push({ v: ops.pop() });
          if (!ops.length) { return null; }
          ops.pop();
        }
        else {
          while (ops.length && ops[ops.length - 1] !== '(' &&
                 ((assoc[tok] === 'L' && precedence[tok] <= precedence[ops[ops.length - 1]]) ||
                  (assoc[tok] === 'R' && precedence[tok] < precedence[ops[ops.length - 1]]))) {
            output.push({ v: ops.pop() });
          }
          ops.push(tok);
        }
      }
      while (ops.length) {
        if (ops[ops.length - 1] === '(') { return null; }
        output.push({ v: ops.pop() });
      }

      // Evaluate RPN
      if (output.length === 0) { return null; }
      const stack = [];
      for (const tok of output) {
        if (typeof tok === 'object' && typeof tok.v === 'number') {
          stack.push(tok.v);
        } else if (typeof tok === 'object' && typeof tok.v === 'string') {
          if (stack.length < 2) { return null; }
          const b = stack.pop();
          const a = stack.pop();
          switch (tok.v) {
            case '+': stack.push(a + b); break;
            case '-': stack.push(a - b); break;
            case '*': stack.push(a * b); break;
            case '/': stack.push(a / b); break;
            case '%': stack.push(a % b); break;
            default: return null;
          }
        } else {
          return null;
        }
      }
      if (stack.length !== 1) { return null; }
      const result = stack[0];
      if (typeof result === "number" && isFinite(result)) {
        return String(result);
      }
    }
  } catch (e) {}
  return null;
}

const cases = [
  ["2+3*4", "14"],
  ["(2+3)*4", "20"],
  ["10/0", null],
  ["-5+3", "-2"],
  ["2*-3", "-6"],
  ["10%3", "1"],
  ["((2+3)*4)-5", "15"],
  ["", null],
  ["2+", null],
  ["(2+3", null],
];
let fail = 0;
for (const [inp, exp] of cases) {
  const got = evaluateSimpleExpression(inp);
  const ok = got === exp;
  if (!ok) fail++;
  console.log((ok?"PASS":"FAIL") + " " + JSON.stringify(inp) + " => " + JSON.stringify(got) + " (expected " + JSON.stringify(exp) + ")");
}
process.exit(fail ? 1 : 0);

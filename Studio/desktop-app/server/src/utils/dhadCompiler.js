// ============================================================
// Dhad Compiler — Server-side (Node.js compatible)
// Concatenates all source files into single execution context
// ============================================================

const path = require('path');
const fs = require('fs');

const DHAD_DIR = path.join(__dirname, 'dhad');

let _compiler = null;

function getCompiler() {
  if (_compiler) return _compiler;

  // Read all source files
  const astSrc = fs.readFileSync(path.join(DHAD_DIR, 'ast.js'), 'utf8');
  const lexerSrc = fs.readFileSync(path.join(DHAD_DIR, 'lexer.js'), 'utf8');
  const parserSrc = fs.readFileSync(path.join(DHAD_DIR, 'parser.js'), 'utf8');
  const codegenSrc = fs.readFileSync(path.join(DHAD_DIR, 'codegen.js'), 'utf8');

  // Combine into single scope — all globals shared
  // Remove the CommonJS export wrappers (they reference `module` which we provide)
  const combined = `
    var module = { exports: {} };
    var exports = module.exports;

    ${astSrc}

    var DhadAST = module.exports;

    module = { exports: {} };
    exports = module.exports;

    ${lexerSrc}

    var DhadLexer = module.exports;

    module = { exports: {} };
    exports = module.exports;

    ${parserSrc}

    var DhadParser = module.exports;

    module = { exports: {} };
    exports = module.exports;

    ${codegenSrc}

    var DhadCodeGen = module.exports;
  `;

  // Execute in global-like context
  const fn = new Function(combined);
  fn();

  // Now DhadLexer, DhadParser, DhadCodeGen should be available
  // But since new Function has its own scope, we need a different approach

  // Use sandboxed execution
  const vm = require('vm');
  const sandbox = {
    module: { exports: {} },
    exports: {},
    console: { log: function(){}, error: function(){}, warn: function(){} },
    parseInt, parseFloat, isNaN, isFinite,
    Math, Date, Array, Object, String, Number, Boolean, RegExp, Error,
    TypeError, RangeError, SyntaxError, JSON,
    undefined, NaN, Infinity,
    ArrayBuffer, SharedArrayBuffer, DataView,
    Uint8Array, Int8Array, Uint16Array, Int16Array, Uint32Array, Int32Array,
    Float32Array, Float64Array,
  };

  const context = vm.createContext(sandbox);

  // Load AST first
  vm.runInContext(astSrc, context, { filename: 'ast.js' });

  // Get AST reference
  const DhadAST = context.DhadAST;
  context.DhadAST = DhadAST;

  // Load Lexer (doesn't depend on AST or Parser)
  vm.runInContext(lexerSrc, context, { filename: 'lexer.js' });

  // Load Parser (depends on DhadLexer.TT and DhadAST)
  context.DhadLexer = context.DhadLexer;
  context.DhadAST = DhadAST;
  vm.runInContext(parserSrc, context, { filename: 'parser.js' });

  // Load CodeGen (depends on DhadAST)
  vm.runInContext(codegenSrc, context, { filename: 'codegen.js' });

  _compiler = {
    Lexer: context.DhadLexer.Lexer,
    Parser: context.DhadParser.Parser,
    CodeGen: context.DhadCodeGen.CodeGen,
    TT: context.DhadLexer.TT,
  };

  return _compiler;
}

function compile(source) {
  const { Lexer, Parser, CodeGen } = getCompiler();
  const errors = [];

  const lexer = new Lexer(source);
  const lexResult = lexer.tokenize();
  errors.push(...(lexResult.errors || []));
  if (errors.length > 0) return { code: null, errors, ast: null };

  const parser = new Parser(lexResult.tokens, errors);
  const ast = parser.parseProgram();
  errors.push(...(parser.errors || []));
  if (errors.length > 0) return { code: null, errors, ast: null };

  const codegen = new CodeGen();
  const code = codegen.generate(ast);

  return { code, errors: [], ast };
}

module.exports = { compile, getCompiler };

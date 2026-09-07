// ═══════════════════════════════════════════════════════════════════════════════
// Dhad (ض) Language — JavaScript Code Generator
// Walks AST → JavaScript output (matches C++ compiler patterns)
// ═══════════════════════════════════════════════════════════════════════════════

var DhadCodeGen = (function () {
  'use strict';

  var AST = DhadAST;

  function CodeGen() {
    this.output = '';
    this.indent = 0;
    this.functions = {};    // collected function declarations
    this.classes = {};      // collected class declarations
    this.globals = [];      // global variable declarations
    this.initialized = {};  // track initialized variables
    this.classFields = null; // track class fields when inside a method
  }

  // ── Helpers ─────────────────────────────────────────────────────────────────
  CodeGen.prototype.line = function (code) {
    var prefix = '';
    for (var i = 0; i < this.indent; i++) prefix += '  ';
    this.output += prefix + code + '\n';
  };

  CodeGen.prototype.open = function () { this.indent++; };
  CodeGen.prototype.close = function () { this.indent--; };

  CodeGen.prototype.emit = function (code) {
    this.output += code;
  };

  // ── Main Generate ───────────────────────────────────────────────────────────
  CodeGen.prototype.generate = function (program) {
    this.output = '';
    this.functions = {};
    this.classes = {};
    this.globals = [];
    this.initialized = {};

    // First pass: collect all function and class declarations
    this.collectDeclarations(program);

    // Emit runtime helpers
    this.emitRuntime();
    this.output += '\n';

    // Emit collected classes
    this.emitClasses();
    this.output += '\n';

    // Emit collected functions
    this.emitFunctions();
    this.output += '\n';

    // Emit global variable declarations
    this.emitGlobals();
    this.output += '\n';

    // Emit main body
    this.line('// ── Main Program ──');
    this.open();
    this.emitStatements(program.body);
    this.close();

    return this.output;
  };

  // ── First Pass: Collect Declarations ────────────────────────────────────────
  CodeGen.prototype.collectDeclarations = function (program) {
    for (var i = 0; i < program.body.length; i++) {
      var stmt = program.body[i];
      if (stmt.type === 'FunctionDecl') {
        this.functions[stmt.name] = stmt;
      } else if (stmt.type === 'ClassDecl') {
        this.classes[stmt.name] = stmt;
      } else if (stmt.type === 'StructDecl') {
        this.classes[stmt.name] = stmt;
      }
    }
  };

  // ── Runtime Helpers ─────────────────────────────────────────────────────────
  CodeGen.prototype.emitRuntime = function () {
    this.line('if (typeof __print === "undefined") {');
    this.open();
    this.line('var __output = [];');
    this.line('var __print = function() {');
    this.open();
    this.line('var args = Array.prototype.slice.call(arguments);');
    this.line('var s = args.map(function(x) { return x === null ? "عدم" : x === true ? "صواب" : x === false ? "خطأ" : String(x); }).join(" ");');
    this.line('if (typeof __guiPrint === "function") __guiPrint(s);');
    this.line('else { if (typeof __output !== "undefined") __output.push(s); if (typeof console !== "undefined") console.log(s); }');
    this.close();
    this.line('};');
    this.close();
    this.line('}');
    this.line('var __array = function() { return Array.prototype.slice.call(arguments); };');
    // ── المكتبة المدمجة الحقيقية (توافق مع سطح المكتب) ──
    this.line('var __files = { handles: [], next: 0, mem: {} };');
    this.line('function افتح(مسار, وضع) {'
      + ' var key = "f" + (__files.next++);'
      + ' if ((وضع || "").indexOf("w") >= 0) __files.mem[مسار] = "";'
      + ' __files.handles.push({ path: مسار, mode: وضع || "r", pos: 0, key: key });'
      + ' return __files.handles.length - 1; }');
    this.line('function سجّل(h, سطر) { var f = __files.handles[h];'
      + ' if (!f) return -1;'
      + ' __files.mem[f.path] = (__files.mem[f.path] || "") + سطر + "\\n"; return 0; }');
    this.line('function اقرأ_سطر(h) { var f = __files.handles[h];'
      + ' if (!f) return "";'
      + ' var all = __files.mem[f.path] || "";'
      + ' if (f.pos >= all.length) return "";'
      + ' var nl = all.indexOf("\\n", f.pos);'
      + ' if (nl < 0) { var last = all.slice(f.pos); f.pos = all.length; return last; }'
      + ' var line = all.slice(f.pos, nl); f.pos = nl + 1; return line; }');
    this.line('function فصل(s, sep) { s = String(s); sep = String(sep); if (!sep) return [s]; return s.split(sep); }'
      + 'function ليس_نهاية(h) { var f = __files.handles[h];'
      + ' if (!f) return false;'
      + ' return f.pos < ((__files.mem[f.path] || "").length); }');
    this.line('function أغلق(h) { /* في المتصفح: لا مورد نظام */ }');
    this.line('function أضف(arr, قيمة) { arr.push(قيمة); return arr; }');
    this.line('function طول(x) { return (typeof x === "string") ? x.length : (x && x.length) || 0; }');
    this.line('function يحتوي(كامل, جزء) { return String(كامل).indexOf(String(جزء)) >= 0; }');
    this.line('function اقتطع(s, بداية, ل) { s = String(s); return s.substr(بداية || 0, ل === undefined ? undefined : ل); }');
    this.line('function استبدل(s, من, إلى) { return String(s).split(من).join(إلى); }');
    // Canvas-based game engine
    this.line('if (typeof __dhad === "undefined") {');
    this.open();
    this.line('var __dhad = (function() {');
    this.open();
    this.line('var _canvas = null, _ctx = null, _w = 800, _h = 600;');
    this.line('var _color = "#000000", _bgColor = "#ffffff", _lineW = 2;');
    this.line('var _sprites = {}, _images = {};');
    this.line('function _ensureCanvas() {');
    this.open();
    this.line('if (_canvas) return;');
    this.line('if (typeof document !== "undefined") {');
    this.open();
    this.line('_canvas = document.createElement("canvas");');
    this.line('_canvas.width = _w; _canvas.height = _h;');
    this.line('_canvas.style.cssText = "display:block;margin:10px auto;border:2px solid #333;border-radius:8px;background:#111;";');
    this.line('var container = document.getElementById("outputConsole") || document.body;');
    this.line('if (container && container.parentNode) container.parentNode.insertBefore(_canvas, container);');
    this.line('else if (typeof document.body !== "undefined") document.body.appendChild(_canvas);');
    this.line('_ctx = _canvas.getContext("2d");');
    this.line('_ctx.fillStyle = _bgColor; _ctx.fillRect(0, 0, _w, _h);');
    this.close();
    this.line('} else {');
    this.open();
    this.line('_canvas = { width: _w, height: _h, style: {} };');
    this.line('_ctx = { fillRect: function(){}, strokeRect: function(){}, beginPath: function(){}, arc: function(){}, fill: function(){}, stroke: function(){}, moveTo: function(){}, lineTo: function(){}, fillText: function(){}, strokeText: function(){}, clearRect: function(){}, drawImage: function(){}, save: function(){}, restore: function(){}, translate: function(){}, rotate: function(){}, scale: function(){}, measureText: function(){ return {width:0}; }, createLinearGradient: function(){ return {addColorStop:function(){}}; }, createRadialGradient: function(){ return {addColorStop:function(){}}; }, fillStyle: "", strokeStyle: "", lineWidth: 1, font: "", textAlign: "", textBaseline: "", globalAlpha: 1, imageSmoothingEnabled: true };');
    this.close();
    this.line('}');
    this.close();
    this.line('}');
    // Drawing functions
    this.line('function _rect(x,y,w,h,filled) { _ensureCanvas(); if(filled){_ctx.fillStyle=_color;_ctx.fillRect(x,y,w,h);}else{_ctx.strokeStyle=_color;_ctx.strokeRect(x,y,w,h);} }');
    this.line('function _circle(x,y,r,filled) { _ensureCanvas(); _ctx.beginPath(); _ctx.arc(x,y,r,0,Math.PI*2); if(filled){_ctx.fillStyle=_color;_ctx.fill();}else{_ctx.strokeStyle=_color;_ctx.stroke();} }');
    this.line('function _line(x1,y1,x2,y2) { _ensureCanvas(); _ctx.beginPath(); _ctx.moveTo(x1,y1); _ctx.lineTo(x2,y2); _ctx.strokeStyle=_color; _ctx.lineWidth=_lineW; _ctx.stroke(); }');
    this.line('function _text(t,x,y,size) { _ensureCanvas(); _ctx.fillStyle=_color; _ctx.font=(size||16)+"px Arial"; _ctx.textAlign="right"; _ctx.textBaseline="top"; _ctx.fillText(t,x,y); }');
    this.line('function _clear() { _ensureCanvas(); _ctx.fillStyle=_bgColor; _ctx.fillRect(0,0,_w,_h); }');
    this.line('function _fill(x,y,w,h,c) { _ensureCanvas(); _ctx.fillStyle=c||_color; _ctx.fillRect(x,y,w,h); }');
    this.line('function _drawSprite(name,x,y,scale) { _ensureCanvas(); var img=_sprites[name]; if(img&&img.complete&&img.naturalWidth>0){var s=scale||1; _ctx.drawImage(img,x,y,img.width*s,img.height*s);} }');
    this.line('function _loadSprite(name,url) { var img=new Image(); img.src=url; _sprites[name]=img; return {width:0,height:0,name:name}; }');
    this.line('function _drawPixel(x,y,c) { _ensureCanvas(); _ctx.fillStyle=c; _ctx.fillRect(x,y,1,1); }');
    this.line('function _gradient(x1,y1,x2,y2,stops) { _ensureCanvas(); var g=_ctx.createLinearGradient(x1,y1,x2,y2); for(var i=0;i<stops.length;i++){g.addColorStop(stops[i][0],stops[i][1]);} return g; }');
    this.line('function _saveState() { _ensureCanvas(); _ctx.save(); }');
    this.line('function _restoreState() { _ensureCanvas(); _ctx.restore(); }');
    this.line('function _translate(x,y) { _ensureCanvas(); _ctx.translate(x,y); }');
    this.line('function _rotate(a) { _ensureCanvas(); _ctx.rotate(a); }');
    this.line('function _scale(x,y) { _ensureCanvas(); _ctx.scale(x,y); }');
    this.line('function _opacity(a) { _ensureCanvas(); _ctx.globalAlpha=a; }');
    this.line('function _textSize(t,s) { _ensureCanvas(); _ctx.font=(s||16)+"px Arial"; return _ctx.measureText(t); }');
    // Public API
    this.line('return {');
    this.open();
    this.line('ensureCanvas: _ensureCanvas,');
    this.line('rectangle: function(x,y,w,h) { _rect(x,y,w,h,true); },');
    this.line('strokeRect: function(x,y,w,h) { _rect(x,y,w,h,false); },');
    this.line('circle: function(x,y,r) { _circle(x,y,r,true); },');
    this.line('strokeCircle: function(x,y,r) { _circle(x,y,r,false); },');
    this.line('line: _line,');
    this.line('text: _text,');
    this.line('clear: _clear,');
    this.line('fill: _fill,');
    this.line('setColor: function(c) { _color=c; },');
    this.line('setBgColor: function(c) { _bgColor=c; _ensureCanvas(); _ctx.fillStyle=c; _ctx.fillRect(0,0,_w,_h); },');
    this.line('setLineWidth: function(w) { _lineW=w; },');
    this.line('getCanvasSize: function() { return {width:_w,height:_h}; },');
    this.line('setCanvasSize: function(w,h) { _w=w; _h=h; _ensureCanvas(); _canvas.width=w; _canvas.height=h; _ctx.fillStyle=_bgColor; _ctx.fillRect(0,0,_w,_h); },');
    this.line('loadSprite: _loadSprite,');
    this.line('drawSprite: _drawSprite,');
    this.line('pixel: _drawPixel,');
    this.line('gradient: _gradient,');
    this.line('save: _saveState,');
    this.line('restore: _restoreState,');
    this.line('translate: _translate,');
    this.line('rotate: _rotate,');
    this.line('scale: _scale,');
    this.line('opacity: _opacity,');
    this.line('textSize: _textSize,');
    this.line('loadImage: function(p) { return _loadSprite(p, p); },');
    this.line('drawImage: function(img,x,y,w,h) { _ensureCanvas(); if(typeof img==="string"){_loadSprite("_tmp_"+x+"_"+y, img);img=_sprites["_tmp_"+x+"_"+y];} if(img&&img.complete&&img.naturalWidth>0){if(w&&h){_ctx.drawImage(img,x,y,w,h);}else{_ctx.drawImage(img,x,y);}} },');
    this.line('imageSize: function(i) { return {width:i.width||0,height:i.height||0}; },');
    this.line('saveImage: function() {},');
    this.line('cropImage: function(i) { return i; },');
    this.line('resize: function(i) { return i; },');
    this.line('rotateImage: function(i) { return i; },');
    this.line('flipImage: function(i) { return i; },');
    this.line('filter: function(i) { return i; },');
    this.line('overlay: function() {},');
    this.line('background: function(c) { _ensureCanvas(); _ctx.fillStyle=c; _ctx.fillRect(0,0,_w,_h); },');
    this.line('SUPPORTED_FORMATS: {jpg:{name:"JPEG"},jpeg:{name:"JPEG"},png:{name:"PNG"},gif:{name:"GIF"},bmp:{name:"BMP"},tiff:{name:"TIFF"},webp:{name:"WebP"},svg:{name:"SVG"},ico:{name:"ICO"}},');
    this.line('isFormatSupported: function(p) { var e=p.split(".").pop().toLowerCase(); return this.SUPPORTED_FORMATS.hasOwnProperty(e); },');
    this.line('getSupportedFormats: function() { return Object.keys(this.SUPPORTED_FORMATS); },');
    this.line('detectFormat: function(p) { var e=p.split(".").pop().toLowerCase(); return this.SUPPORTED_FORMATS[e]||null; }');
    this.close();
    this.line('};');
    this.line('})();');
    this.close();
    this.line('}');
  };

  // ── Emit Classes ────────────────────────────────────────────────────────────
  CodeGen.prototype.emitClasses = function () {
    for (var name in this.classes) {
      if (this.classes.hasOwnProperty(name)) {
        var cls = this.classes[name];
        var ctor = null;
        var fields = [];
        var methods = [];
        var privateVars = [];
        for (var i = 0; i < cls.body.length; i++) {
          var m = cls.body[i];
          if (m.type === 'ConstructorDecl') { ctor = m; }
          else if (m.type === 'VarDecl') {
            fields.push(m);
            if (m.access === 'private') privateVars.push(m);
          }
          else if (m.type === 'FunctionDecl') methods.push(m);
        }

        // Emit constructor function
        this.line('function ' + name + '(' + (ctor ? ctor.params.map(function(p) { return p.name; }).join(', ') : '') + ') {');
        this.open();

        // Call parent constructor if inheriting
        if (cls.parent) {
          if (ctor && ctor.params.length > 0) {
            this.line(cls.parent + '.call(this, ' + ctor.params.map(function(p) { return p.name; }).join(', ') + ');');
          } else {
            this.line(cls.parent + '.apply(this, arguments);');
          }
        }

        // Private vars as closure variables
        for (var pv = 0; pv < privateVars.length; pv++) {
          var pvar = privateVars[pv];
          this.line('var _' + pvar.name + ' = ' + (pvar.init ? this.genExpr(pvar.init) : 'undefined') + ';');
        }

        // Default params
        if (ctor) {
          for (var j = 0; j < ctor.params.length; j++) {
            var p = ctor.params[j];
            if (p.defaultValue !== null && p.defaultValue !== undefined) {
              this.line('if (' + p.name + ' === undefined) ' + p.name + ' = ' + this.genExpr(p.defaultValue) + ';');
            }
          }
        }

        // Public/protected fields
        for (var k = 0; k < fields.length; k++) {
          var f = fields[k];
          if (f.access !== 'private') {
            this.line('this.' + f.name + ' = ' + (f.init ? this.genExpr(f.init) : 'undefined') + ';');
          }
        }

        // Constructor body — set classFields so field assignments use this.
        if (ctor) {
          var savedFieldsForCtor = this.classFields;
          this.classFields = fields.map(function(f) { return f.name; });
          this.emitStatements(ctor.body);
          this.classFields = savedFieldsForCtor;
        }

        // Methods with getters/setters for private fields
        for (var pm = 0; pm < privateVars.length; pm++) {
          var pv = privateVars[pm];
          this.line('Object.defineProperty(this, "' + pv.name + '", {');
          this.open();
          this.line('get: function() { return _' + pv.name + '; },');
          this.line('set: function(v) { _' + pv.name + ' = v; },');
          this.line('enumerable: true');
          this.close();
          this.line('});');
        }

        // Methods
        var fieldNames = fields.map(function(f) { return f.name; });
        for (var mi = 0; mi < methods.length; mi++) {
          var met = methods[mi];
          if (met.isStatic) {
            this.line(name + '.' + met.name + ' = function(' + met.params.map(function(p) { return p.name; }).join(', ') + ') {');
          } else {
            this.line('this.' + met.name + ' = function(' + met.params.map(function(p) { return p.name; }).join(', ') + ') {');
          }
          this.open();
          var savedFields = this.classFields;
          this.classFields = fieldNames;
          this.emitStatements(met.body);
          this.classFields = savedFields;
          this.close();
          this.line('};');
        }

        this.close();
        this.line('}');

        // Inheritance
        if (cls.parent) {
          this.line(name + '.prototype = Object.create(' + cls.parent + '.prototype);');
          this.line(name + '.prototype.constructor = ' + name + ';');
        }
      }
    }
  };

  // ── Emit Functions ──────────────────────────────────────────────────────────
  CodeGen.prototype.emitFunctions = function () {
    for (var name in this.functions) {
      if (this.functions.hasOwnProperty(name)) {
        var fn = this.functions[name];
        var defaults = [];
        var params = fn.params.map(function(p) {
          var n = p.name;
          if (n === 'س') n = 'x';
          if (n === 'ص') n = 'y';
          if (p.defaultValue) defaults.push({ name: n, expr: p.defaultValue });
          return n;
        }).join(', ');
        this.line('function ' + name + '(' + params + ') {');
        this.open();
        for (var di = 0; di < defaults.length; di++) {
          this.line('if (' + defaults[di].name + ' === undefined) ' + defaults[di].name + ' = ' + this.genExpr(defaults[di].expr) + ';');
        }
        this.emitStatements(fn.body);
        this.close();
        this.line('}');
      }
    }
  };

  // ── Emit Globals ────────────────────────────────────────────────────────────
  CodeGen.prototype.emitGlobals = function () {
    for (var i = 0; i < this.globals.length; i++) {
      var g = this.globals[i];
      this.line('var ' + g.name + (g.init ? ' = ' + this.genExpr(g.init) : ' = undefined') + ';');
    }
  };

  // ── Emit Statements ─────────────────────────────────────────────────────────
  CodeGen.prototype.emitStatements = function (stmts) {
    for (var i = 0; i < stmts.length; i++) {
      this.emitStmt(stmts[i]);
    }
  };

  CodeGen.prototype.emitStmt = function (stmt) {
    if (!stmt) return;

    switch (stmt.type) {
      case 'VarDecl':   this.emitVarDecl(stmt); break;
      case 'Assignment': this.emitAssignment(stmt); break;
      case 'CompoundAssignment': this.emitCompoundAssignment(stmt); break;
      case 'ExprStmt':  this.emitExprStmt(stmt); break;
      case 'IfStmt':    this.emitIf(stmt); break;
      case 'WhileStmt': this.emitWhile(stmt); break;
      case 'ForStmt':   this.emitFor(stmt); break;
      case 'ForEachStmt': this.emitForEach(stmt); break;
      case 'DoWhileStmt': this.emitDoWhile(stmt); break;
      case 'SwitchStmt': this.emitSwitch(stmt); break;
      case 'ReturnStmt': this.emitReturn(stmt); break;
      case 'BreakStmt': this.line('break;'); break;
      case 'ContinueStmt': this.line('continue;'); break;
      case 'FunctionDecl': break; // already emitted via emitFunctions
      case 'ClassDecl': break; // already emitted via emitClasses
      case 'StructDecl': break; // already emitted via emitClasses
      case 'NamespaceDecl': this.emitNamespace(stmt); break;
      case 'EnumDecl': this.emitEnum(stmt); break;
      case 'TryCatchStmt': this.emitTryCatch(stmt); break;
      case 'ThrowStmt': this.line('throw ' + this.genExpr(stmt.value) + ';'); break;
      case 'DeleteStmt': this.line('delete ' + this.genExpr(stmt.operand) + ';'); break;
      case 'PrintStmt': this.emitPrint(stmt); break;
      case 'InputStmt': this.emitInput(stmt); break;
      case 'MemberAssignment': this.line(this.genExpr(stmt.object) + '.' + stmt.member + ' = ' + this.genExpr(stmt.value) + ';'); break;
      case 'ArraySubscriptAssign': this.line(stmt.name + '[' + this.genExpr(stmt.index) + '] = ' + this.genExpr(stmt.value) + ';'); break;
      case 'TemplateDecl': break; // templates are erased at runtime
      case 'Import': break; // no-op in web interpreter
      case 'Export': break; // no-op in web interpreter
      case 'Program': this.emitStatements(stmt.body); break;

      // Image Processing (20)
      case 'LoadImage': this.emitLoadImage(stmt); break;
      case 'DrawImage': this.emitDrawImage(stmt); break;
      case 'ImageSize': this.emitImageSize(stmt); break;
      case 'SaveImage': this.emitSaveImage(stmt); break;
      case 'CropImage': this.emitCropImage(stmt); break;
      case 'Resize': this.emitResize(stmt); break;
      case 'RotateImage': this.emitRotateImage(stmt); break;
      case 'FlipImage': this.emitFlipImage(stmt); break;
      case 'Opacity': this.emitOpacity(stmt); break;
      case 'Filter': this.emitFilter(stmt); break;
      case 'Overlay': this.emitOverlay(stmt); break;
      case 'Background': this.emitBackground(stmt); break;
      case 'Pixel': this.emitPixel(stmt); break;
      case 'Draw': this.emitDraw(stmt); break;
      case 'Fill': this.emitFill(stmt); break;
      case 'Rectangle': this.emitRectangle(stmt); break;
      case 'Circle': this.emitCircle(stmt); break;
      case 'Line': this.emitLine(stmt); break;
      case 'TextOnCanvas': this.emitTextOnCanvas(stmt); break;
      case 'Clear': this.emitClearCanvas(stmt); break;
      default:
        this.line('// TODO: ' + stmt.type);
    }
  };

  // ── Variable Declaration ────────────────────────────────────────────────────
  CodeGen.prototype.emitVarDecl = function (stmt) {
    var init = stmt.init ? this.genExpr(stmt.init) : 'undefined';
    var keyword = 'var';
    if (stmt.varType.indexOf('const') === 0) {
      keyword = 'const';
    }
    var name = stmt.name;
    if (name === 'س') name = 'x';
    if (name === 'ص') name = 'y';
    this.line(keyword + ' ' + name + ' = ' + init + ';');
  };

  // ── Assignment ──────────────────────────────────────────────────────────────
  CodeGen.prototype.emitAssignment = function (stmt) {
    var name = stmt.name;
    if (this.classFields && this.classFields.indexOf(name) !== -1) {
      name = 'this.' + name;
    }
    if (name === 'س') name = 'x';
    if (name === 'ص') name = 'y';
    this.line(name + ' = ' + this.genExpr(stmt.value) + ';');
  };

  // ── Compound Assignment ─────────────────────────────────────────────────────
  CodeGen.prototype.emitCompoundAssignment = function (stmt) {
    var op = stmt.op;
    // Translate ^ to ** for JavaScript exponentiation
    if (op === '^') op = '**';
    if (op === '^=') op = '**=';
    var name = stmt.name;
    if (this.classFields && this.classFields.indexOf(name) !== -1) {
      name = 'this.' + name;
    }
    if (name === 'س') name = 'x';
    if (name === 'ص') name = 'y';
    this.line(name + ' ' + op + ' ' + this.genExpr(stmt.value) + ';');
  };

  // ── Expression Statement ────────────────────────────────────────────────────
  CodeGen.prototype.emitExprStmt = function (stmt) {
    this.line(this.genExpr(stmt.expr) + ';');
  };

  // ── If Statement ────────────────────────────────────────────────────────────
  CodeGen.prototype.emitIf = function (stmt) {
    this.line('if (' + this.genExpr(stmt.condition) + ') {');
    this.open();
    this.emitStatements(stmt.thenBody);
    this.close();
    if (stmt.elseBody && stmt.elseBody.length > 0) {
      this.line('} else {');
      this.open();
      this.emitStatements(stmt.elseBody);
      this.close();
    }
    this.line('}');
  };

  // ── While Loop ──────────────────────────────────────────────────────────────
  CodeGen.prototype.emitWhile = function (stmt) {
    this.line('while (' + this.genExpr(stmt.condition) + ') {');
    this.open();
    this.emitStatements(stmt.body);
    this.close();
    this.line('}');
  };

  // ── For Loop ────────────────────────────────────────────────────────────────
  CodeGen.prototype.emitFor = function (stmt) {
    this.line('for (');
    this.open();
    if (stmt.init) {
      if (stmt.init.type === 'VarDecl') {
        var initName = stmt.init.name;
        if (initName === 'س') initName = 'x';
        if (initName === 'ص') initName = 'y';
        this.output += 'var ' + initName + ' = ' + (stmt.init.init ? this.genExpr(stmt.init.init) : '0');
      } else if (stmt.init.type === 'Assignment') {
        var initN = stmt.init.name;
        if (initN === 'س') initN = 'x';
        if (initN === 'ص') initN = 'y';
        this.output += initN + ' = ' + this.genExpr(stmt.init.value);
      } else {
        this.output += this.genExpr(stmt.init);
      }
    }
    this.output += '; ';
    this.output += stmt.condition ? this.genExpr(stmt.condition) : '';
    this.output += '; ';
    if (stmt.update) {
      var upd = stmt.update;
      // ExprStmtAST wraps an inner expression
      if (upd.type === 'ExprStmt') upd = upd.expr;
      if (upd.type === 'Assignment') {
        var updN = upd.name;
        if (updN === 'س') updN = 'x';
        if (updN === 'ص') updN = 'y';
        this.output += updN + ' = ' + this.genExpr(upd.value);
      } else {
        this.output += this.genExpr(upd);
      }
    }
    this.output += ') {\n';
    this.emitStatements(stmt.body);
    this.line('}');
  };

  // ── For-Each Loop ──────────────────────────────────────────────────────────
  CodeGen.prototype.emitForEach = function (stmt) {
    var vn = stmt.varName;
    if (vn === 'س') vn = 'x';
    if (vn === 'ص') vn = 'y';
    this.line('for (var ' + vn + ' of ' + this.genExpr(stmt.iterable) + ') {');
    this.open();
    this.emitStatements(stmt.body);
    this.close();
    this.line('}');
  };

  // ── Do-While Loop ───────────────────────────────────────────────────────────
  CodeGen.prototype.emitDoWhile = function (stmt) {
    this.line('do {');
    this.open();
    this.emitStatements(stmt.body);
    this.close();
    this.line('} while (' + this.genExpr(stmt.condition) + ');');
  };

  // ── Switch Statement ────────────────────────────────────────────────────────
  CodeGen.prototype.emitSwitch = function (stmt) {
    this.line('switch (' + this.genExpr(stmt.expression) + ') {');
    this.open();
    for (var i = 0; i < stmt.cases.length; i++) {
      var c = stmt.cases[i];
      this.line('case ' + this.genExpr(c.value) + ':');
      this.open();
      this.emitStatements(c.body);
      // Don't auto-add break — user must explicitly use توقف
      this.close();
    }
    if (stmt.defaultBody && stmt.defaultBody.length > 0) {
      this.line('default:');
      this.open();
      this.emitStatements(stmt.defaultBody);
      this.close();
    }
    this.close();
    this.line('}');
  };

  // ── Return Statement ────────────────────────────────────────────────────────
  CodeGen.prototype.emitReturn = function (stmt) {
    if (stmt.value) {
      this.line('return ' + this.genExpr(stmt.value) + ';');
    } else {
      this.line('return;');
    }
  };

  // ── Function Declaration ────────────────────────────────────────────────────
  CodeGen.prototype.emitFunctionDecl = function (stmt) {
    var params = stmt.params.map(function(p) {
      var n = p.name;
      if (n === 'س') n = 'x';
      if (n === 'ص') n = 'y';
      return n;
    }).join(', ');
    this.line('function ' + stmt.name + '(' + params + ') {');
    this.open();
    // Emit default parameter values
    for (var i = 0; i < stmt.params.length; i++) {
      var p = stmt.params[i];
      if (p.defaultValue !== null && p.defaultValue !== undefined) {
        this.line('if (' + p.name + ' === undefined) ' + p.name + ' = ' + this.genExpr(p.defaultValue) + ';');
      }
    }
    this.emitStatements(stmt.body);
    this.close();
    this.line('}');
  };

  // ── Class Declaration (inline) ──────────────────────────────────────────────
  CodeGen.prototype.emitClassDecl = function (stmt) {
    var ctor = null;
    var members = [];
    for (var i = 0; i < stmt.body.length; i++) {
      if (stmt.body[i].type === 'ConstructorDecl') {
        ctor = stmt.body[i];
      } else {
        members.push(stmt.body[i]);
      }
    }
    this.line('function ' + stmt.name + '(' + (ctor ? ctor.params.map(function(p) { return p.name; }).join(', ') : '') + ') {');
    this.open();
    // Call parent constructor if inheriting
    if (stmt.parent) {
      if (ctor && ctor.params.length > 0) {
        this.line(stmt.parent + '.call(this, ' + ctor.params.map(function(p) { return p.name; }).join(', ') + ');');
      } else {
        this.line(stmt.parent + '.apply(this, arguments);');
      }
    }
    if (ctor) {
      for (var j = 0; j < ctor.params.length; j++) {
        var p = ctor.params[j];
        if (p.defaultValue !== null && p.defaultValue !== undefined) {
          this.line('if (' + p.name + ' === undefined) ' + p.name + ' = ' + this.genExpr(p.defaultValue) + ';');
        }
      }
    }
    // Fields first (instance only)
    for (var k = 0; k < members.length; k++) {
      var member = members[k];
      if (member.type === 'VarDecl' && !member.isStatic) {
        this.line('this.' + member.name + ' = ' + (member.init ? this.genExpr(member.init) : 'undefined') + ';');
      }
    }
    // Constructor body — set classFields so field assignments use this.
    if (ctor) {
      var savedFieldsForCtor = this.classFields;
      this.classFields = fieldNames;
      this.emitStatements(ctor.body);
      this.classFields = savedFieldsForCtor;
    }
    // Instance methods
    var fieldNames = members.filter(function(m) { return m.type === 'VarDecl' && !m.isStatic; }).map(function(m) { return m.name; });
    for (var m = 0; m < members.length; m++) {
      var mem = members[m];
      if (mem.type === 'FunctionDecl' && !mem.isStatic) {
        this.line('this.' + mem.name + ' = function(' + mem.params.map(function(p) { return p.name; }).join(', ') + ') {');
        this.open();
        var savedFields = this.classFields;
        this.classFields = fieldNames;
        this.emitStatements(mem.body);
        this.classFields = savedFields;
        this.close();
        this.line('};');
      }
    }
    this.close();
    this.line('}');
    // Inheritance prototype chain
    if (stmt.parent) {
      this.line(stmt.name + '.prototype = Object.create(' + stmt.parent + '.prototype);');
      this.line(stmt.name + '.prototype.constructor = ' + stmt.name + ';');
    }
    // Static members on constructor
    for (var s = 0; s < members.length; s++) {
      var sm = members[s];
      if (sm.type === 'VarDecl' && sm.isStatic) {
        this.line(stmt.name + '.' + sm.name + ' = ' + (sm.init ? this.genExpr(sm.init) : 'undefined') + ';');
      }
      if (sm.type === 'FunctionDecl' && sm.isStatic) {
        this.line(stmt.name + '.' + sm.name + ' = function(' + sm.params.map(function(p) { return p.name; }).join(', ') + ') {');
        this.open();
        this.emitStatements(sm.body);
        this.close();
        this.line('};');
      }
    }
  };

  // ── Struct Declaration (same as class for JS) ──────────────────────────────
  CodeGen.prototype.emitStructDecl = function (stmt) {
    this.line('function ' + stmt.name + '() {');
    this.open();
    for (var i = 0; i < stmt.body.length; i++) {
      var member = stmt.body[i];
      if (member.type === 'VarDecl') {
        this.line('this.' + member.name + ' = ' + (member.init ? this.genExpr(member.init) : 'undefined') + ';');
      }
    }
    this.close();
    this.line('}');
  };

  // ── Namespace (create namespace object) ─────────────────────────────────────
  CodeGen.prototype.emitNamespace = function (stmt) {
    this.line('var ' + stmt.name + ' = {};');
    this.line('(function() {');
    this.open();
    // Capture the namespace object
    this.line('var _ns = ' + stmt.name + ';');
    // Emit body but redirect assignments to namespace object
    for (var i = 0; i < stmt.body.length; i++) {
      var member = stmt.body[i];
      if (member.type === 'FunctionDecl') {
        this.line('_ns.' + member.name + ' = function(' + member.params.map(function(p) { return p.name; }).join(', ') + ') {');
        this.open();
        this.emitStatements(member.body);
        this.close();
        this.line('};');
      } else if (member.type === 'VarDecl') {
        this.line('_ns.' + member.name + ' = ' + (member.init ? this.genExpr(member.init) : 'undefined') + ';');
      } else {
        this.emitStmt(member);
      }
    }
    this.close();
    this.line('})();');
  };

  // ── Enum ────────────────────────────────────────────────────────────────────
  CodeGen.prototype.emitEnum = function (stmt) {
    this.line('var ' + stmt.name + ' = {');
    this.open();
    for (var i = 0; i < stmt.values.length; i++) {
      var trailing = i < stmt.values.length - 1 ? ',' : '';
      this.line(stmt.values[i] + ': ' + i + trailing);
    }
    this.close();
    this.line('};');
  };

  // ── Try-Catch ───────────────────────────────────────────────────────────────
  CodeGen.prototype.emitTryCatch = function (stmt) {
    this.line('try {');
    this.open();
    this.emitStatements(stmt.tryBody);
    this.close();
    if (stmt.catchBody && stmt.catchBody.length > 0) {
      this.line('} catch (' + (stmt.catchVar || 'e') + ') {');
      this.open();
      this.emitStatements(stmt.catchBody);
      this.close();
    }
    if (stmt.finallyBody && stmt.finallyBody.length > 0) {
      this.line('} finally {');
      this.open();
      this.emitStatements(stmt.finallyBody);
      this.close();
    }
    this.line('}');
  };

  // ── Print Statement ─────────────────────────────────────────────────────────
  CodeGen.prototype.emitPrint = function (stmt) {
    var args = stmt.args.map(function(a) { return this.genExpr(a); }.bind(this)).join(', ');
    this.line('__print(' + args + ');');
  };

  // ── Input Statement: ادخل(متغير، ...) → قراءة من المستخدم ──────────────────
  CodeGen.prototype.emitInput = function (stmt) {
    var self = this;
    stmt.varName.forEach(function (name) {
      self.line(name + ' = __input();');
    });
  };

  // ═══════════════════════════════════════════════════════════════════════════════
  // EXPRESSION CODEGEN
  // ═══════════════════════════════════════════════════════════════════════════════

  CodeGen.prototype.genExpr = function (expr) {
    if (!expr) return 'undefined';

    switch (expr.type) {
      case 'NumberExpr':
        return String(expr.value);

      case 'StringExpr':
        return '"' + this.escapeString(expr.value) + '"';

      case 'BoolExpr':
        return expr.value ? 'true' : 'false';

      case 'NullExpr':
        return 'null';

      case 'VariableExpr':
        if (this.classFields && this.classFields.indexOf(expr.name) !== -1) {
          return 'this.' + expr.name;
        }
        if (expr.name === 'س') return 'x';
        if (expr.name === 'ص') return 'y';
        return expr.name;

      case 'Assignment':
        var aName = expr.name;
        if (this.classFields && this.classFields.indexOf(aName) !== -1) {
          aName = 'this.' + aName;
        }
        if (aName === 'س') aName = 'x';
        if (aName === 'ص') aName = 'y';
        return '(' + aName + ' = ' + this.genExpr(expr.value) + ')';

      case 'CompoundAssignment':
        var cName = expr.name;
        if (this.classFields && this.classFields.indexOf(cName) !== -1) {
          cName = 'this.' + cName;
        }
        if (cName === 'س') cName = 'x';
        if (cName === 'ص') cName = 'y';
        return '(' + cName + ' ' + expr.op + ' ' + this.genExpr(expr.value) + ')';

      case 'BinaryExpr':
        // Power operator → Math.pow()
        if (expr.op === '^') {
          return 'Math.pow(' + this.genExpr(expr.lhs) + ', ' + this.genExpr(expr.rhs) + ')';
        }
        // Arabic logical operators
        var op = expr.op;
        if (op === 'و') op = '&&';
        if (op === 'أو') op = '||';
        return '(' + this.genExpr(expr.lhs) + ' ' + op + ' ' + this.genExpr(expr.rhs) + ')';

      case 'UnaryExpr':
        // Address-of and pointer deref are no-ops in JS
        if (expr.op === '&' || expr.op === '*') {
          return this.genExpr(expr.operand);
        }
        if (expr.op === '!' || expr.op === '-' || expr.op === '++' || expr.op === '--') {
          return '(' + expr.op + this.genExpr(expr.operand) + ')';
        }
        return '(' + this.genExpr(expr.operand) + expr.op + ')';

      case 'FunctionCall':
        // Built-in: طول(x) → x.length
        if (expr.name === 'طول' || expr.name === 'حجم') {
          return this.genExpr(expr.args[0]) + '.length';
        }
        if (expr.name && typeof expr.name === 'object' && expr.name.type === 'MemberAccessExpr') {
          var obj = this.genExpr(expr.name.object);
          var member = expr.name.member;
          // Arabic method translation
          var methodMap = {
            'احذف': 'splice',
            'أضف': 'push',
            'يحتوي': 'includes',
            'يبدأ': 'startsWith',
            'ينتهي': 'endsWith',
            'قص من': 'slice',
            'قص_من': 'slice',
            'استبدل': 'replace',
            'تقسيم': 'split',
            'مزج': 'concat',
            'لأحرف_كبيرة': 'toUpperCase',
            'لأحرف_صغيرة': 'toLowerCase',
            'كرر': 'repeat',
            'اتجه': 'trim',
            'ابحث': 'indexOf',
            'استخرج': 'substring',
            'احترس': 'charAt',
            'طول': 'length',
            'حجم': 'length'
          };
          if (methodMap[member]) {
            // Special handling for properties (not methods)
            if (member === 'طول' || member === 'حجم') {
              return obj + '.' + methodMap[member];
            }
            // Special handling for methods that need argument transformation
            if (member === 'احذف') {
              // احذف(index) → splice(index, 1)
              return obj + '.' + methodMap[member] + '(' + this.genExprList(expr.args) + ', 1)';
            }
            if (member === 'استبدل') {
              // استبدل(old, new) → replace(old, new)
              return obj + '.' + methodMap[member] + '(' + this.genExprList(expr.args) + ')';
            }
            if (member === 'تقسيم') {
              // تقسيم(sep) → split(sep)
              return obj + '.' + methodMap[member] + '(' + this.genExprList(expr.args) + ')';
            }
            return obj + '.' + methodMap[member] + '(' + this.genExprList(expr.args) + ')';
          }
          return obj + '.' + member + '(' + this.genExprList(expr.args) + ')';
        }
        if (expr.name && typeof expr.name === 'object') {
          // Method call via expression
          return this.genExpr(expr.name) + '(' + this.genExprList(expr.args) + ')';
        }
        return expr.name + '(' + this.genExprList(expr.args) + ')';

      case 'MemberAccessExpr':
        var objStr = this.genExpr(expr.object);
        // Arabic property translation
        var propMap = {
          'طول': 'length',
          'حجم': 'length'
        };
        if (propMap[expr.member]) {
          return objStr + '.' + propMap[expr.member];
        }
        return objStr + '.' + expr.member;

      case 'ArraySubscriptExpr':
        return expr.name + '[' + this.genExpr(expr.index) + ']';

      case 'NewExpr':
        return 'new ' + expr.typeName + '(' + this.genExprList(expr.args) + ')';

      case 'SizeofExpr':
        return '(' + this.genExpr(expr.expr) + ').length';

      case 'TypeofExpr':
        return 'typeof(' + this.genExpr(expr.expr) + ')';

      case 'TernaryExpr':
        return '(' + this.genExpr(expr.condition) + ' ? ' + this.genExpr(expr.trueExpr) + ' : ' + this.genExpr(expr.falseExpr) + ')';

      case 'ExprStmt':
        return this.genExpr(expr.expr);

      case 'Program':
        // Shouldn't appear as expression, but handle gracefully
        return '(function() { ' + expr.body.map(function(s) { return this.genExpr(s); }.bind(this)).join('; ') + ' })()';

      default:
        return '/* unknown: ' + expr.type + ' */';
    }
  };

  CodeGen.prototype.genExprList = function (args) {
    if (!args || args.length === 0) return '';
    return args.map(function(a) { return this.genExpr(a); }.bind(this)).join(', ');
  };

  CodeGen.prototype.escapeString = function (s) {
    return s.replace(/\\/g, '\\\\').replace(/"/g, '\\"').replace(/\n/g, '\\n').replace(/\t/g, '\\t');
  };

  // ── Image Processing CodeGen (20) ──────────────────────────────────────────

  // حمّل_صورة("path", varName);
  CodeGen.prototype.emitLoadImage = function (stmt) {
    this.line(stmt.varName + ' = __dhad loadImage(' + JSON.stringify(stmt.path) + ');');
  };

  // ارسم_صورة(imgVar, x, y, w, h);
  CodeGen.prototype.emitDrawImage = function (stmt) {
    this.line('__dhad drawImage(' + stmt.imgVar + ', ' + this.genExpr(stmt.x) + ', ' + this.genExpr(stmt.y) + ', ' + this.genExpr(stmt.w) + ', ' + this.genExpr(stmt.h) + ');');
  };

  // حجم_صورة(imgVar);
  CodeGen.prototype.emitImageSize = function (stmt) {
    this.line('__dhad imageSize(' + stmt.imgVar + ');');
  };

  // احفظ_صورة(imgVar, "path");
  CodeGen.prototype.emitSaveImage = function (stmt) {
    this.line('__dhad saveImage(' + stmt.imgVar + ', ' + JSON.stringify(stmt.path) + ');');
  };

  // قص_صورة(imgVar, x, y, w, h);
  CodeGen.prototype.emitCropImage = function (stmt) {
    this.line(stmt.imgVar + ' = __dhad cropImage(' + stmt.imgVar + ', ' + this.genExpr(stmt.x) + ', ' + this.genExpr(stmt.y) + ', ' + this.genExpr(stmt.w) + ', ' + this.genExpr(stmt.h) + ');');
  };

  // غيّر_حجم(imgVar, w, h);
  CodeGen.prototype.emitResize = function (stmt) {
    this.line(stmt.imgVar + ' = __dhad resize(' + stmt.imgVar + ', ' + this.genExpr(stmt.w) + ', ' + this.genExpr(stmt.h) + ');');
  };

  // لف_صورة(imgVar, angle);
  CodeGen.prototype.emitRotateImage = function (stmt) {
    this.line(stmt.imgVar + ' = __dhad rotateImage(' + stmt.imgVar + ', ' + this.genExpr(stmt.angle) + ');');
  };

  // قلب_صورة(imgVar, "direction");
  CodeGen.prototype.emitFlipImage = function (stmt) {
    this.line(stmt.imgVar + ' = __dhad flipImage(' + stmt.imgVar + ', ' + JSON.stringify(stmt.direction) + ');');
  };

  // شفافية(imgVar, value);
  CodeGen.prototype.emitOpacity = function (stmt) {
    this.line('__dhad opacity(' + stmt.imgVar + ', ' + this.genExpr(stmt.value) + ');');
  };

  // فلتر(imgVar, "filterName");
  CodeGen.prototype.emitFilter = function (stmt) {
    this.line(stmt.imgVar + ' = __dhad filter(' + stmt.imgVar + ', ' + JSON.stringify(stmt.filterName) + ');');
  };

  //تراكب(imgVar1, imgVar2, x, y);
  CodeGen.prototype.emitOverlay = function (stmt) {
    this.line('__dhad overlay(' + stmt.imgVar1 + ', ' + stmt.imgVar2 + ', ' + this.genExpr(stmt.x) + ', ' + this.genExpr(stmt.y) + ');');
  };

  // خلفية(imgVar, bgVar);
  CodeGen.prototype.emitBackground = function (stmt) {
    this.line('__dhad background(' + stmt.imgVar + ', ' + stmt.bgVar + ');');
  };

  //بكسل(imgVar, x, y);
  CodeGen.prototype.emitPixel = function (stmt) {
    this.line('__dhad pixel(' + stmt.imgVar + ', ' + this.genExpr(stmt.x) + ', ' + this.genExpr(stmt.y) + ');');
  };

  // ارسم("shape", args...);
  CodeGen.prototype.emitDraw = function (stmt) {
    this.line('__dhad.draw(' + JSON.stringify(stmt.shape) + ', ' + this.genExprList(stmt.args) + ');');
  };

  // ملء("shape", args...);
  CodeGen.prototype.emitFill = function (stmt) {
    this.line('__dhad.fill(' + JSON.stringify(stmt.shape) + ', ' + this.genExprList(stmt.args) + ');');
  };

  // مستطيل(x, y, w, h);
  CodeGen.prototype.emitRectangle = function (stmt) {
    this.line('__dhad rectangle(' + this.genExpr(stmt.x) + ', ' + this.genExpr(stmt.y) + ', ' + this.genExpr(stmt.w) + ', ' + this.genExpr(stmt.h) + ');');
  };

  // دائرة(x, y, r);
  CodeGen.prototype.emitCircle = function (stmt) {
    this.line('__dhad circle(' + this.genExpr(stmt.x) + ', ' + this.genExpr(stmt.y) + ', ' + this.genExpr(stmt.r) + ');');
  };

  // خط(x1, y1, x2, y2);
  CodeGen.prototype.emitLine = function (stmt) {
    this.line('__dhad line(' + this.genExpr(stmt.x1) + ', ' + this.genExpr(stmt.y1) + ', ' + this.genExpr(stmt.x2) + ', ' + this.genExpr(stmt.y2) + ');');
  };

  // نص_على_لوحة("text", x, y, fontSize);
  CodeGen.prototype.emitTextOnCanvas = function (stmt) {
    this.line('__dhad.text(' + this.genExpr(stmt.text) + ', ' + this.genExpr(stmt.x) + ', ' + this.genExpr(stmt.y) + ', ' + this.genExpr(stmt.fontSize) + ');');
  };

  // مسح();
  CodeGen.prototype.emitClearCanvas = function () {
    this.line('__dhad clear();');
  };

  // ── Exports ─────────────────────────────────────────────────────────────────
  return {
    CodeGen: CodeGen
  };
})();

if (typeof module !== 'undefined' && module.exports) {
  module.exports = DhadCodeGen;
}

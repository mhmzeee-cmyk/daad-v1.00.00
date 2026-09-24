// ═══════════════════════════════════════════════════════════════════════════════
// Dhad (ض) Language — AST Node Definitions
// Matches C++ compiler's 32 node types
// ═══════════════════════════════════════════════════════════════════════════════

var DhadAST = (function () {
  'use strict';

  // ── Base Classes ────────────────────────────────────────────────────────────
  function ASTNode(line, col) {
    this.line = line || 0;
    this.col = col || 0;
  }

  function ExprAST(line, col) { ASTNode.call(this, line, col); }
  ExprAST.prototype = Object.create(ASTNode.prototype);
  ExprAST.prototype.constructor = ExprAST;

  function StmtAST(line, col) { ASTNode.call(this, line, col); }
  StmtAST.prototype = Object.create(ASTNode.prototype);
  StmtAST.prototype.constructor = StmtAST;

  // ── Expressions ─────────────────────────────────────────────────────────────

  function NumberExprAST(value, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'NumberExpr';
    this.value = value;
  }
  NumberExprAST.prototype = Object.create(ExprAST.prototype);

  function StringExprAST(value, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'StringExpr';
    this.value = value;
  }
  StringExprAST.prototype = Object.create(ExprAST.prototype);

  function BoolExprAST(value, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'BoolExpr';
    this.value = value;
  }
  BoolExprAST.prototype = Object.create(ExprAST.prototype);

  function NullExprAST(line, col) {
    ExprAST.call(this, line, col);
    this.type = 'NullExpr';
  }
  NullExprAST.prototype = Object.create(ExprAST.prototype);

  function VariableExprAST(name, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'VariableExpr';
    this.name = name;
  }
  VariableExprAST.prototype = Object.create(ExprAST.prototype);

  function BinaryExprAST(op, lhs, rhs, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'BinaryExpr';
    this.op = op;
    this.lhs = lhs;
    this.rhs = rhs;
  }
  BinaryExprAST.prototype = Object.create(ExprAST.prototype);

  function UnaryExprAST(op, operand, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'UnaryExpr';
    this.op = op;
    this.operand = operand;
  }
  UnaryExprAST.prototype = Object.create(ExprAST.prototype);

  function FunctionCallAST(name, args, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'FunctionCall';
    this.name = name;
    this.args = args;
  }
  FunctionCallAST.prototype = Object.create(ExprAST.prototype);

  function MemberAccessExprAST(object, member, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'MemberAccessExpr';
    this.object = object;
    this.member = member;
  }
  MemberAccessExprAST.prototype = Object.create(ExprAST.prototype);

  function ArraySubscriptExprAST(name, index, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'ArraySubscriptExpr';
    this.name = name;
    this.index = index;
  }
  ArraySubscriptExprAST.prototype = Object.create(ExprAST.prototype);

  function NewExprAST(typeName, args, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'NewExpr';
    this.typeName = typeName;
    this.args = args;
  }
  NewExprAST.prototype = Object.create(ExprAST.prototype);

  function SizeofExprAST(expr, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'SizeofExpr';
    this.expr = expr;
  }
  SizeofExprAST.prototype = Object.create(ExprAST.prototype);

  function TypeofExprAST(expr, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'TypeofExpr';
    this.expr = expr;
  }
  TypeofExprAST.prototype = Object.create(ExprAST.prototype);

  function TernaryExprAST(condition, trueExpr, falseExpr, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'TernaryExpr';
    this.condition = condition;
    this.trueExpr = trueExpr;
    this.falseExpr = falseExpr;
  }
  TernaryExprAST.prototype = Object.create(ExprAST.prototype);

  // ── Statements ──────────────────────────────────────────────────────────────

  function VarDeclStmtAST(type, name, init, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'VarDecl';
    this.varType = type;
    this.name = name;
    this.init = init; // can be null
    this.access = 'public'; // public/private/protected
    this.isStatic = false;
  }
  VarDeclStmtAST.prototype = Object.create(StmtAST.prototype);

  function AssignmentAST(name, value, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Assignment';
    this.name = name;
    this.value = value;
  }
  AssignmentAST.prototype = Object.create(StmtAST.prototype);

  function CompoundAssignmentAST(name, op, value, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'CompoundAssignment';
    this.name = name;
    this.op = op;
    this.value = value;
  }
  CompoundAssignmentAST.prototype = Object.create(StmtAST.prototype);

  function ExprStmtAST(expr, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'ExprStmt';
    this.expr = expr;
  }
  ExprStmtAST.prototype = Object.create(StmtAST.prototype);

  function IfStmtAST(condition, thenBody, elseBody, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'IfStmt';
    this.condition = condition;
    this.thenBody = thenBody;
    this.elseBody = elseBody;
  }
  IfStmtAST.prototype = Object.create(StmtAST.prototype);

  function WhileStmtAST(condition, body, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'WhileStmt';
    this.condition = condition;
    this.body = body;
  }
  WhileStmtAST.prototype = Object.create(StmtAST.prototype);

  function ForStmtAST(init, condition, update, body, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'ForStmt';
    this.init = init;
    this.condition = condition;
    this.update = update;
    this.body = body;
  }
  ForStmtAST.prototype = Object.create(StmtAST.prototype);

  function DoWhileStmtAST(condition, body, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'DoWhileStmt';
    this.condition = condition;
    this.body = body;
  }
  DoWhileStmtAST.prototype = Object.create(StmtAST.prototype);

  function ForEachStmtAST(varType, varName, iterable, body, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'ForEachStmt';
    this.varType = varType;
    this.varName = varName;
    this.iterable = iterable;
    this.body = body;
  }
  ForEachStmtAST.prototype = Object.create(StmtAST.prototype);

  function SwitchCaseAST(value, body) {
    this.value = value;
    this.body = body;
  }

  function SwitchStmtAST(expression, cases, defaultBody, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'SwitchStmt';
    this.expression = expression;
    this.cases = cases;
    this.defaultBody = defaultBody;
  }
  SwitchStmtAST.prototype = Object.create(StmtAST.prototype);

  function ReturnStmtAST(value, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'ReturnStmt';
    this.value = value;
  }
  ReturnStmtAST.prototype = Object.create(StmtAST.prototype);

  function BreakStmtAST(line, col) {
    StmtAST.call(this, line, col);
    this.type = 'BreakStmt';
  }
  BreakStmtAST.prototype = Object.create(StmtAST.prototype);

  function ContinueStmtAST(line, col) {
    StmtAST.call(this, line, col);
    this.type = 'ContinueStmt';
  }
  ContinueStmtAST.prototype = Object.create(StmtAST.prototype);

  function FunctionDeclAST(returnType, name, params, body, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'FunctionDecl';
    this.returnType = returnType;
    this.name = name;
    this.params = params; // [{type, name}]
    this.body = body;
    this.access = 'public';
    this.isStatic = false;
  }
  FunctionDeclAST.prototype = Object.create(StmtAST.prototype);

  function ConstructorDeclAST(params, body, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'ConstructorDecl';
    this.params = params;
    this.body = body;
    this.access = 'public';
  }
  ConstructorDeclAST.prototype = Object.create(StmtAST.prototype);

  function ClassDeclAST(name, body, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'ClassDecl';
    this.name = name;
    this.body = body;
  }
  ClassDeclAST.prototype = Object.create(StmtAST.prototype);

  function StructDeclAST(name, body, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'StructDecl';
    this.name = name;
    this.body = body;
  }
  StructDeclAST.prototype = Object.create(StmtAST.prototype);

  function NamespaceDeclAST(name, body, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'NamespaceDecl';
    this.name = name;
    this.body = body;
  }
  NamespaceDeclAST.prototype = Object.create(StmtAST.prototype);

  function EnumDeclAST(name, values, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'EnumDecl';
    this.name = name;
    this.values = values;
  }
  EnumDeclAST.prototype = Object.create(StmtAST.prototype);

  function TryCatchStmtAST(tryBody, catchVar, catchBody, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'TryCatchStmt';
    this.tryBody = tryBody;
    this.catchVar = catchVar;
    this.catchBody = catchBody;
  }
  TryCatchStmtAST.prototype = Object.create(StmtAST.prototype);

  function ThrowStmtAST(value, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'ThrowStmt';
    this.value = value;
  }
  ThrowStmtAST.prototype = Object.create(StmtAST.prototype);

  function DeleteStmtAST(operand, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'DeleteStmt';
    this.operand = operand;
  }
  DeleteStmtAST.prototype = Object.create(StmtAST.prototype);

  function TemplateDeclAST(name, params, body, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'TemplateDecl';
    this.name = name;
    this.params = params;
    this.body = body;
  }
  TemplateDeclAST.prototype = Object.create(StmtAST.prototype);

  function MemberAssignmentAST(object, member, value, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'MemberAssignment';
    this.object = object;
    this.member = member;
    this.value = value;
  }
  MemberAssignmentAST.prototype = Object.create(StmtAST.prototype);

  function ArraySubscriptAssignAST(name, index, value, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'ArraySubscriptAssign';
    this.name = name;
    this.index = index;
    this.value = value;
  }
  ArraySubscriptAssignAST.prototype = Object.create(StmtAST.prototype);

  function PrintStmtAST(args, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'PrintStmt';
    this.args = args;
  }
  PrintStmtAST.prototype = Object.create(StmtAST.prototype);

  // ── Input Statement: ادخل(متغير) ────────────────────────────────────────────
  function InputStmtAST(varName, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'InputStmt';
    this.varName = varName;
  }
  InputStmtAST.prototype = Object.create(StmtAST.prototype);

  // ── Program Root ────────────────────────────────────────────────────────────
  function ProgramAST(body, line, col) {
    ASTNode.call(this, line, col);
    this.type = 'Program';
    this.body = body;
  }
  ProgramAST.prototype = Object.create(ASTNode.prototype);

  function ImportStmtAST(path, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Import';
    this.path = path;
  }
  ImportStmtAST.prototype = Object.create(StmtAST.prototype);

  function ExportStmtAST(names, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Export';
    this.names = names;
  }
  ExportStmtAST.prototype = Object.create(StmtAST.prototype);

  // ── Image Processing AST Nodes (20) ────────────────────────────────────────
  function LoadImageAST(path, varName, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'LoadImage';
    this.path = path;
    this.varName = varName;
  }
  LoadImageAST.prototype = Object.create(StmtAST.prototype);

  function DrawImageAST(imgVar, x, y, w, h, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'DrawImage';
    this.imgVar = imgVar;
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
  }
  DrawImageAST.prototype = Object.create(StmtAST.prototype);

  function ImageSizeAST(imgVar, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'ImageSize';
    this.imgVar = imgVar;
  }
  ImageSizeAST.prototype = Object.create(ExprAST.prototype);

  function SaveImageAST(imgVar, path, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'SaveImage';
    this.imgVar = imgVar;
    this.path = path;
  }
  SaveImageAST.prototype = Object.create(StmtAST.prototype);

  function CropImageAST(imgVar, x, y, w, h, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'CropImage';
    this.imgVar = imgVar;
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
  }
  CropImageAST.prototype = Object.create(StmtAST.prototype);

  function ResizeAST(imgVar, w, h, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Resize';
    this.imgVar = imgVar;
    this.w = w;
    this.h = h;
  }
  ResizeAST.prototype = Object.create(StmtAST.prototype);

  function RotateImageAST(imgVar, angle, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'RotateImage';
    this.imgVar = imgVar;
    this.angle = angle;
  }
  RotateImageAST.prototype = Object.create(StmtAST.prototype);

  function FlipImageAST(imgVar, direction, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'FlipImage';
    this.imgVar = imgVar;
    this.direction = direction;
  }
  FlipImageAST.prototype = Object.create(StmtAST.prototype);

  function OpacityAST(imgVar, value, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Opacity';
    this.imgVar = imgVar;
    this.value = value;
  }
  OpacityAST.prototype = Object.create(StmtAST.prototype);

  function FilterAST(imgVar, filterName, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Filter';
    this.imgVar = imgVar;
    this.filterName = filterName;
  }
  FilterAST.prototype = Object.create(StmtAST.prototype);

  function OverlayAST(imgVar1, imgVar2, x, y, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Overlay';
    this.imgVar1 = imgVar1;
    this.imgVar2 = imgVar2;
    this.x = x;
    this.y = y;
  }
  OverlayAST.prototype = Object.create(StmtAST.prototype);

  function BackgroundAST(imgVar, bgVar, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Background';
    this.imgVar = imgVar;
    this.bgVar = bgVar;
  }
  BackgroundAST.prototype = Object.create(StmtAST.prototype);

  function PixelAST(imgVar, x, y, line, col) {
    ExprAST.call(this, line, col);
    this.type = 'Pixel';
    this.imgVar = imgVar;
    this.x = x;
    this.y = y;
  }
  PixelAST.prototype = Object.create(ExprAST.prototype);

  function DrawAST(shape, args, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Draw';
    this.shape = shape;
    this.args = args;
  }
  DrawAST.prototype = Object.create(StmtAST.prototype);

  function FillAST(shape, args, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Fill';
    this.shape = shape;
    this.args = args;
  }
  FillAST.prototype = Object.create(StmtAST.prototype);

  function RectangleAST(x, y, w, h, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Rectangle';
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
  }
  RectangleAST.prototype = Object.create(StmtAST.prototype);

  function CircleAST(x, y, r, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Circle';
    this.x = x;
    this.y = y;
    this.r = r;
  }
  CircleAST.prototype = Object.create(StmtAST.prototype);

  function LineAST(x1, y1, x2, y2, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Line';
    this.x1 = x1;
    this.y1 = y1;
    this.x2 = x2;
    this.y2 = y2;
  }
  LineAST.prototype = Object.create(StmtAST.prototype);

  function TextOnCanvasAST(text, x, y, fontSize, line, col) {
    StmtAST.call(this, line, col);
    this.type = 'TextOnCanvas';
    this.text = text;
    this.x = x;
    this.y = y;
    this.fontSize = fontSize;
  }
  TextOnCanvasAST.prototype = Object.create(StmtAST.prototype);

  function ClearAST(line, col) {
    StmtAST.call(this, line, col);
    this.type = 'Clear';
  }
  ClearAST.prototype = Object.create(StmtAST.prototype);

  // ── Exports ─────────────────────────────────────────────────────────────────
  return {
    ASTNode: ASTNode,
    ExprAST: ExprAST,
    StmtAST: StmtAST,
    NumberExprAST: NumberExprAST,
    StringExprAST: StringExprAST,
    BoolExprAST: BoolExprAST,
    NullExprAST: NullExprAST,
    VariableExprAST: VariableExprAST,
    BinaryExprAST: BinaryExprAST,
    UnaryExprAST: UnaryExprAST,
    FunctionCallAST: FunctionCallAST,
    MemberAccessExprAST: MemberAccessExprAST,
    ArraySubscriptExprAST: ArraySubscriptExprAST,
    NewExprAST: NewExprAST,
    SizeofExprAST: SizeofExprAST,
    TypeofExprAST: TypeofExprAST,
    TernaryExprAST: TernaryExprAST,
    VarDeclStmtAST: VarDeclStmtAST,
    AssignmentAST: AssignmentAST,
    CompoundAssignmentAST: CompoundAssignmentAST,
    ExprStmtAST: ExprStmtAST,
    IfStmtAST: IfStmtAST,
    WhileStmtAST: WhileStmtAST,
    ForStmtAST: ForStmtAST,
    DoWhileStmtAST: DoWhileStmtAST,
    ForEachStmtAST: ForEachStmtAST,
    SwitchCaseAST: SwitchCaseAST,
    SwitchStmtAST: SwitchStmtAST,
    ReturnStmtAST: ReturnStmtAST,
    BreakStmtAST: BreakStmtAST,
    ContinueStmtAST: ContinueStmtAST,
    FunctionDeclAST: FunctionDeclAST,
    ConstructorDeclAST: ConstructorDeclAST,
    ClassDeclAST: ClassDeclAST,
    StructDeclAST: StructDeclAST,
    NamespaceDeclAST: NamespaceDeclAST,
    EnumDeclAST: EnumDeclAST,
    TryCatchStmtAST: TryCatchStmtAST,
    ThrowStmtAST: ThrowStmtAST,
    DeleteStmtAST: DeleteStmtAST,
    TemplateDeclAST: TemplateDeclAST,
    MemberAssignmentAST: MemberAssignmentAST,
    ArraySubscriptAssignAST: ArraySubscriptAssignAST,
    PrintStmtAST: PrintStmtAST,
    InputStmtAST: InputStmtAST,
    ProgramAST: ProgramAST,
    ImportStmtAST: ImportStmtAST,
    ExportStmtAST: ExportStmtAST,

    // Image Processing AST Nodes (20)
    LoadImageAST: LoadImageAST,
    DrawImageAST: DrawImageAST,
    ImageSizeAST: ImageSizeAST,
    SaveImageAST: SaveImageAST,
    CropImageAST: CropImageAST,
    ResizeAST: ResizeAST,
    RotateImageAST: RotateImageAST,
    FlipImageAST: FlipImageAST,
    OpacityAST: OpacityAST,
    FilterAST: FilterAST,
    OverlayAST: OverlayAST,
    BackgroundAST: BackgroundAST,
    PixelAST: PixelAST,
    DrawAST: DrawAST,
    FillAST: FillAST,
    RectangleAST: RectangleAST,
    CircleAST: CircleAST,
    LineAST: LineAST,
    TextOnCanvasAST: TextOnCanvasAST,
    ClearAST: ClearAST
  };
})();

if (typeof module !== 'undefined' && module.exports) {
  module.exports = DhadAST;
}

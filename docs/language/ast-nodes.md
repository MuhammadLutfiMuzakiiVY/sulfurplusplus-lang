# Sulfur++ AST Specification & Node Reference v0.1

This document provides the formal mapping between the **EBNF Grammar Rules**, the **C++ AST Data Structures** (`include/ast.hpp`), and their semantic representations.

---

## 1. Expression Nodes (`include/ast.hpp: struct Expr`)

`Expr` is defined as a `std::variant` over 24 distinct expression nodes:

| # | AST Struct | EBNF Rule | Fields / Layout | Description |
|---|------------|-----------|-----------------|-------------|
| 1 | `IntLitExpr` | `integer_literal` | `int64_t value`, `int line` | 64-bit integer literal |
| 2 | `FloatLitExpr` | `float_literal` | `double value`, `int line` | 64-bit float literal |
| 3 | `BoolLitExpr` | `boolean_literal` | `bool value`, `int line` | Boolean `true` or `false` |
| 4 | `NullLitExpr` | `null_literal` | `int line` | Null value |
| 5 | `StringLitExpr` | `string_literal` | `std::string value`, `int line` | UTF-8 string literal |
| 6 | `CharLitExpr` | `char_literal` | `char value`, `int line` | Single character literal |
| 7 | `PSStringExpr` | `ps_string_literal` | `std::vector<PSSegment> segments`, `int line` | Process String with interpolation (`ps"..."`) |
| 8 | `IdentExpr` | `IDENTIFIER` | `std::string name`, `int line` | Identifier symbol reference |
| 9 | `BinaryExpr` | `binary_expression` | `std::string op`, `ExprPtr left, right`, `int line` | Binary operations (`+`, `-`, `*`, `==`, `&&`, etc.) |
| 10 | `UnaryExpr` | `unary_expression` | `std::string op`, `ExprPtr operand`, `int line` | Prefix unary operations (`!`, `-`, `~`) |
| 11 | `AssignExpr` | `assignment_expression` | `ExprPtr target`, `std::string op`, `ExprPtr value`, `int line` | Assignment (`=`, `+=`, `-=`, `*=`, `/=`) |
| 12 | `CallExpr` | `call_suffix` | `ExprPtr callee`, `std::vector<ExprPtr> args`, `int line` | Function or method call |
| 13 | `IndexExpr` | `index_suffix` | `ExprPtr object`, `ExprPtr index`, `int line` | Collection indexing `obj[idx]` |
| 14 | `MemberExpr` | `member_access` | `ExprPtr object`, `std::string member`, `bool safe`, `std::string op`, `int line` | Property/method access (`.`, `?.`, `::`, `->`) |
| 15 | `PipelineExpr` | `pipeline_expression` | `ExprPtr left`, `ExprPtr right`, `int line` | Pipeline operation `left -> right` |
| 16 | `NullCoalExpr` | `null_coalesce_expression` | `ExprPtr left`, `ExprPtr right`, `int line` | Null-coalescing `left ?? right` |
| 17 | `ListLitExpr` | `list_literal` | `std::vector<ExprPtr> elements`, `int line` | List literal `[1, 2, 3]` |
| 18 | `DictLitExpr` | `dict_literal` | `std::vector<std::pair<ExprPtr, ExprPtr>> pairs`, `int line` | Dictionary literal `{"k": v}` |
| 19 | `LambdaExpr` | `lambda_expression` | `vector<pair<string, string>> params`, `string retType`, `StmtPtr body`, `int line` | Anonymous closure `fn(x) { ... }` |
| 20 | `NewExpr` | `new_expression` | `std::string className`, `std::vector<ExprPtr> args`, `int line` | Heap instantiation `new Class(args)` |
| 21 | `TernaryExpr` | `ternary_expression` | `ExprPtr cond, thenExpr, elseExpr`, `int line` | Ternary conditional `cond ? then : else` |
| 22 | `AddrOfExpr` | `unary_expression (&)` | `ExprPtr operand`, `int line` | Address-of operator `&x` in `unsafe` |
| 23 | `DerefExpr` | `unary_expression (*)` | `ExprPtr operand`, `int line` | Pointer dereference `*p` in `unsafe` |
| 24 | `DeleteExpr` | `delete_expression` | `ExprPtr operand`, `int line` | Manual memory release `delete p` |

---

## 2. Statement Nodes (`include/ast.hpp: struct Stmt`)

`Stmt` is defined as a `std::variant` over 23 statement and declaration nodes:

| # | AST Struct | EBNF Rule | Fields / Layout | Description |
|---|------------|-----------|-----------------|-------------|
| 1 | `VarDeclStmt` | `variable_declaration` | `string keyword`, `string name`, `string type`, `ExprPtr initializer`, `int line` | Variable declaration (`let`, `var`, `auto`) |
| 2 | `FnDeclStmt` | `function_declaration` | `string name`, `vector<pair<string,string>> params`, `string retType`, `StmtPtr body`, `bool isMethod`, `int line` | Function or method declaration |
| 3 | `ReturnStmt` | `return_statement` | `ExprPtr value`, `int line` | Return statement |
| 4 | `BreakStmt` | `break_statement` | `int line` | Loop break |
| 5 | `ContinueStmt` | `continue_statement` | `int line` | Loop continue |
| 6 | `BlockStmt` | `block` | `std::vector<StmtPtr> stmts`, `int line` | Compound statement block `{ ... }` |
| 7 | `IfStmt` | `if_statement` | `ExprPtr cond`, `StmtPtr thenBranch`, `StmtPtr elseBranch`, `int line` | Conditional branching |
| 8 | `WhileStmt` | `while_statement` | `ExprPtr cond`, `StmtPtr body`, `int line` | While loop |
| 9 | `ForStmt` | `for_in_statement` | `std::string var`, `ExprPtr iterable`, `StmtPtr body`, `int line` | For-in loop over iterable |
| 10 | `ForCStyleStmt` | `for_c_style_statement` | `StmtPtr init`, `ExprPtr cond`, `ExprPtr post`, `StmtPtr body`, `int line` | C-style 3-clause for loop |
| 11 | `ClassDeclStmt` | `class_declaration` | `string name`, `vector<string> interfaces`, `vector<StmtPtr> members`, `vector<pair<int,string>> ctorOrder, dtorOrder`, `int line` | Class definition with lifecycle orders |
| 12 | `StructDeclStmt` | `struct_declaration` | `string name`, `vector<pair<string, string>> fields`, `int line` | Struct definition |
| 13 | `InterfaceDeclStmt` | `interface_declaration` | `string name`, `vector<FnDeclStmt> methods`, `int line` | Interface definition |
| 14 | `ExprStmt` | `expression_statement` | `ExprPtr expr`, `int line` | Expression evaluated for side-effects |
| 15 | `StreamOutStmt` | `stream_out_statement` | `ExprPtr target`, `ExprPtr value`, `int line` | Terminal stream output (`Terminal.Out << ...`) |
| 16 | `ImportStmt` | `import_declaration` | `string pkg`, `string alias`, `vector<string> flags`, `int line` | Module import statement |
| 17 | `ExportStmt` | `export_declaration` | `string alias`, `int line` | Module export declaration |
| 18 | `ExposeStmt` | `expose_declaration` | `string name`, `string alias`, `int line` | Native FFI symbol exposure |
| 19 | `OverwriteStmt` | `overwrite_statement` | `string target`, `ExprPtr value`, `int line` | Dynamic symbol/config overwrite |
| 20 | `UnsafeStmt` | `unsafe_statement` | `StmtPtr body`, `int line` | Unsafe execution block |
| 21 | `DeferStmt` | `defer_statement` | `StmtPtr body`, `int line` | Deferred execution block |
| 22 | `TryCatchStmt` | `try_statement` | `StmtPtr tryBody`, `string catchVar`, `StmtPtr catchBody`, `StmtPtr finallyBody`, `int line` | Exception handling block |
| 23 | `ThrowStmt` | `throw_statement` | `ExprPtr value`, `int line` | Exception throw statement |
| 24 | `MatchStmt` | `match_statement` | `ExprPtr value`, `vector<MatchCase> cases`, `int line` | Pattern match statement |

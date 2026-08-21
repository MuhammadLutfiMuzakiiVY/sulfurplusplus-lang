# Sulfur++ Abstract Syntax Tree Specification (AST_SPEC.md)

**Specification Version:** 0.1-AST (Component 146)  
**Status:** Official AST Node Specification  
**Target File:** [`include/ast.hpp`](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/include/ast.hpp)

---

## 146. AST Architecture & Node Types

AST (Abstract Syntax Tree) adalah struktur data pohon yang dihasilkan oleh Parser dari Token Stream. AST merepresentasikan struktur hirarkis program tanpa noise sintaksis (seperti tanda kurung atau titik koma).

```
                  ProgramStmt (Root)
                       │
       ┌───────────────┴───────────────┐
   VarDeclStmt                    FnDeclStmt
  ("x", IntLit(10))           ("add", [a, b], Block)
```

---

## 1. Abstract Base Nodes

1. **`ASTNode`**: Base class untuk seluruh simpul AST (menyimpan metadata `line` & `column`).
2. **`Expr`**: Base class untuk seluruh node ekspresi (menghasilkan nilai saat dievaluasi).
3. **`Stmt`**: Base class untuk seluruh node statement (menjalankan perintah/efek samping).

---

## 2. Expression Nodes (`Expr`)

* **`IntLitExpr`**: Literal Integer (`10`, `0xFF`)
* **`FloatLitExpr`**: Literal Float (`3.14`)
* **`BoolLitExpr`**: Literal Boolean (`true`, `false`)
* **`StringLitExpr`**: Literal String (`"text"`)
* **`CharLitExpr`**: Literal Character (`'A'`)
* **`NullLitExpr`**: Literal Null (`null`)
* **`IdentExpr`**: Nama Variabel / Identifier (`x`, `user`)
* **`BinaryExpr`**: Operasi Biner (`left op right`, misal `a + b`, `x * y`)
* **`UnaryExpr`**: Operasi Uner (`op operand`, misal `-x`, `!flag`)
* **`AssignExpr`**: Penugasan (`target op value`, misal `x = 10`, `sum += 5`)
* **`CallExpr`**: Pemanggilan Fungsi (`callee(args...)`)
* **`IndexExpr`**: Akses Indeks / Array (`object[index]`)
* **`MemberExpr`**: Akses Property / Method (`object.member`, `object?.member`)
* **`LambdaExpr`**: Fungsi Anonim (`(params) => expr`)
* **`ListLitExpr`**: Literal Array (`[1, 2, 3]`)
* **`DictLitExpr`**: Literal Map (`{"key": value}`)

---

## 3. Statement Nodes (`Stmt`)

* **`VarDeclStmt`**: Deklarasi Variabel (`let`/`var`/`const x: Type = expr`)
* **`FnDeclStmt`**: Deklarasi Fungsi (`fn name(params) -> Type { body }`)
* **`BlockStmt`**: Blok Pernyataan (`{ stmt1; stmt2; }`)
* **`IfStmt`**: Percabangan Kondisional (`if condition { thenBlock } else { elseBlock }`)
* **`WhileStmt`**: Perulangan While (`while condition { body }`)
* **`ForInStmt`**: Perulangan For-In (`for item in collection { body }`)
* **`ReturnStmt`**: Return Value (`return expr`)
* **`BreakStmt`**: Break Loop (`break`)
* **`ContinueStmt`**: Continue Loop (`continue`)
* **`MatchStmt`**: Pattern Matching (`match expr { case pattern => action }`)
* **`TryCatchStmt`**: Error Handling (`try { ... } catch (e) { ... } finally { ... }`)
* **`ClassDeclStmt`**: Deklarasi Class (`class Name { members }`)
* **`StructDeclStmt`**: Deklarasi Struct (`struct Name { fields }`)
* **`EnumDeclStmt`**: Deklarasi Enum (`enum Name { variants }`)
* **`ImportStmt`**: Import Modul (`import path as alias`)
* **`ExprStmt`**: Expression Statement (`expr;`)
* **`ProgramStmt`**: Root Node (`std::vector<StmtPtr> statements`)

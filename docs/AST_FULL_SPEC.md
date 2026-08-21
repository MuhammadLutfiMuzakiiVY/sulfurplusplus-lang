# Sulfur++ Complete AST Specification (AST_FULL_SPEC.md)

**Specification Version:** 0.1-AST-FULL (Components 146 - 185)  
**Status:** Frozen AST Specification & Visitor Architecture  
**Target File:** [`include/ast.hpp`](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/include/ast.hpp)

---

## 1. AST Architecture & Root Node

```
                  Program (Root)
                       │
       ┌───────────────┴───────────────┐
  VarDecl                          FnDecl
 ("x", 10)                     ("add", [a, b])
```

### 147. Program Node
```sfpp
Program {
    declarations: List<Declaration>
}
```

---

## 2. Declaration Nodes (148 - 151)

* **149. VariableDeclaration**: `kind` (Let/Var/Const), `name`, `type?`, `initializer?`
* **150. FunctionDeclaration**: `name`, `parameters`, `return_type?`, `body`, `modifiers`, `attributes`
* **151. Parameter**: `name`, `type?`, `default_value?`, `variadic: Bool`

---

## 3. Statement Nodes (152 - 157)

* **153. Block**: `statements: List<Statement>`
* **154. IfStatement**: `condition`, `then_branch`, `else_branch?`
* **155. ForStatement**: `variable`, `iterable`, `body`
* **156. WhileStatement**: `condition`, `body`
* **157. ReturnStatement**: `value: Expression?`

---

## 4. Expression Nodes (158 - 171)

* **159. IdentifierExpression**: `name: String`
* **160. LiteralExpression**: `kind` (Int, Float, String, Char, Bool, Null), `value`
* **161. BinaryExpression**: `left`, `operator`, `right` (Precedence enforced at tree construction)
* **162. UnaryExpression**: `operator`, `operand`
* **163. AssignmentExpression**: `target`, `operator`, `value`
* **164. CallExpression**: `callee`, `arguments: List<Expression>`
* **165. MemberExpression**: `object`, `property: Identifier`, `op: "." | "::" | "->"`
* **166. IndexExpression**: `object`, `index: Expression`
* **167. LambdaExpression**: `parameters`, `body: Block | Expression`
* **168 - 171. Collection ASTs**: Array (`[elements]`), Tuple (`(elements)`), Map (`entries`), Set (`elements`)

---

## 5. Type & Structure ASTs (172 - 179)

* **172. ClassDeclaration**: `name`, `superclass?`, `interfaces`, `members`
* **173. StructDeclaration**: `name`, `fields`, `methods`
* **174. EnumDeclaration**: `name`, `variants`
* **175. InterfaceDeclaration**: `name`, `methods`
* **176. ImportDeclaration**: `module`, `alias?`, `items`
* **177. Type AST**: Primitive, Named, Generic (`List<T>`), Array (`T[]`), Function (`(T1)->T2`), Optional (`T?`), Union (`T1|T2`)
* **178 - 179. Match & Pattern AST**: `MatchStatement` (`value`, `cases`), `Pattern` (Literal, Identifier, Wildcard `_`, Tuple, Array, Type)

---

## 6. Infrastructure & Architectural Invariants (180 - 185)

### 180. SourceLocation
Setiap AST node menyimpan lokasi presisi file dan kordinat:
```cpp
struct SourceLocation {
    std::string file;
    int start_line, start_column;
    int end_line, end_column;
};
```

### 181. AST Visitor Pattern
Visitor interface untuk traversal terisolasi (Semantic Analyzer, Type Checker, Compiler, Linter, Formatter):
```cpp
class ASTVisitor {
public:
    virtual void visitProgram(Program* node) = 0;
    virtual void visitVarDecl(VarDecl* node) = 0;
    virtual void visitFnDecl(FnDecl* node) = 0;
    virtual void visitClassDecl(ClassDecl* node) = 0;
    virtual void visitIf(IfStmt* node) = 0;
    virtual void visitBinary(BinaryExpr* node) = 0;
    virtual void visitCall(CallExpr* node) = 0;
    virtual void visitLiteral(LiteralExpr* node) = 0;
};
```

### 182 - 183. AST Printer & Invariants
* Tree visualizer untuk debugging compiler.
* **AST Invariants**: No syntax noise, precedence resolved at tree generation, parent-child links intact, non-null source locations.

### 184 - 185. Parser Strategy (Hybrid Engine)
```
          RECURSIVE DESCENT PARSER
          (Declarations & Statements)
                     │
                     ▼
            PRATT EXPRESSION PARSER
        (Operators, Precedence & Calls)
                     │
                     ▼
             ABSTRACT SYNTAX TREE
```

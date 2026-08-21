# Sulfur++ Parser Specification (PARSER_SPEC.md)

**Specification Version:** 0.1-PARSER (Component 186)  
**Status:** Official Parser Specification  
**Target File:** [`include/parser.hpp`](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/include/parser.hpp) & [`src/core/parser.cpp`](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/src/core/parser.cpp)

---

## 186. Parser Architecture: Hybrid Engine

Sulfur++ Parser menggunakan kombinasi **Recursive Descent** untuk struktur/declarations/statements dan **Pratt Parsing** (*Top-Down Operator Precedence*) untuk ekspresi.

```
Token Stream -> Recursive Descent (Statements & Top-level) -> Pratt Parser (Expressions) -> AST
```

---

## 1. Recursive Descent Function Hierarchy

```cpp
class Parser {
    // Top-Level & Declarations
    ProgramPtr parseProgram();
    DeclPtr parseDeclaration();
    DeclPtr parseVarDecl();
    DeclPtr parseFnDecl();
    DeclPtr parseClassDecl();
    DeclPtr parseStructDecl();

    // Statements
    StmtPtr parseStatement();
    StmtPtr parseBlock();
    StmtPtr parseIfStmt();
    StmtPtr parseWhileStmt();
    StmtPtr parseForInStmt();
    StmtPtr parseReturnStmt();
    StmtPtr parseMatchStmt();
    StmtPtr parseTryCatchStmt();

    // Pratt Parser Expressions
    ExprPtr parseExpression(int precedence = 0);
    ExprPtr parsePrefix();
    ExprPtr parseInfix(ExprPtr left);

    // Types & Patterns
    TypePtr parseType();
    PatternPtr parsePattern();
};
```

---

## 2. Pratt Parsing Binding Power Table

Pratt parser menggunakan tabel *Binding Power* (Precedence 1-14) untuk memproses operator tanpa memerlukan ratusan fungsi rekursif:

| Operator | Precedence Level | Prefix / Infix Handler | Associativity |
|---|---|---|---|
| `=` `+=` `-=` `*=` `/=` | 1 | `parseAssignment` | Right-to-Left |
| `? :` | 2 | `parseTernary` | Right-to-Left |
| `??` | 3 | `parseNullCoalesce` | Left-to-Right |
| `||` | 4 | `parseLogicalOr` | Left-to-Right |
| `&&` | 5 | `parseLogicalAnd` | Left-to-Right |
| `==` `!=` | 6 | `parseEquality` | Left-to-Right |
| `<` `<=` `>` `>=` | 7 | `parseComparison` | Left-to-Right |
| `+` `-` | 8 | `parseAdditive` | Left-to-Right |
| `*` `/` `%` | 9 | `parseMultiplicative` | Left-to-Right |
| `**` | 10 | `parseExponentiation` | Right-to-Left |
| `!` `-` `~` `await` | 11 | `parsePrefixUnary` | Right-to-Left |
| `()` `[]` `.` `?.` | 12 | `parsePostfixCallMemberIndex` | Left-to-Right |

---

## 3. Error Recovery Strategy (Panic-Mode Synchronization)

Jika terjadi kesalahan sintaksis, parser tidak langsung mengalami crash (*fail-fast*), melainkan melakukan *resynchronization* ke statement boundary berikutnya:

```cpp
void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FN:
            case TokenType::VAR:
            case TokenType::LET:
            case TokenType::CONST:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        advance();
    }
}
```

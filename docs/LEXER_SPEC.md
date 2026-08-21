# Sulfur++ Lexer & Token Specification (LEXER_SPEC.md)

**Specification Version:** 0.1-LEXER  
**Status:** Official Token Specification (Implementation Ready)  
**Target File:** [`include/token.hpp`](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/include/token.hpp) & [`src/core/lexer.cpp`](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/src/core/lexer.cpp)

---

## 1. Token Enumeration (`TokenType`)

Lexer memecah source code Sulfur++ menjadi aliran token dengan metadata `line` dan `column`:

```cpp
enum class TokenType {
    // Literals
    INT_LIT, FLOAT_LIT, STRING_LIT, CHAR_LIT, BOOL_LIT, NULL_LIT,

    // Identifiers
    IDENT,

    // Keywords
    LET, VAR, CONST, FN, RETURN, CLASS, STRUCT, INTERFACE, TRAIT,
    IF, ELSE, WHILE, FOR, IN, BREAK, CONTINUE, MATCH, CASE,
    IMPORT, FROM, AS, EXPORT, PUBLIC, PRIVATE, PROTECTED, STATIC,
    ASYNC, AWAIT, YIELD, TRY, CATCH, FINALLY, THROW, UNSAFE, DEFER,
    NULL_KW, TRUE_KW, FALSE_KW,

    // Operators & Delimiters
    PLUS, MINUS, STAR, SLASH, PERCENT, POWER,
    ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, STAR_ASSIGN, SLASH_ASSIGN,
    EQ, NEQ, LT, GT, LTE, GTE,
    AND, OR, NOT, BANG, BIT_AND, BIT_OR, BIT_XOR, BIT_NOT,
    ARROW, FAT_ARROW, NULL_COAL, QUESTION,
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    SEMICOLON, COLON, COMMA, DOT, AT,

    // End of File & Errors
    EOF_T, INVALID
};
```

---

## 2. Scanning Rules & Regex Rules

| Token Category | Rule / Character Range |
|---|---|
| Whitespace & Newline | `[ \t\r\n]+` (diabaikan kecuali sebagai pemisah token) |
| Single-Line Comment | `//[^\n]*` |
| Multi-Line Comment | `/\*[\s\S]*?\*/` |
| Identifier / Keyword | `[a-zA-Z_][a-zA-Z0-9_]*` (Pencocokan keyword dilakukan via Hash Table) |
| Integer Literal | `[0-9]+`, `0x[0-9a-fA-F]+`, `0b[01]+` |
| Float Literal | `[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?` |
| String Literal | `"([^"\\]|\\.)*"` (Mendukung escape sequence `\n`, `\t`, `\"`) |
| Character Literal | `'([^'\\]|\\.)'` |

---

## 3. Alur Ekseksi Scanning (`Lexer::nextToken()`)

```
Source Code Character Stream
       │
       ▼
[Skip Whitespace & Comments]
       │
       ▼
Match Character Prefix
 ├── Letter / '_' ──► Scan Identifier -> Lookup Keyword Map -> Token
 ├── Digit (0-9)  ──► Scan Number -> Detect '.' -> Int/Float Token
 ├── Quote (")    ──► Scan String Literal -> String Token
 ├── Symbol (+,-,*)──► Scan Operator/Delimiter -> Operator Token
 └── EOF / Unknown──► Return EOF_T / INVALID Token
```

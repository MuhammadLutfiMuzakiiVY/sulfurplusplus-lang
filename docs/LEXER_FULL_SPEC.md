# Sulfur++ Full Lexer Specification (LEXER_FULL_SPEC.md)

**Specification Version:** 0.1-LEXER-FULL (Components 114 - 145)  
**Status:** Frozen Lexer & Tokenizer Architecture Specification

---

### 114. Token Categories
1. Keywords
2. Identifiers
3. Literals
4. Operators
5. Delimiters
6. Comments
7. Whitespace (Skipped)
8. Newline (Option A: Ignored for `{}` blocks)
9. End-of-file (`EOF`)

### 115. Identifier Token
```ebnf
IDENTIFIER = (LETTER | "_") { LETTER | DIGIT | "_" } ;
```

### 116. Keyword Tokens
`LET`, `VAR`, `CONST`, `FN`, `RETURN`, `IF`, `ELSE`, `FOR`, `IN`, `WHILE`, `BREAK`, `CONTINUE`, `MATCH`, `CASE`, `CLASS`, `EXTENDS`, `STRUCT`, `ENUM`, `INTERFACE`, `TRAIT`, `IMPLEMENTS`, `IMPORT`, `FROM`, `AS`, `EXPORT`, `PUBLIC`, `PRIVATE`, `PROTECTED`, `STATIC`, `ASYNC`, `AWAIT`, `YIELD`, `TRY`, `CATCH`, `FINALLY`, `THROW`, `TRUE`, `FALSE`, `NULL`.

### 117. Integer Tokens
Standard (`10`), Hex (`0xFF`), Binary (`0b1010`), Octal (`0o755`), Separators (`1_000_000`).

### 118. Float Tokens
Decimals (`3.14`), Exponents (`1.5e10`, `2.5E-4`).

### 119. String & Character Tokens
* `STRING`: `"Hello"`, `"Hello World"`, `"User: ${name}"`
* `RAW_STRING`: `r"C:\path"`
* `MULTILINE_STRING`: `"""text"""`
* `CHARACTER`: `'a'`, `'\n'`, `'\t'`

### 121 - 131. Operator Tokens
* **Arithmetic**: `+`, `-`, `*`, `/`, `%`, `**`
* **Assignment**: `=`, `+=`, `-=`, `*=`, `/=`, `%=`
* **Comparison**: `==`, `!=`, `<`, `<=`, `>`, `>=`
* **Logical & Bitwise**: `&&`, `||`, `!`, `&`, `|`, `^`, `~`, `<<`, `>>`
* **Inc/Dec**: `++`, `--`
* **Range & Coalescing**: `..`, `..=`, `??`, `?`, `?.`
* **Arrow Operators**: `->` (return), `=>` (fat arrow)

### 132 - 133. Delimiters & Special Symbols
`()`, `{}`, `[]`, `,`, `.`, `:`, `;`, `@`, `#`, `$`, `_`, `...`.

### 134 - 137. Comments & Whitespace Policy
* Single-line `//`, Multi-line `/* */`, Doc comment `///`.
* Whitespace is ignored; `NEWLINE` is not emitted since `{}` defines block structure.
* Final stream token: `EOF`.

### 138 - 140. Token Structure, Source Location & Priority
```cpp
struct Token {
    TokenType kind;
    std::string lexeme;
    int line;
    int column;
    int offset;
    int length;
};
```
* **Longest Match First Priority**: Operators like `==`, `>=`, `..=`, `->`, `=>`, `??` are scanned before single-character tokens (`=`, `>`, `.`, `-`, `?`).

### 141 - 145. Lexer Pipeline, Error Recovery & Architecture
```
Source Code -> Character Stream -> Scanner -> Tokenization -> Token Stream -> Parser
```
Errors handled: `Unterminated String`, `Unterminated Comment`, `Invalid Character`, `Invalid Number`.

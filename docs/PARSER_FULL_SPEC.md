# Sulfur++ Complete Parser Specification (PARSER_FULL_SPEC.md)

**Specification Version:** 0.1-PARSER-FULL (Components 186 - 228)  
**Status:** Frozen Parser Architecture & Diagnostics Specification  
**Target File:** [`include/parser.hpp`](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/include/parser.hpp) & [`src/core/parser.cpp`](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/src/core/parser.cpp)

---

## 1. Parser Architecture & State (186 - 187)

Parser mengkonsumsi stream Token dan menghasilkan pohon AST tanpa melakukan type-checking.

```cpp
struct ParserState {
    std::vector<Token> tokens;
    size_t current = 0;
    std::vector<Diagnostic> errors;

    const Token& peek() const;
    const Token& previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token expect(TokenType type, const std::string& errorMsg);
};
```

---

## 2. Declaration & Statement Parsers (188 - 202)

* **188. `parse_program()`**: Iterasi top-level declarations hingga `EOF`.
* **189. `parse_declaration()`**: Dispatcher ke `parse_variable()`, `parse_function()`, `parse_class()`, `parse_struct()`, `parse_enum()`, `parse_interface()`, `parse_import()`.
* **190 - 192. Variable, Function & Parameter Parsers**: Parsing immutability (`let`), mutability (`var`), constants (`const`), default parameters (`name: Type = expr`), dan variadic args (`...numbers: Int`).
* **193. `parse_type()`**: Primary types (`Int`, `String`), Generic types (`List<Int>`), Array types (`Int[]`), Optional (`String?`), Function signatures (`(Int, Int) -> Bool`).
* **194 - 202. Statement Parsers**: Block `{ ... }`, `if / else if / else`, `for in`, `while`, `return`, `break`, `continue`, `match / case`, `try / catch / finally`.

---

## 3. Expression Pratt Parser (203 - 216)

```
parse_expression(precedence)
    ├── parse_prefix()    (Literals, Identifiers, Unary !, -, ~, Lambdas)
    └── parse_infix()     (Binary +, *, ==, Ternary, Call, Member, Index)
```

### Precedence Table (Lowest to Highest)
1. `ASSIGNMENT` (`=`, `+=`, `-=`)
2. `TERNARY` (`? :`)
3. `NULL_COALESCE` (`??`)
4. `OR` (`||`)
5. `AND` (`&&`)
6. `EQUALITY` (`==`, `!=`)
7. `COMPARISON` (`<`, `<=`, `>`, `>=`)
8. `ADDITION` (`+`, `-`)
9. `MULTIPLICATION` (`*`, `/`, `%`)
10. `POWER` (`**`)
11. `UNARY` (`!`, `-`, `~`, `await`)
12. `POSTFIX` (`()`, `[]`, `.`, `?.`)
13. `PRIMARY` (Literals, Identifiers, Parenthesized)

---

## 4. Structure, Attributes & Module Parsers (217 - 222)

* **217. Class Parser**: Parsing constructors (`init`), fields, methods, dan inheritance (`extends / implements`).
* **218 - 220. Struct, Enum & Interface**: Data POD, tagged unions, dan kontrak interface.
* **221 - 222. Import & Attributes**: `import path as alias`, `@test`, `@deprecated`, `@inline`.

---

## 5. Panic-Mode Error Recovery & Diagnostics (223 - 228)

### 223 - 224. Diagnostic Format & Resynchronization
Jika terjadi kesalahan sintaksis, parser tidak crash melainkan mencatat error dan melompat ke synchronization point (`SEMICOLON`, `FN`, `LET`, `CLASS`, `IF`, `}`);

```text
error[SYN001]: expected expression
 --> main.sul:5:13
  |
5 | let result =
  |             ^
```

### 227 - 228. Parser Contract
* **Rule**: Valid Tokens $\to$ Valid AST. Invalid Syntax $\to$ Diagnostic Report.
* **Separation of Concerns**: Parser HANYA memverifikasi tata bahasa (*syntax*). Type-checking dilakukan oleh **Semantic Analyzer**.

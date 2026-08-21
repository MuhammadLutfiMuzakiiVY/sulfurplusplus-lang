# Sulfur++ Lexical Specification v0.1

This document formalizes the complete lexical structure of **Sulfur++ (SF++ / .sfpp) Version 0.1**, defining tokens, keywords, literal grammars, operators, delimiters, and lexical rules.

---

## 1. Top-Level Token Hierarchy

Every source stream is parsed into a sequence of discrete lexical tokens:

```text
TOKEN ::= KEYWORD
        | IDENTIFIER
        | INTEGER
        | FLOAT
        | COMPLEX
        | STRING
        | CHAR
        | OPERATOR
        | DELIMITER
        | LIFECYCLE
        | COMMENT
        | WHITESPACE
        | NEWLINE
        | SHEBANG
        | EOF
```

---

## 2. Keywords

### 2.1 Active Language Keywords

| Category | Keywords |
|----------|----------|
| **Declarations** | `let`, `var`, `const`, `auto` |
| **Functions & Classes** | `fn`, `return`, `class`, `struct`, `enum`, `trait`, `interface`, `type` |
| **OOP & Lifecycle** | `init`, `drop`, `this`, `super` |
| **Branching** | `if`, `else`, `match` |
| **Loops** | `while`, `do`, `for`, `in`, `loop`, `break`, `continue` |
| **Resource & Safety** | `defer`, `unsafe`, `native`, `extern`, `inline`, `mut`, `ref`, `out`, `move` |
| **Error Handling** | `try`, `catch`, `finally`, `throw`, `assert` |
| **Module & FFI** | `import`, `export`, `expose`, `overwrite`, `module`, `namespace`, `from`, `as` |
| **Visibility & Modifiers** | `public`, `private`, `protected`, `static`, `final`, `abstract`, `sealed`, `virtual`, `override` |
| **Async / Coroutine** | `async`, `await` |
| **Memory / Instantiation** | `ptr`, `new`, `delete` |
| **Properties & Constraints**| `get`, `set`, `where`, `implements`, `extends` |
| **Literals** | `true`, `false`, `null` |
| **Built-in Type Tokens** | `int_8`, `int_16`, `int_32`, `int_64`, `uint_8`, `uint_16`, `uint_32`, `uint_64`, `float_32`, `float_64`, `bool`, `char`, `str`, `void`, `list`, `dict`, `set`, `matrix` |

### 2.2 Reserved Future Keywords

The following keywords are reserved for subsequent language revisions:
`macro`, `yield`, `generator`, `actor`, `channel`, `spawn`, `select`, `union`, `package`, `pub`, `priv`, `operator`.

---

## 3. Identifiers

```ebnf
IDENTIFIER       ::= IDENTIFIER_START IDENTIFIER_CONTINUE* ;
IDENTIFIER_START ::= LETTER | "_" ;
IDENTIFIER_CONTINUE ::= LETTER | DIGIT | "_" ;
LETTER           ::= ASCII_LETTER | UNICODE_LETTER ;
ASCII_LETTER     ::= "A".."Z" | "a".."z" ;
DIGIT            ::= "0".."9" ;
```

Identifiers are case-sensitive. In standard identifier positions, type tokens are contextually accepted where non-ambiguous.

---

## 4. Literals

### 4.1 Integer Literals

Supports decimal, hexadecimal (`0x`/`0X`), binary (`0b`/`0B`), and octal (`0o`/`0O`) forms with digit separators (`_`):

```sfpp
42
1_000_000
0xFF_A0
0b1101_0010
0o755
```

Time-unit suffixes are automatically converted to millisecond floating-point representations:
- `100ms` $\rightarrow$ `100.0`
- `500us` $\rightarrow$ `0.5`
- `2s` $\rightarrow$ `2000.0`
- `5m` $\rightarrow$ `300000.0`
- `1h` $\rightarrow$ `3600000.0`
- `1d` $\rightarrow$ `86400000.0`

### 4.2 Floating Point & Complex Literals

```ebnf
FLOAT   ::= DECIMAL_DIGIT+ "." DECIMAL_DIGIT+ EXPONENT?
          | DECIMAL_DIGIT+ EXPONENT ;
COMPLEX ::= FLOAT "i" | INTEGER "i" ;
```

Examples: `3.14159`, `1.0e-5`, `2.5e3`, `4.0i`, `10i`.

### 4.3 String Literals

1. **Normal Strings (`"..."`)**: Standard UTF-8 strings with escape sequences.
2. **Raw Strings (`r"..."`)**: Treats backslashes as raw literal characters.
3. **Process Strings (`ps"..."`)**: Interpolation with formatted segment tags (e.g. `ps"Val: {x:pad(5)}"`).
4. **Format Strings (`f"..."`)**: Direct expression interpolation (e.g. `f"Hello {name}"`).

### 4.4 Escape Sequences

| Sequence | Value |
|----------|-------|
| `\n`, `\r`, `\t` | Line feed, Carriage return, Horizontal tab |
| `\a`, `\b`, `\f`, `\v` | Bell, Backspace, Formfeed, Vertical tab |
| `\0` | Null byte |
| `\\`, `\"`, `\'` | Backslash, Double quote, Single quote |
| `\xHH` | 8-bit hex byte |
| `\uHHHH` | 16-bit Unicode code point |
| `\UHHHHHHHH` | 32-bit Unicode code point |

---

## 5. Operators & Delimiters

### 5.1 Operator Table

| Type | Operators |
|------|-----------|
| **Arithmetic** | `+`, `-`, `*`, `/`, `%`, `**` |
| **Comparison** | `==`, `!=`, `<`, `>`, `<=`, `>=` |
| **Logical** | `&&`, `\|\|`, `!` |
| **Bitwise** | `&`, `\|`, `^`, `~`, `<<`, `>>` |
| **Assignment** | `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `\|=`, `^=` |
| **Navigation & Arrow** | `->`, `=>`, `::`, `?.`, `??`, `\|>` |
| **Range & Spread** | `..`, `..=`, `...` |
| **Increment/Decrement** | `++`, `--` |

### 5.2 Delimiters

`(`, `)`, `[`, `]`, `{`, `}`, `,`, `.`, `;`, `:`, `@`, `#`, `'`.

### 5.3 Special Lifecycle Tokens

- `+N>` : Class constructor invocation sequence (e.g., `+1>init`).
- `~N>` : Class destructor / cleanup invocation sequence (e.g., `~1>cleanup`).

---

## 6. Shebang, Comments, Whitespace

```ebnf
SHEBANG       ::= "#!" ? any character except newline ?* ;
LINE_COMMENT  ::= "//" ? any character except newline ?* ;
BLOCK_COMMENT ::= "/*" ? any character ?* "*/" ;
WHITESPACE    ::= " " | "\t" | "\r" ;
NEWLINE       ::= "\n" | "\r\n" ;
```

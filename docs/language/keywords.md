# Sulfur++ Keywords Specification v0.1

This document lists all reserved keywords in Sulfur++ v0.1 as implemented in `src/core/lexer.cpp` and recognized by `src/core/parser.cpp`.

---

## 1. Primary Language Keywords

| Keyword | Token Type | Category | Usage Description |
|---------|------------|----------|-------------------|
| `let` | `TokenType::LET` | Variable Declaration | Immutable variable binding (enforced in semantic pass) |
| `var` | `TokenType::VAR` | Variable Declaration | Mutable variable declaration |
| `auto` | `TokenType::AUTO` | Variable Declaration / Type | Inferred type variable declaration |
| `fn` | `TokenType::FN` | Functions & Lambdas | Function, method, or anonymous lambda declaration |
| `return` | `TokenType::RETURN` | Control Flow | Return from a function body |
| `class` | `TokenType::CLASS` | OOP | Class definition |
| `struct` | `TokenType::STRUCT` | OOP / Value Type | Struct definition |
| `interface` | `TokenType::INTERFACE` | OOP / Contract | Interface contract definition |
| `if` | `TokenType::IF` | Control Flow | Conditional branching |
| `else` | `TokenType::ELSE` | Control Flow | Alternative branch in `if` statement |
| `while` | `TokenType::WHILE` | Loops | While loop condition |
| `for` | `TokenType::FOR` | Loops | `for-in` iterable or C-style `for` loop |
| `in` | `TokenType::IN` | Loops | In operator for `for (x in iterable)` |
| `break` | `TokenType::BREAK` | Loop Control | Break out of nearest enclosing loop |
| `continue` | `TokenType::CONTINUE` | Loop Control | Skip to next iteration of nearest loop |
| `import` | `TokenType::IMPORT` | Module System | Import module/library |
| `as` | `TokenType::AS` | Module System | Alias definition for import/export/expose |
| `export` | `TokenType::EXPORT` | Module System | Export definitions to module scope |
| `expose` | `TokenType::EXPOSE` | FFI / Native | Expose native/external functions |
| `overwrite` | `TokenType::OVERWRITE` | Runtime / Config | Dynamic runtime parameter/symbol overwrite |
| `this` | `TokenType::THIS_KW` | OOP | Current instance reference |
| `unsafe` | `TokenType::UNSAFE` | Memory Safety | Unsafe block for pointer manipulation & manual memory |
| `defer` | `TokenType::DEFER` | Resource Management | LIFO execution block upon exiting current scope |
| `try` | `TokenType::TRY` | Error Handling | Begin try-catch exception block |
| `catch` | `TokenType::CATCH` | Error Handling | Catch block for thrown exceptions |
| `throw` | `TokenType::THROW` | Error Handling | Throw an exception or error value |
| `ptr` | `TokenType::PTR` | Memory / Types | Pointer type qualifier (`ptr<T>`) |
| `new` | `TokenType::NEW` | Object Lifecycle | Heap instantiation of class / object |
| `delete` | `TokenType::DELETE` | Memory Management | Manual deallocation in `unsafe` block |
| `match` | `TokenType::MATCH` | Control Flow | Pattern matching construct |
| `null` | `TokenType::NULL_KW` | Literal | Null value literal |
| `true` | `TokenType::TRUE_KW` | Literal | Boolean true literal |
| `false` | `TokenType::FALSE_KW` | Literal | Boolean false literal |

---

## 2. Type Tokens (First-Class Keywords)

| Keyword | Token Type | Size | Description |
|---------|------------|------|-------------|
| `int_8` | `TokenType::TYPE_INT8` | 1 byte | Signed 8-bit integer (-128 .. 127) |
| `int_16` | `TokenType::TYPE_INT16` | 2 bytes | Signed 16-bit integer |
| `int_32` | `TokenType::TYPE_INT32` | 4 bytes | Signed 32-bit integer |
| `int_64` | `TokenType::TYPE_INT64` | 8 bytes | Signed 64-bit integer |
| `uint_8` | `TokenType::TYPE_UINT8` | 1 byte | Unsigned 8-bit integer (0 .. 255) |
| `uint_16` | `TokenType::TYPE_UINT16` | 2 bytes | Unsigned 16-bit integer |
| `uint_32` | `TokenType::TYPE_UINT32` | 4 bytes | Unsigned 32-bit integer |
| `uint_64` | `TokenType::TYPE_UINT64` | 8 bytes | Unsigned 64-bit integer |
| `float_32` | `TokenType::TYPE_FLOAT32` | 4 bytes | IEEE 754 32-bit single precision float |
| `float_64` | `TokenType::TYPE_FLOAT64` | 8 bytes | IEEE 754 64-bit double precision float |
| `bool` | `TokenType::TYPE_BOOL` | 1 byte | Boolean type |
| `char` | `TokenType::TYPE_CHAR` | 1 byte | Character type |
| `str` | `TokenType::TYPE_STR` | Variable | String type (UTF-8) |
| `void` | `TokenType::TYPE_VOID` | 0 bytes | Void return type |
| `list` | `TokenType::TYPE_LIST` | Variable | Dynamic list collection (`list<T>`) |
| `set` | `TokenType::TYPE_SET` | Variable | Hash set collection (`set<T>`) |
| `dict` | `TokenType::TYPE_DICT` | Variable | Hash map / dictionary collection (`dict<K, V>`) |
| `matrix` | `TokenType::TYPE_MATRIX` | Variable | Multi-dimensional matrix collection |

---

## 3. Contextual Keywords (Parsed by Context)

- `finally`: Parsed after `catch (...) { ... }` in `try` statement.
- `with`: Parsed in `overwrite <target> with <expr>;`.
- `_`: Wildcard pattern in `match` statement.
- `+N>` / `~N>`: Constructor/Destructor ordering annotations in `class` bodies.

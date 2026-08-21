# Sulfur++ Formal Language Specification v0.1

This directory serves as the **Single Source of Truth** for the Sulfur++ language syntax, lexical grammar, AST specification, type system, and semantic rules. The C++ compiler components (`src/core/lexer.cpp`, `src/core/parser.cpp`, `src/core/semantic/analyzer.cpp`, `src/core/semantic/type_checker.cpp`, and `src/core/interpreter.cpp`) adhere strictly to this specification.

---

## 1. Modular Grammar (`docs/language/grammar/`)

The grammar is organized into modular EBNF specifications:

| Module | File | Description |
|--------|------|-------------|
| **AST Grammar** | [`ast.ebnf`](grammar/ast.ebnf) | Complete 40-section formal AST node grammar |
| **Compilation Unit** | [`compilation-unit.ebnf`](grammar/compilation-unit.ebnf) | Top-level entry point and module items |
| **Declarations** | [`declarations.ebnf`](grammar/declarations.ebnf) | Variables, functions, structs, classes, interfaces, modules |
| **Statements** | [`statements.ebnf`](grammar/statements.ebnf) | Blocks, conditionals, loops, match, defer, exceptions |
| **Expressions** | [`expressions.ebnf`](grammar/expressions.ebnf) | Complete 17-level expression precedence hierarchy |
| **Patterns** | [`patterns.ebnf`](grammar/patterns.ebnf) | Match case patterns and future structural destructuring |
| **Types** | [`types.ebnf`](grammar/types.ebnf) | Primitives, generics, pointers, collections, nullable |
| **Literals** | [`literals.ebnf`](grammar/literals.ebnf) | Numbers, strings, chars, ps-strings, booleans, null |
| **Lexical** | [`lexical.ebnf`](grammar/lexical.ebnf) | 16-section formal lexical grammar |

---

## 2. Core Specification Documents

| Document | File | Description |
|----------|------|-------------|
| **Complete Grammar (Single File)** | [`grammar.ebnf`](grammar.ebnf) | Full unified EBNF syntax specification |
| **AST Node Reference** | [`ast-nodes.md`](ast-nodes.md) | Mapping of all 24 Expr & 24 Stmt nodes to C++ structs |
| **Lexical Specification** | [`lexical-spec.md`](lexical-spec.md) | Tokens, literals, escape sequences, operators, shebang |
| **Keywords Reference** | [`keywords.md`](keywords.md) | Reserved keywords, type tokens, and contextual keywords |
| **Operator Precedence** | [`operator-precedence.md`](operator-precedence.md) | 17-level precedence table and parser descent chain |
| **Semantic Specification & Type System** | [`semantic-specification.md`](semantic-specification.md) | Full 14-domain formal semantic rules, resolution, and error codes |
| **Type System Reference** | [`type-system.md`](type-system.md) | Type hierarchy, gradual typing, widening conversions |
| **Scope & Resolution Rules** | [`scope-rules.md`](scope-rules.md) | Scope hierarchy, shadowing, closures, defer lifecycle |
| **Semantics Summary** | [`semantics.md`](semantics.md) | High-level summary of semantic validation |

---

## 3. Specification Pipeline

```text
                    Sulfur++ Source (.sfpp)
                              │
                              ▼
                        ┌───────────┐
                        │   Lexer   │  (docs/language/grammar/lexical.ebnf)
                        └─────┬─────┘
                              │
                              ▼
                        ┌───────────┐
                        │  Parser   │  (docs/language/grammar/grammar.ebnf)
                        └─────┬─────┘
                              │
                              ▼
                        ┌───────────┐
                        │    AST    │  (docs/language/grammar/ast.ebnf)
                        └─────┬─────┘
                              │
                  ┌───────────┴───────────┐
                  ▼                       ▼
            ┌───────────┐           ┌───────────┐
            │ Resolver  │           │ Validator │
            └─────┬─────┘           └─────┬─────┘
                  │                       │
                  └───────────┬───────────┘
                              ▼
                       ┌──────────────┐
                       │ Type Checker │ (docs/language/semantic-specification.md)
                       └──────┬───────┘
                              │
                              ▼
                       ┌──────────────┐
                       │  Semantic IR │
                       └──────┬───────┘
                              │
                 ┌────────────┼────────────┐
                 ▼            ▼            ▼
              Bytecode       LLVM IR      Runtime
                 │            │            │
                 ▼            ▼            ▼
              VM/GC         Native       FFI
```

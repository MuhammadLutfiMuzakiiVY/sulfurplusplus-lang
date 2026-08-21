# Sulfur++ Scope & Resolution Rules v0.1

This document specifies the formal scope resolution, symbol lifecycle, visibility, and immutability rules enforced by the Sulfur++ Semantic Analyzer (`src/core/semantic/analyzer.cpp`) and Resolver.

---

## 1. Scope Hierarchy

Sulfur++ utilizes **lexical scoping** with a hierarchical tree structure:

```text
Global / Module Scope (Top Level)
  ├── Namespace Scope (Optional)
  │     ├── Class / Struct Scope
  │     │     ├── Member Functions / Methods
  │     │     │     ├── Local Block Scope (Level 1)
  │     │     │     │     └── Nested Block Scope (Level 2...)
  │     │     └── Field Initializers
  │     └── Free Functions
  │           └── Local Block Scope (Level 1...)
  └── Global Statements & Declarations
```

---

## 2. Symbol Resolution Order

When an identifier `x` is encountered in an expression:
1. **Local Block Scope**: Search the innermost `{ ... }` block.
2. **Enclosing Blocks**: Walk up through parent block scopes within the same function.
3. **Function Parameters**: Check declared parameter names of the enclosing `fn`.
4. **Closure Captures**: If inside an anonymous `fn` lambda, capture references from outer function scopes.
5. **Class / Struct Scope**: If inside a method or class body, resolve instance and static members (`this.x` or direct `x`).
6. **Module / Global Scope**: Check top-level symbols, imported modules, and built-in functions.
7. **Failure**: Emit semantic error `E_UNDEF_VAR` / `E_UNDEF_FN`.

---

## 3. Variable Immutability & Mutability Rules

Sulfur++ enforces strict variable mutability based on the declaration keyword:

### `let` (Immutable)
- Must be assigned during declaration or initialization.
- Subsequent assignment (`x = ...`, `x += ...`) triggers a compile-time/semantic error `E_IMMUT_ASSIGN`.
- Value cannot be reassigned; however, mutating contents of reference types (e.g. `list.push()`) is permitted unless frozen.

### `var` (Mutable)
- Can be declared with or without initial value.
- Reassignment and in-place arithmetic updates (`+=`, `-=`, etc.) are permitted anywhere in scope.

### `auto` (Type-Inferred Mutable)
- Type is statically or dynamically inferred from the initializer.
- Behave as mutable variables (`var`).

---

## 4. Variable Shadowing

- **Inner vs Outer Block**: An inner block may declare a variable with the same name as an outer block. The inner declaration shadows the outer one until the block terminates.
- **Function vs Global**: Local variables inside functions shadow module/global symbols of the same name.
- **Duplicate in Same Scope**: Declaring two variables with the same name within the exact same block scope is an error (`E_DUP_DECL`).

---

## 5. Closure & Lambda Scoping

```sfpp
fn makeCounter() {
    var count = 0;
    return fn() {
        count += 1;
        return count;
    };
}
```
- Closures capture enclosing variables by reference in the runtime environment.
- Mutating a captured variable inside a closure reflects in the enclosing scope across invocations.

---

## 6. Defer Scope Semantics

- `defer { ... }` blocks are bound to the **enclosing function scope** (or lexical block depending on execution context).
- When execution leaves the scope (via normal flow, `return`, or uncaught exception), deferred blocks execute in **LIFO (Last-In, First-Out)** order.
- Deferred statements retain access to the scope's variables at the time of exit.

# Sulfur++ Semantic Specification & Type System v0.1

This document defines the formal **Semantic Grammar, Static Type System, and Resolution Rules** for Sulfur++ Version 0.1. It specifies the compile-time checks performed by the **Resolver**, **Type Checker**, and **Semantic Validator** before bytecode lowering or native code generation.

---

## 1. Scope & Symbol Table

### 1.1 Scope Kinds
Sulfur++ organizes code into five distinct lexical scope levels:

| Scope Kind | Creator | Lifetime | Hoisting Rules |
|------------|---------|----------|----------------|
| **Global Scope** | Compilation Unit (`.sfpp`) | Module execution | Top-level functions & classes are hoisted; variables are evaluated sequentially. |
| **Namespace Scope** | `namespace N { ... }` | Compilation Unit | Named hierarchy partition; symbols accessed via `N::sym`. |
| **Class/Struct Scope**| `class C { ... }` / `struct S { ... }` | Type lifetime | Fields and methods visible to all members via `this`. |
| **Function Scope** | `fn name(...) { ... }` | Function call invocation | Parameters and local declarations live in this activation record. |
| **Block Scope** | `{ ... }` (if, while, match, bare) | Block execution | Variables declared within `{ ... }` are destroyed at block exit. |

### 1.2 Symbol Record Structure

```text
Symbol {
    id: SymbolId,
    name: String,
    kind: SymbolKind (Var | Const | Fn | Parameter | Class | Struct | Interface | Trait | TypeAlias),
    type: Type,
    mutability: Immutable | Mutable,
    visibility: Public | Private | Protected,
    is_hoisted: Bool,
    scope_depth: Int,
    declaration_span: SourceSpan
}
```

---

## 2. Name Resolution & Shadowing

### 2.1 Resolution Algorithm (Lexical Lookup)
To resolve identifier $E$:
1. Search current innermost block symbol table $S_0$.
2. If not found, iterate upward through parent scopes $S_1, S_2, \dots, S_{\text{global}}$.
3. If inside a class method and not found in block/function scopes, inspect `this` fields and methods.
4. If inside a module, inspect imported symbols and standard built-ins (`std/io`, `std/math`, etc.).
5. If unresolved at top level, emit diagnostic `E001: UndefinedSymbol`.

### 2.2 Shadowing Invariants
- **Permitted**: An inner block variable may shadow an outer variable or global symbol with identical name.
- **Prohibited**: Redeclaring a variable in the *exact same* scope level produces `E002: DuplicateDeclaration`.

---

## 3. Type System & Hierarchy

```text
                      any (Top Type)
                       │
       ┌───────────────┼───────────────┬───────────────┐
       ▼               ▼               ▼               ▼
     numeric         bool            char             str
       │
  ┌────┴────┐
  ▼         ▼
integer   float
(int_8..64) (float_32..64)
(uint_8..64)
  │
  ▼
complex_128

       ┌───────────────┼───────────────┬───────────────┐
       ▼               ▼               ▼               ▼
   collection      class instances   structs        ptr<T>
   (list, dict,
    set, matrix)
       │
       ▼
   fn(T) -> U

       │
       ▼
     never / void (Bottom Types)
```

### 3.1 Type Definitions
- **Primitive Types**: `bool`, `char`, `str`, `int_8`, `int_16`, `int_32`, `int_64`, `uint_8`, `uint_16`, `uint_32`, `uint_64`, `float_32`, `float_64`, `complex_128`, `void`.
- **Reference Types**: `list<T>`, `dict<K, V>`, `set<T>`, `matrix<T>`, class instances, closures.
- **Value Types**: Struct instances, primitives.
- **Unsafe Types**: `ptr<T>` (raw pointer).
- **Gradual Type**: `any` (dynamically checked fallback).

---

## 4. Type Inference & Conversion Rules

### 4.1 Local Bidirectional Inference
- If a declaration omits type annotation (`let x = expr;` or `var x = expr;`), $\text{Type}(x) = \text{Infer}(\text{expr})$.
- Integer literals infer as `int_64` by default.
- Float literals infer as `float_64` by default.
- Untyped parameters default to `any`.

### 4.2 Widening vs Coercion Matrix

| Target Type ($T_{\text{dest}}$) | Permitted Source ($T_{\text{src}}$) | Conversion Kind | Rule |
|---------------------------------|-----------------------------------|-----------------|------|
| `int_16` | `int_8` | Implicit | Safe Widening |
| `int_32` | `int_8`, `int_16` | Implicit | Safe Widening |
| `int_64` | `int_8`, `int_16`, `int_32` | Implicit | Safe Widening |
| `float_64` | `float_32`, `int_8`, `int_16`, `int_32` | Implicit | Safe Promotion |
| `complex_128` | `float_64`, `int_64`, `float_32` | Implicit | Imaginary = 0.0 |
| `T?` | `T`, `null` | Implicit | Nullable Wrap |
| `int_32` | `int_64` | Explicit (`toInt32()`) | Narrowing (Potential Overflow) |
| `str` | Any Primitive | Explicit (`toStr()`) | Stringification |

---

## 5. Mutability & Variable Semantics

| Declaration | Mutability | Reassignment (`=`) | In-Place Mutation (`list.push()`) | Re-declaration in Same Scope |
|-------------|------------|-------------------|-----------------------------------|------------------------------|
| `let x: T = v;` | Immutable | ❌ Rejected (`E010`) | Permitted (if reference type) | ❌ Rejected (`E002`) |
| `var x: T = v;` | Mutable | Allowed | Allowed | ❌ Rejected (`E002`) |
| `auto x = v;` | Mutable | Allowed | Allowed | ❌ Rejected (`E002`) |
| `const C: T = v;`| Compile-time Constant | ❌ Rejected (`E010`) | ❌ Rejected | ❌ Rejected (`E002`) |

---

## 6. Function Rules & Signatures

1. **Arity Validation**: Calling `f(a_1, ..., a_n)` where signature is `fn f(p_1: T_1, ..., p_m: T_m)` requires $n = m$ unless default arguments exist. Failing emits `E020: ArityMismatch`.
2. **Type Checking**: $\text{Type}(a_i) \sqsubseteq T_i$ for all $i \in [1, n]$.
3. **Return Consistency**: All control-flow paths in a function with explicit return type `-> T` (where $T \neq \text{void}$) must terminate in `return expr;` where $\text{Type}(\text{expr}) \sqsubseteq T$. Missing return triggers `E021: MissingReturnPath`.

---

## 7. Class & OOP Semantics

```sfpp
class Animal {
    name: str;
    +1>init;
    ~1>cleanup;

    fn init(name: str) {
        this.name = name;
    }
    fn cleanup() {
        // resource release
    }
}
```

1. **Lifecycle Constructor Chain (`+N>name`)**: Ordered ascending by integer $N$. When `new C(...)` executes, constructor stages run sequentially $+1 \rightarrow +2 \rightarrow \dots \rightarrow +k$.
2. **Lifecycle Destructor Chain (`~N>name`)**: Ordered ascending by integer $N$. When object is dropped/reclaimed, destructor stages run sequentially.
3. **Interface Adherence**: If `class C : InterfaceA`, $C$ must provide concrete implementations for all method signatures declared in `InterfaceA` (`E030: InterfaceNotSatisfied`).
4. **Member Visibility**:
   - `public`: Accessible from any module or caller.
   - `private`: Accessible only inside methods of the declaring class.
   - `protected`: Accessible inside declaring class and inheriting subclasses.

---

## 8. Struct Rules (Value Semantics)

```sfpp
struct Point {
    x: float_64;
    y: float_64;
}
```

1. **Stack Allocation**: Struct instances with non-escaping lifetimes are stack-allocated.
2. **Copy on Assignment**: Assigning `let p2 = p1;` copies all fields by value.
3. **Immutability Invariant**: Mutating a field of an immutable struct binding `let p = Point{1.0, 2.0}; p.x = 3.0;` is rejected (`E010: ImmutableMutation`).

---

## 9. Closures & Environment Captures

```sfpp
fn makeAdder(x: int_64) {
    return fn(y: int_64) -> int_64 {
        return x + y;
    };
}
```

1. **Capture Mode**: Variables captured from outer scopes are bound by **shared reference**.
2. **Escape Analysis**: If a closure escapes its enclosing activation frame (e.g. returned or stored in heap), the captured environment frame is promoted to the GC Heap.
3. **Concurrent Mutation**: Modifying captured state is visible across all active closure references.

---

## 10. Pattern Matching Semantics

```sfpp
match (value) {
    0 => { return "Zero"; }
    1 => { return "One"; }
    _ => { return "Other"; }
}
```

1. **Top-to-Bottom Evaluation**: Match cases are evaluated sequentially. The first matching pattern executes its associated body.
2. **Wildcard `_`**: Matches any value of any type. Acts as catch-all.
3. **Exhaustiveness**: A match statement used in an expression context or expected to be exhaustive must contain a default arm `_` or cover the entire variant domain (`E040: NonExhaustiveMatch`).

---

## 11. Error Handling & Exceptions

```sfpp
try {
    if (divisor == 0) throw "Division by zero";
} catch (err) {
    io.Terminal.Err << err << "\n";
} finally {
    cleanup();
}
```

1. **Throw Semantics**: `throw expr;` unwinds the call stack until the nearest enclosing `try-catch` frame is found.
2. **Catch Binding**: The identifier in `catch (e)` receives the thrown value.
3. **Finally Invariant**: The `finally` block is guaranteed to execute before leaving the try-catch construct, regardless of whether execution completed normally, threw an exception, or executed a `return`.

---

## 12. Null Safety & Optionals

1. **Non-Nullable Default**: Primitive types (`int_64`, `str`, etc.) cannot hold `null`. Assigning `let s: str = null;` produces `E050: NullAssignmentToNonNullable`.
2. **Nullable Types (`T?`)**: Declared with `?` suffix (`let s: str? = null;`).
3. **Null-Coalescing (`??`)**: `exprA ?? exprB` evaluates `exprA`; if `null`, yields `exprB`. Type is $\text{NonNull}(\text{Type}(A)) \cup \text{Type}(B)$.
4. **Safe Navigation (`?.`)**: `obj?.field` returns `null` immediately if `obj == null` without evaluating subsequent member chains.

---

## 13. Unsafe & Raw Pointer Rules

```sfpp
unsafe {
    var raw: ptr<int_64> = &value;
    *raw = 99;
    delete raw;
}
```

1. **Safety Boundary**: Pointer address-of `&`, dereference `*`, and manual deallocation `delete` are strictly forbidden outside `unsafe { ... }` blocks (`E060: UnsafeOperationOutsideUnsafeBlock`).
2. **GC Independence**: Memory managed via `ptr<T>` is exempt from automatic GC sweep; caller is responsible for preventing memory leaks and use-after-free bugs.

---

## 14. Compile-Time Semantic Diagnostic Catalog

| Code | Diagnostic Name | Severity | Description |
|------|-----------------|----------|-------------|
| `E001` | `UndefinedSymbol` | Error | Reference to undeclared identifier. |
| `E002` | `DuplicateDeclaration` | Error | Redeclaration of symbol in identical scope. |
| `E010` | `ImmutableReassignment`| Error | Reassignment to immutable `let` or `const`. |
| `E011` | `TypeMismatch` | Error | Expression type incompatible with expected type. |
| `E020` | `ArityMismatch` | Error | Argument count does not match function parameter count. |
| `E021` | `MissingReturnPath` | Error | Function missing return statement along branch. |
| `E030` | `InterfaceNotSatisfied`| Error | Class missing implementation of interface method. |
| `E040` | `NonExhaustiveMatch` | Error | Match statement missing default arm or unhandled variants. |
| `E050` | `NullSafetyViolation` | Error | Null assigned or passed to non-nullable type. |
| `E060` | `UnsafeViolation` | Error | Raw pointer or manual memory operation outside `unsafe`. |
| `E070` | `InvalidControlFlow` | Error | `break` or `continue` outside loop, or `return` outside function. |
| `E080` | `CircularInheritance` | Error | Class inherits from itself directly or transitively. |

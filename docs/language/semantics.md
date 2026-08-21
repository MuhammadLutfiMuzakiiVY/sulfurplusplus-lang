# Sulfur++ Semantic Rules & Static Analysis v0.1

This document provides a concise reference for the semantic rules enforced by the Sulfur++ Semantic Analyzer (`src/core/semantic/analyzer.cpp`), Type Checker (`src/core/semantic/type_checker.cpp`), and Interpreter Runtime (`src/core/interpreter.cpp`).

For the complete formal mathematical specification, see [Semantic Specification & Type System](semantic-specification.md).

---

## Quick Reference Summary

### 1. Variables & Scope
- **Lexical Scoping**: Inner blocks can shadow outer blocks.
- **`let`**: Immutable binding. Reassignment emits `E010: ImmutableReassignment`.
- **`var` / `auto`**: Mutable variable binding.
- **`const`**: Compile-time evaluated constant.

### 2. Gradual & Static Typing
- Explicit annotations (`let x: int_64 = 10;`) enforce strict compile-time checking.
- Omitted annotations infer type from the initializer.
- `int_8` $\rightarrow$ `int_16` $\rightarrow$ `int_32` $\rightarrow$ `int_64` $\rightarrow$ `float_64` widen implicitly.
- Narrowing conversions require explicit cast functions (`toInt32()`, etc.).

### 3. Functions & Methods
- Exact arity validation (argument count == parameter count).
- Parameter type checking and return type enforcement.
- Closures capture environment variables by shared reference.

### 4. Classes, Interfaces, & Structs
- Constructor execution orders: `+1>initMethod`, `+2>subInit` executed ascending.
- Destructor cleanup orders: `~1>cleanupMethod` executed ascending.
- Classes must implement all interface method signatures.
- Structs have value-copy semantics.

### 5. Control Flow Validity
- `break` and `continue` are only legal inside `while` or `for` loops.
- `return` is only legal inside function or method bodies.
- `defer { ... }` blocks execute LIFO on function exit.

### 6. Memory Safety & Pointers
- Pointer creation (`&`), dereferencing (`*`), and deletion (`delete`) are only legal inside `unsafe { ... }` blocks.
- GC automatically manages objects on the heap.

### 7. Diagnostics
- See [`semantic-specification.md#14-compile-time-semantic-diagnostic-catalog`](semantic-specification.md#14-compile-time-semantic-diagnostic-catalog) for full `E001`–`E080` error codes.

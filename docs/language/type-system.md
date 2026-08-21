# Sulfur++ Type System Specification v0.1

This document defines the formal type system: primitive types, collection types, user-defined types, type relationships, conversion rules, and the gradual typing model.

---

## 1. Type Hierarchy

```text
any
 ├── null
 ├── bool
 ├── numeric
 │    ├── integer
 │    │    ├── int_8  int_16  int_32  int_64
 │    │    └── uint_8 uint_16 uint_32 uint_64
 │    ├── float
 │    │    ├── float_32
 │    │    └── float_64
 │    └── complex_128
 ├── char
 ├── str
 ├── collection
 │    ├── list<T>
 │    ├── dict<K,V>
 │    ├── set<T>
 │    └── matrix<T>
 ├── ptr<T>
 ├── fn(params) -> ret
 ├── class instances
 └── void
```

---

## 2. Primitive Types

| Type | Alias | Size | Default | Range |
|------|-------|------|---------|-------|
| `int_8` | — | 1 byte | `0` | -128 to 127 |
| `int_16` | — | 2 bytes | `0` | -32768 to 32767 |
| `int_32` | — | 4 bytes | `0` | -2³¹ to 2³¹-1 |
| `int_64` | `int` | 8 bytes | `0` | -2⁶³ to 2⁶³-1 |
| `uint_8` | — | 1 byte | `0` | 0 to 255 |
| `uint_16` | — | 2 bytes | `0` | 0 to 65535 |
| `uint_32` | — | 4 bytes | `0` | 0 to 2³²-1 |
| `uint_64` | — | 8 bytes | `0` | 0 to 2⁶⁴-1 |
| `float_32` | — | 4 bytes | `0.0` | IEEE 754 single |
| `float_64` | `float` | 8 bytes | `0.0` | IEEE 754 double |
| `complex_128` | — | 16 bytes | `0+0i` | (real, imag) as float_64 |
| `bool` | — | 1 byte | `false` | `true` / `false` |
| `char` | — | 1 byte | `'\0'` | Single character |
| `str` | — | heap | `""` | Immutable string |
| `void` | — | 0 | — | No value |
| `null` | — | 0 | `null` | Null reference |

---

## 3. Collection Types

### 3.1 List

```sfpp
var nums: list<int_64> = [1, 2, 3];
nums[0];            // index access
nums.push(4);       // append
len(nums);          // length
```

Lists are ordered, mutable, dynamically-sized sequences.

### 3.2 Dictionary

```sfpp
var ages: dict<str, int_64> = {"alice": 30, "bob": 25};
ages["alice"];       // key access
ages["carol"] = 28; // insert/update
```

Dictionaries are unordered key-value maps. Keys must be hashable.

### 3.3 Set

```sfpp
var s: set<int_64> = set{1, 2, 3};
s.add(4);
s.contains(2);      // true
```

### 3.4 Matrix

```sfpp
var m: matrix<float_64> = [[1.0, 2.0], [3.0, 4.0]];
m[0][1];            // 2.0
```

---

## 4. User-Defined Types

### 4.1 Classes

Class instances are reference types allocated on the heap. Fields are accessed via `.` and methods are bound to the instance.

### 4.2 Structs

Structs are value types with named fields:
```sfpp
struct Point {
    x: float_64;
    y: float_64;
}
```

### 4.3 Interfaces

Interfaces define method contracts. A class implementing an interface must provide all declared methods.

---

## 5. Nullable Types

Any type may be suffixed with `?` to indicate it can hold `null`:

```sfpp
var name: str? = null;     // OK
var count: int_64 = null;  // Type error
```

Operations on nullable types require null-checking or the `??` operator.

---

## 6. Pointer Types

`ptr<T>` represents a raw pointer to a value of type `T`. Pointer creation (`&`), dereferencing (`*`), and deallocation (`delete`) are only permitted inside `unsafe` blocks.

---

## 7. Function Types

```sfpp
var callback: fn(int_64, int_64) -> int_64 = fn(a, b) { return a + b; };
```

Function types encode parameter types and return type.

---

## 8. Type Compatibility & Conversion

### 8.1 Implicit Widening

| From | To | Condition |
|------|----|-----------|
| `int_8` | `int_16` / `int_32` / `int_64` | Always safe |
| `int_N` | `float_64` | Always safe (may lose precision for large int_64) |
| `float_32` | `float_64` | Always safe |
| `T` | `T?` | Always safe (value → nullable) |

### 8.2 Explicit Conversion

Narrowing conversions require explicit cast or conversion function:
```sfpp
var x: int_32 = toInt32(bigValue);
```

### 8.3 String Conversion

`toStr(value)` converts any value to its string representation. String concatenation with `+` auto-converts the right operand if the left is `str`.

---

## 9. Gradual Typing Model

Sulfur++ v0.1 uses **gradual typing**:

- **Untyped variables** (`let x = 42;`) default to `any` and bypass static checks.
- **Typed variables** (`let x: int_64 = 42;`) enable compile-time type enforcement.
- The semantic analyzer checks types where annotations are present and defers to runtime where they are absent.
- `any` is compatible with all types in both directions (assignable to and from).

---

## 10. Type Inference

When a variable is declared without a type annotation but with an initializer, the type is inferred from the initializer expression:

```sfpp
let x = 42;          // inferred: int_64
let y = 3.14;        // inferred: float_64
let z = "hello";     // inferred: str
let w = [1, 2, 3];   // inferred: list<int_64>
let v = true;        // inferred: bool
```

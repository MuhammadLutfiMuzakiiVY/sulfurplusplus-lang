# Complete Master Syntax Specification (FULL_SYNTAX_OUTLINE.md)

Dokumen ini mendefinisikan secara menyeluruh seluruh elemen sintaksis, struktur bahasa, tipe data, pemrosesan fungsional, metaprogramming, dan antarmuka standar untuk **Sulfur++**.

---

## 1. Basic Syntax & Structure

### 1.1 Indentation & Blocks
Sulfur++ mendukung eksekusi berbasis blok dengan tanda kurung kurawal `{}` atau alur berbasis baris/indentasi yang konsisten.

### 1.2 Comments
* Single-line: `// comment` atau `# comment`
* Multi-line: `/* comment */` atau `""" comment """`

### 1.3 Statements & Expressions
* **Statement**: Instruksi yang mengeksekusi aksi (misal: `let x = 10;`, `if`, `while`).
* **Expression**: Konstruksi yang mengevaluasi suatu nilai (misal: `a + b`, `x := 5`, ternary `a ? b : c`).

### 1.4 Keywords & Identifiers
* Keywords: `fn`, `def`, `let`, `var`, `const`, `if`, `elif`, `else`, `match`, `case`, `for`, `while`, `break`, `continue`, `pass`, `try`, `except`, `finally`, `raise`, `assert`, `import`, `from`, `as`, `class`, `self`, `this`, `super`, `global`, `nonlocal`, `with`, `async`, `await`, `yield`, `lambda`, `in`, `is`, `and`, `or`, `not`.
* Identifiers: Huruf `[a-zA-Z_]` diikuti `[a-zA-Z0-9_]`.

---

## 2. Variables & Constants

```sfpp
// Variable assignment
let x = 10;              // Immutable local
var y = 20;              // Mutable local
const MAX_LIMIT = 1000;   // Compile-time constant

// Multiple assignment & Unpacking
let (a, b, c) = (1, 2, 3);
let [head, *tail] = [10, 20, 30, 40];

// Type annotations
let name: str = "Sulfur++";
let age: int_64 = 21;
```

---

## 3. Data Types System

| Tipe Data | Deskripsi | Contoh Expression / Literal |
|---|---|---|
| `int` / `int_64` | Bilangan bulat presisi | `42`, `-100` |
| `float` / `float_64` | Bilangan desimal 64-bit | `3.14159` |
| `complex` / `complex_128` | Bilangan kompleks | `1.0 + 2.0i` |
| `bool` | Boolean | `true`, `false` |
| `str` | Text UTF-8 | `"Sulfur++"` |
| `list` | Dynamic Array Mutable | `[1, 2, 3]` |
| `tuple` | Fixed Array Immutable | `(1, 2, 3)` |
| `set` | Unique Collection Mutable | `{1, 2, 3}` |
| `frozenset` | Immutable Set | `frozenset({1, 2, 3})` |
| `dict` | Key-Value Hash Map | `{"key": "value"}` |
| `bytes` | Immutable Byte Sequence | `b"hello"` |
| `bytearray` | Mutable Byte Array | `bytearray([65, 66])` |
| `None` / `null` | Absence of value | `None` / `null` |

---

## 4. Operators

* **Arithmetic**: `+`, `-`, `*`, `/`, `%`, `**` (Power), `//` (Floor div)
* **Comparison**: `==`, `!=`, `<`, `<=`, `>`, `>=`, Chained (`18 <= age < 65`)
* **Assignment**: `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `**=`
* **Logical**: `&&` (`and`), `||` (`or`), `!` (`not`)
* **Bitwise**: `&`, `|`, `^`, `~`, `<<`, `>>`
* **Membership**: `in`, `not in`
* **Identity**: `is`, `is not`
* **Walrus Operator (`:=`)**: `if (n := len(data)) > 0 { print(n); }`

---

## 5. Control Flow & Loops

```sfpp
// Conditionals
if age >= 18 {
    print("Adult");
} elif age >= 13 {
    print("Teenager");
} else {
    print("Child");
}

// Pattern Matching
match command {
    case "start" => start_server(),
    case "stop"  => stop_server(),
    case _       => pass
}

// Loops
for i in 0..10 {
    if i == 5 { break; }
    if i % 2 == 0 { continue; }
    print(i);
}

while condition {
    // ...
}
```

---

## 6. Functions & Comprehensions

### 6.1 Function Signatures, `*args`, `**kwargs`
```sfpp
fn process(data: str, *args, **kwargs) -> bool {
    return true;
}
```

### 6.2 Comprehensions
```sfpp
// List Comprehension
let squares = [x ** 2 for x in 0..10 if x % 2 == 0];

// Set Comprehension
let unique_chars = {char for char in "sulfurplusplus"};

// Dict Comprehension
let char_map = {c: idx for (idx, c) in enumerate("sulfur")};

// Generator Expression
let gen = (x * 2 for x in 0..100);
```

---

## 7. Strings & Formatting

* **f-string**: `f"Hello ${name}, age: {age}"`
* **Raw String**: `r"C:\Users\Path"`
* **Multiline String**: `"""Multi-line text"""`
* **Slicing**: `text[0:5]`, `text[::-1]`

---

## 8. OOP & Special Dunder Methods

```sfpp
class User {
    private var _name: str;
    pub var age: int_64;

    init(name: str, age: int_64) {
        self._name = name;
        self.age = age;
    }

    // Special Dunder Methods
    fn __str__() -> str { return f"User({self._name})"; }
    fn __len__() -> int_64 { return len(self._name); }
    fn __eq__(other: User) -> bool { return self._name == other._name; }
    fn __add__(other: User) -> int_64 { return self.age + other.age; }
}
```

---

## 9. Decorators & Context Managers

### 9.1 Decorators (`@decorator`)
```sfpp
@log_execution
@timing
fn calculate() {
    // ...
}
```

### 9.2 Context Managers (`with / as`)
```sfpp
with open("file.txt", "r") as f {
    let content = f.read();
}
```

---

## 10. Type System & Async Programming

### 10.1 Advanced Type Hints
`Optional[T]`, `Union[T1, T2]`, `Any`, `Generic[T]`, `TypeVar("T")`, `Protocol`, `TypedDict`.

### 10.2 Async & Concurrency
```sfpp
async fn fetchData(url: str) -> str {
    let data = await http.get(url);
    return data;
}

// Async Generator & Comprehension
async fn number_stream() {
    yield 42;
}
```

---

## 11. Functional Programming & Metaprogramming

* **Functional Utilities**: `map()`, `filter()`, `reduce()`, `zip()`, `enumerate()`
* **Metaprogramming**: `getattr()`, `setattr()`, `hasattr()`, `delattr()`, `eval()`, `exec()`, Metaclasses.

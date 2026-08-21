# Sulfur++ Core Syntax Specification (CORE_SYNTAX_SPEC.md)

**Specification Version:** 0.1-CORE  
**Status:** Frozen Core Syntax Specification & Lexical Foundation  
**Target:** Compiler Frontend (Lexer, Parser & AST Builder)

---

## 41. Lexical Syntax
* **Identifier**: `[a-zA-Z_][a-zA-Z0-9_]*`
* **Keyword**: Fixed reserved words (see Section 42).
* **Literals**: Integer (`10`), Float (`3.14`), String (`"text"`), Char (`'A'`), Bool (`true`/`false`), Null (`null`).
* **Operators & Delimiters**: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `&&`, `||`, `!`, `??`, `?:`, `|=>`, `->`, `=`, `+=`, `-=`, `*=`, `/=`, `(`, `)`, `{`, `}`, `[`, `]`, `:`, `;`, `,`, `.`.
* **Comments**: Single-line `//`, Multi-line `/* */`, Docstrings `///`.
* **Whitespace & Newline**: Spaces, tabs, and newline characters act as token boundaries.

---

## 42. Keywords
Official core keywords:
```text
let       var       const     fn        return    if        else      
for       while     break     continue  match     case      class     
struct    enum      interface trait     import    from      as        
export    public    private   protected static    async     await     
yield     try       catch     finally   throw     true      false     
null
```

---

## 43. Variable Declaration
* Immutables (`let`), Mutables (`var`), Constants (`const`).
* Explicit type annotation: `let name: String = "Sulfur"`
* Type inference: `let age = 20`

---

## 44. Function Grammar
```sfpp
fn add(a: Int, b: Int) -> Int {
    return a + b
}

fn greet(name: String = "User") {
}

fn sum(...numbers: Int) {
}
```

---

## 45. Expression Grammar
* Literals, identifiers, function calls, binary/unary expressions, assignments, conditional expressions, collection literals, lambda expressions.
```sfpp
let result = (10 + 20) * 2
```

---

## 46. Operator Precedence Table (Highest to Lowest)
1. `()` `[]` `.`
2. `**`
3. `!` `~` `-` (Unary)
4. `*` `/` `%`
5. `+` `-`
6. `<` `<=` `>` `>=`
7. `==` `!=`
8. `&&`
9. `||`
10. `??`
11. `?:`
12. `=` `+=` `-=` `*=` `/=`

---

## 47. Block Syntax
```sfpp
if condition {
    statement
}

fn main() {
    statement
}

class User {
    field: String
}
```

---

## 48. Collection Syntax
```sfpp
let numbers = [1, 2, 3, 4]
let user = { "name": "Lutfi", "age": 20 }
let point = (10, 20)
let values = {1, 2, 3}
```

---

## 49. Indexing & Property Access
```sfpp
numbers[0]
numbers[1:4]
user.name
user.greet()
```

---

## 50. Control Flow Grammar
```sfpp
if condition {
} else if condition {
} else {
}

for item in items {
}

for i in 0..10 {
}

while condition {
}
```

---

## 51. Match System
```sfpp
match value {
    case 1 {
    }
    case 2 {
    }
    case _ {
    }
}

match age {
    case x if x >= 18 {
    }
    case _ {
    }
}
```

---

## 52. Error Handling
```sfpp
try {
    risky_operation()
} catch error {
    print(error)
} finally {
    cleanup()
}

throw Error("Something went wrong")
```

---

## 53. Module System
```sfpp
module user

import math
from math import sqrt
import mathematics as math

export fn calculate() {
}
```

---

## 54. Class System
```sfpp
class User {
    name: String
    age: Int

    fn init(name: String, age: Int) {
        self.name = name
        self.age = age
    }

    fn greet() {
        print(self.name)
    }
}
```

---

## 55. Inheritance & Interfaces
```sfpp
class Admin extends User {
}

interface Printable {
    fn print()
}

class User implements Printable {
    fn print() {
    }
}
```

---

## 56. Struct & Enum
```sfpp
struct User {
    name: String
    age: Int
}

enum Status {
    Active
    Inactive
    Pending
}
```

---

## 57. Generics
```sfpp
fn identity<T>(value: T) -> T {
    return value
}

let users: List<User>
```

---

## 58. Async/Await
```sfpp
async fn fetch_data() -> Data {
    let data = await fetch()
    return data
}
```

---

## 59. Lambda Expressions
```sfpp
let add = fn(a, b) => a + b
let square = (x) => x * x
```

---

## 60. Generators & Yield
```sfpp
fn numbers() {
    yield 1
    yield 2
    yield 3
}
```

---

## 61. Decorators / Attributes
```sfpp
@deprecated
fn old_function() {
}

@test
fn test_add() {
}
```

---

## 62. Visibility Modifiers
```sfpp
public fn greet() {
}

private fn calculate() {
}

protected fn internal() {
}
```

---

## 63. Comments Syntax
```sfpp
// Single-line comment

/*
    Multi-line comment
*/

/// Documentation comment for functions/types.
```

---

## 64. Main Entry Point
Official entry point:
```sfpp
fn main() {
    print("Hello, Sulfur++")
}
```

---

## 65. Standard Built-ins
Minimal global built-ins:
`print()`, `input()`, `len()`, `range()`, `type()`, `str()`, `int()`, `float()`, `bool()`.

---

## 66. Program Structure
```
module
│
├── imports
├── constants
├── types
├── structs
├── enums
├── classes
├── functions
└── main()
```

---

## 67. Next Stage: EBNF Grammar v0.1 & Implementation Pipeline
With this core syntax finalized, the language transition follows:
`Syntax Specification → EBNF Grammar v0.1 → Lexer → Token → Parser → AST → Semantic Analyzer → Type Checker → Interpreter/Compiler → Runtime → Standard Library → CLI → Package Manager → Testing`.

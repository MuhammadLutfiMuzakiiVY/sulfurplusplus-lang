# Sulfur++ Official Language Specification (LANGUAGE_SPEC.md)

**Specification Version:** 1.0.0-OFFICIAL  
**Status:** Core Structural Specification  
**Philosophy:** *"Write Easy, Perform Fast"* — High-performance modern language for systems, data science, and developer toolchains.  
**Formal Grammar:** [**`docs/GRAMMAR.ebnf`**](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/docs/GRAMMAR.ebnf) (EBNF Grammar v0.1)  
**Core Syntax Specification:** [**`docs/CORE_SYNTAX_SPEC.md`**](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/docs/CORE_SYNTAX_SPEC.md) (Sections 41 - 67)  
**Full Lexer Specification:** [**`docs/LEXER_FULL_SPEC.md`**](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/docs/LEXER_FULL_SPEC.md) (Sections 114 - 145)  
**Full AST Specification:** [**`docs/AST_FULL_SPEC.md`**](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/docs/AST_FULL_SPEC.md) (Sections 146 - 185)  
**Full Parser Specification:** [**`docs/PARSER_FULL_SPEC.md`**](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/docs/PARSER_FULL_SPEC.md) (Sections 186 - 228)  
**Semantic Analyzer Specification:** [**`docs/SEMANTIC_ANALYZER_SPEC.md`**](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/docs/SEMANTIC_ANALYZER_SPEC.md) (Sections 229 - 242)

---

## 1. Basic Syntax
Indentation, comments, statements, expressions, identifiers, keywords, and literals.
```sfpp
// Single-line comment
/* Multi-line comment */

fn main() {
    print("Hello, Sulfur++");
}
```

## 2. Variables & Constants
Variable declarations, assignment, multiple assignments, constants, type annotations, and destructuring.
```sfpp
variable name: String = "Sulfur";
let age: Int = 20;
var counter: Int = 0;
const MAX_LIMIT: Int = 1000;

let (a, b) = (1, 2);
```

## 3. Data Types
Primitives, collections, enums, structs, objects, null, and byte arrays.
```sfpp
let i: Int = 42;
let f: Float = 3.14;
let b: Bool = true;
let s: String = "Text";
let n: Null = null;
```

## 4. Operators
Arithmetic, assignment, comparison, logical, bitwise, inc/dec, membership, identity, range, null-coalescing, ternary.
```sfpp
let sum = a + b;
let isValid = (age >= 18) && (status == true);
let val = name ?? "Default";
let result = isAdult ? "Yes" : "No";
```

## 5. Control Flow
`if`, `else if`, `else`, `match`, `case`, conditional expressions.
```sfpp
if name == "Sulfur" {
    print("Hello");
} else if name == "Rust" {
    print("Fast");
} else {
    print("Welcome");
}
```

## 6. Loops
`for`, `while`, infinite loop, `break`, `continue`, `loop`.
```sfpp
for item in items {
    print(item);
}

while count < 10 {
    count += 1;
}
```

## 7. Functions
Declarations, parameters, arguments, returns, default params, named args, variadic, lambdas, recursion, generic functions.
```sfpp
fn add(a: Int, b: Int) -> Int {
    return a + b;
}

fn greet(name: String = "World") {
    print("Hello " + name);
}
```

## 8. Comprehensions
List, set, map comprehensions, generator expressions.
```sfpp
let squares = [x * x for x in items];
let evens = {x for x in items if x % 2 == 0};
```

## 9. Strings
Literals, multiline, raw strings, interpolation, formatting, string operations.
```sfpp
let text = "Hello ${name}";
let rawPath = r"C:\Sulfur\bin";
let multi = """
Line 1
Line 2
""";
```

## 10. Collections
Array, list, tuple, set, map, stack, queue, iterators, generators.
```sfpp
let listData: List<Int> = [1, 2, 3];
let mapData: Map<String, Int> = {"math": 90};
let setData: Set<String> = {"A", "B"};
```

## 11. Error & Exception Handling
`try`, `catch`, `finally`, `throw`, `raise`, custom exceptions, assertions, error types.
```sfpp
try {
    let file = open("data.txt");
} catch error {
    print(error);
} finally {
    close();
}
```

## 12. Modules
`import`, `from`, `as`, module declarations, package declarations, exports, re-exports.
```sfpp
import std.io;
import std.math as math;
from std.http import Client;
```

## 13. Object-Oriented Programming
Classes, objects, constructors, destructors, properties, methods, inheritance, interfaces, polymorphism, traits.
```sfpp
class User {
    name: String;

    init(name: String) {
        self.name = name;
    }

    fn greet() {
        print("Hello " + self.name);
    }
}
```

## 14. Struct & Data Modeling
Struct fields, methods, constructors, immutable/mutable structs, data classes.
```sfpp
struct Point {
    x: Float;
    y: Float;
}

let p = Point { x: 10.0, y: 20.0 };
```

## 15. Generics
Generic types, generic functions, type parameters, constraints, generic collections.
```sfpp
fn identity<T>(value: T) -> T {
    return value;
}
```

## 16. Type System
Static typing, dynamic typing, type inference, type aliases, union types, optional types.
```sfpp
type UserId = Int;
let name: String? = null;
```

## 17. Scope
Global, local, function, block, module scope, closures, `global`, `nonlocal`.
```sfpp
fn outer() -> fn {
    let x = 10;
    return || { return x + 1; };
}
```

## 18. Decorators / Attributes
Function decorators, class decorators, attribute annotations, metadata.
```sfpp
@inline
fn fastAdd(a: Int, b: Int) -> Int {
    return a + b;
}
```

## 19. Context Management
`with`, resource management, context managers, automatic cleanup.
```sfpp
with open("data.txt") as f {
    let content = f.read();
}
```

## 20. Iterators & Generators
Iterator, generator, `yield`, `yield from`, lazy evaluation.
```sfpp
fn generator() {
    yield 1;
    yield 2;
}
```

## 21. Async Programming
`async`, `await`, async functions, async iterators, async generators, tasks, futures.
```sfpp
async fn fetchData(url: String) -> String {
    let res = await http.get(url);
    return res;
}
```

## 22. Concurrency
Threading, processes, async tasks, channels, locks, mutex, semaphore.
```sfpp
spawn {
    process();
}
```

## 23. File & I/O
File open, read, write, append, binary I/O, stdin, stdout, stderr.
```sfpp
let content = io.readFile("config.json");
io.stdout.write("Done\n");
```

## 24. Pattern Matching
`match`, `case`, wildcards, guards, destructuring patterns, type patterns.
```sfpp
match val {
    case 1 => print("One"),
    case 2 => print("Two"),
    case _ => print("Other")
}
```

## 25. Functional Programming
Lambda, map, filter, reduce, fold, zip, enumerate, higher-order functions.
```sfpp
let evens = filter(|x| x % 2 == 0, items);
```

## 26. Memory Management
References, ownership, borrowing, garbage collection, smart pointers, manual memory management (`unsafe`).
```sfpp
unsafe {
    let p = alloc<Int>();
    free(p);
}
```

## 27. Metaprogramming
Reflection, introspection, dynamic attributes, macros, code generation, runtime execution.
```sfpp
let hasField = hasattr(obj, "name");
```

## 28. Attributes & Properties
Object attributes, static attributes, computed properties, getters, setters, visibility modifiers (`pub`, `private`).
```sfpp
pub class Account {
    pub name: String;
    private balance: Float;
}
```

## 29. Standard Library
Math, string, collections, filesystem, networking, HTTP, JSON, regex, date/time, cryptography, processes.
```sfpp
import std.math;
import std.json;
```

## 30. Package Management
Package declaration, dependency resolution, versioning, lockfile (`sfpp-lock.toml`), package registry.
```sfpp
// sfpp-project.toml
name = "myapp"
version = "1.0.0"
```

## 31. CLI
Command-line arguments, environment variables, exit codes, CLI parser, subcommands.
```sfpp
let args = sys.args();
```

## 32. Testing
Unit testing, integration testing, assertions, test fixtures, mocking, benchmarking.
```sfpp
@test
fn testAdd() {
    assert(add(2, 3) == 5);
}
```

## 33. Documentation
Docstrings, API documentation, type documentation.
```sfpp
/// Computes the sum of two integers.
fn add(a: Int, b: Int) -> Int { return a + b; }
```

## 34. Compiler / Interpreter Features
Lexer, parser, AST, semantic analysis, type checker, IR, optimizer, code generator, runtime.
```
Source -> Lexer -> Parser -> AST -> Resolver -> LLVM IR -> Binary
```

## 35. Tooling
Formatter, linter, Language Server (LSP), REPL, debugger, package manager, build system.
```sfpp
combust script.sfpp --debug
```

## 36. Advanced Language Features
Closures, generators, coroutines, traits, interfaces, operator overloading, macros, compile-time evaluation.
```sfpp
comptime {
    const BUILD_TAG = "v1.0.0";
}
```

## 37. Security Features
Memory safety, type safety, sandboxing, secure random, cryptographic APIs, permission model, unsafe boundary.
```sfpp
unsafe {
    // Isolated raw pointer operations
}
```

## 38. Interoperability
C/C++ FFI, Rust FFI, Python interop, WebAssembly, native libraries.
```sfpp
extern "C" {
    fn printf(fmt: String, ...) -> Int;
}
```

## 39. Runtime
Runtime environment, memory manager, exception runtime, async runtime, thread runtime, module loader.
```sfpp
// Sulfur++ Engine Runtime Init
```

## 40. Language Specification
Lexical grammar, token spec, EBNF grammar, operator precedence, type system spec, runtime semantics, memory model.
```
Single Source of Truth: docs/GRAMMAR.ebnf
```

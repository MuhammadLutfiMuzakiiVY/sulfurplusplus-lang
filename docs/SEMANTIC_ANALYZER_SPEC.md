# Sulfur++ Semantic Analyzer Specification (SEMANTIC_ANALYZER_SPEC.md)

**Specification Version:** 0.1-SEMANTIC (Components 229 - 242)  
**Status:** Official Semantic Analyzer & Type Checker Specification  
**Target File:** [`include/resolver.hpp`](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/include/resolver.hpp) & [`src/core/resolver.cpp`](file:///c:/Users/muham/OneDrive/Documents/NANXIAN/sulfurplusplus-lang/src/core/resolver.cpp)

---

## 229. Semantic Analyzer Architecture

Semantic Analyzer memproses AST untuk memverifikasi kebenaran program secara konteks (*semantic correctness*), membuat **Symbol Table**, menyelesaikan **Scope Resolution**, dan melakukan **Type Checking** sebelum eksekusi/kompilasi IR.

```
AST Node -> Symbol Table & Scope Stack -> Type Checker & Inferencer -> Semantic Diagnostics -> Verified AST
```

---

## 2. Symbol Table & Scope Resolution (230 - 232)

### 230 - 231. Scope Stack & Symbol Information
Setiap blok `{ ... }` membuat `Scope` baru dalam Stack (`scopes_`):

```cpp
struct SymbolInfo {
    bool isMutable;     // let vs var vs const
    bool isDefined;     // Inisialisasi variabel
    int line;           // Koordinat deklarasi
    std::string typeName; // Type Hint (misal: "Int", "String", "User")
};

class ScopeStack {
    std::vector<std::unordered_map<std::string, SymbolInfo>> scopes_;
    void beginScope();
    void endScope();
    void declare(const std::string& name, bool isMutable, int line, const std::string& typeName);
    SymbolInfo* lookupSymbol(const std::string& name);
};
```

### 232. Name Resolution & Shadowing
* Symbol dicari dari inner scope $\to$ outer scope $\to$ global scope.
* Re-declaration dalam scope yang sama dilarang (`E_SEMANTIC_401`).
* Variable shadowing di inner scope diizinkan tanpa mengubah outer scope.

---

## 3. Type Checking & Inference (233 - 239)

### 233 - 234. Type Checker & Type Inference Algorithm
* **Immutability Check**: Re-assignment variabel `let` ditolak (`E_SEMANTIC_400`).
* **Type Inference**: `inferExprType(expr)` menentukan tipe data otomatis dari literal, operasi biner, dan return fungsi.
* **Compatibility Check**: `isTypeCompatible(expected, actual)` memverifikasi assignment dan argumen fungsi.

### 235 - 239. Structural Type Checking
* **Generic Checking**: Verifikasi tipe parameter pada `List<T>` dan `Map<K, V>`.
* **Function Checking**: Parameter count & argumen type compatibility, return type matching.
* **Class & Struct Checking**: Verifikasi member access, constructor `init()`, inheritance (`extends`), dan interface implementation (`implements`).

---

## 4. Control Flow & Memory Rules (240 - 242)

* **Control Context Validation**: Verifikasi bahwa `break` & `continue` berada di dalam perulangan (`LoopContext`), serta `return` berada di dalam fungsi (`FunctionContext`).
* **Unsafe Boundaries**: Verifikasi bahwa operasi pointer mentah (`*p`, `&x`) hanya terjadi di dalam blok `unsafe`.
* **Semantic Diagnostics Format**:

```text
error[E_SEMANTIC_400]: cannot reassign to immutable variable `age` declared with `let`
 --> main.sfpp:10:5
  |
10 | age = 21;
  | ^^^
```

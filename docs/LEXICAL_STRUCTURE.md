# Lexical Structure Specification (LEXICAL_STRUCTURE.md)

Dokumen ini mendefinisikan aturan leksikal, tokenisasi, komentar, dan literal string untuk **Sulfur++**.

---

## 1. Komentar (Comments)

```sfpp
// Single-line comment

/* 
   Multi-line comment
   Dapat mencakup beberapa baris
*/
```

---

## 2. Literals

### 2.1 String Literals & String Interpolation

1. **Standard String & Interpolation**:
   String standar menggunakan tanda petik ganda `"..."`. Variabel atau ekspresi di dalam `${...}` diinterpolasi secara otomatis:
   ```sfpp
   let name: str = "Lutfi";
   let age: int_64 = 21;
   print("Name: ${name}, Age: ${age}");
   ```

2. **Multiline String (`"""..."""`)**:
   String multi-baris mempertahankan format spasi dan karakter newline:
   ```sfpp
   let text = """
   Hello Sulfur++ World!
   This is a multiline string.
   """;
   ```

3. **Raw String (`r"..."`)**:
   Raw string mengabaikan escape character (`\n`, `\t`, `\\`):
   ```sfpp
   let path: str = r"C:\Users\Lutfi\Projects";
   ```

### 2.2 Numeric Literals
* Integer: `42`, `-10`, `0x1F` (hexadecimal), `0b1010` (binary)
* Float: `3.14159`, `-0.001`, `1e10`
* Complex: `1.0 + 2.0i`

### 2.3 Character & Boolean Literals
* Character: `'A'`, `'z'`, `'\n'`
* Boolean: `true`, `false`
* Null: `null`

---

## 3. Keywords & Identifiers

### Keywords
`let`, `var`, `const`, `fn`, `class`, `struct`, `enum`, `init`, `pub`, `private`, `protected`, `if`, `else`, `while`, `for`, `in`, `step`, `match`, `return`, `break`, `continue`, `try`, `catch`, `finally`, `throw`, `defer`, `unsafe`, `spawn`, `async`, `await`, `import`, `export`, `as`, `namespace`, `extern`, `comptime`, `true`, `false`, `null`, `this`.

### Identifiers
Identifiers harus diawali dengan huruf `a-z`, `A-Z`, atau `_`, diikuti oleh kombinasi huruf, angka, atau `_`.

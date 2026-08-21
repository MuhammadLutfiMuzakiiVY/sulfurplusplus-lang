# Standard Library Specification (STANDARD_LIBRARY.md)

Dokumen ini mendefinisikan arsitektur dan pustaka standar resmi untuk **Sulfur++**.

---

## 1. Modul Utama Standard Library

| Modul | Deskripsi |
|---|---|
| `std.io` | I/O Terminal, Pembacaan/Penulisan File |
| `std.math` | Operasi Matematika, Trigonometri, Kompleks |
| `std.http` | HTTP Client & Server Engine |
| `std.crypto` | Hashing (SHA256, MD5) & Encoding (Base64, Hex) |
| `std.sys` | Informasi OS, Hardware, dan Process Management |
| `std.path` | Pengolahan Filesystem Path Utilities |
| `std.json` | JSON Parsing & Stringification |
| `std.regex` | Regular Expression Engine |

---

## 2. Contoh Penggunaan Standard Library

```sfpp
import std.io;
import std.math::{sqrt};
import std.http as http;

fn main() {
    print("Sqrt of 16: ${sqrt(16.0)}");
}
```

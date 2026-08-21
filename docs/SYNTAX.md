# Base Syntax Specification (SYNTAX.md)

Dokumen ini mendefinisikan struktur program dasar dan sintaksis inti **Sulfur++**.

---

## 1. Struktur Program Utama

Setiap executable Sulfur++ dimulai dari fungsi `main()`:

```sfpp
import std.io;

fn main() {
    print("Hello, Sulfur++");
}
```

---

## 2. Struktur File & Modul

Sebuah file `.sfpp` dapat berisi deklarasi import, namespace, fungsi, struct, class, dan variabel global:

```sfpp
namespace app.main;

import std.io;
import std.math::{sqrt, sin};

const VERSION: str = "1.0.0";

fn main() {
    print("Sulfur++ Version: ${VERSION}");
}
```

---

## 3. Blok Kode & Semicolon

* Blok kode ditutup dengan tanda kurung kurawal `{ ... }`.
* Pernyataan/Statement diakhiri dengan semicolon `;` opsional pada akhir baris atau wajib jika berada di baris yang sama.

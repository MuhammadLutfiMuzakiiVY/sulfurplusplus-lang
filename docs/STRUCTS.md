# Structs Specification (STRUCTS.md)

Dokumen ini mendefinisikan tipe komposit `struct` untuk *Plain Old Data* (POD) di **Sulfur++**.

---

## 1. Deklarasi Struct

`struct` adalah tipe value yang ringan, disimpan secara kontigu di stack tanpa overhead pointer kelas:

```sfpp
struct Point {
    x: float_64;
    y: float_64;
}
```

---

## 2. Inisialisasi & Penggunaan

Instansiasi `struct` menggunakan nama struct diikuti oleh field initializer `{ key: value }`:

```sfpp
let p = Point { x: 10.0, y: 20.0 };
print(p.x);
print(p.y);
```

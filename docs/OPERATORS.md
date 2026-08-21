# Operators & Precedence Specification (OPERATORS.md)

Dokumen ini mendefinisikan operator, perilaku evaluasi, dan tabel presedensi resmi untuk **Sulfur++**.

---

## 1. Daftar Operator

### 1.1 Aritmatika (Arithmetic)
* `+` : Penjumlahan
* `-` : Pengurangan
* `*` : Perkalian
* `/` : Pembagian
* `%` : Modulo (Sisa Bagi)
* `**` : Eksponensiasi (Pangkat)

### 1.2 Perbandingan (Comparison)
* `==` : Sama Dengan
* `!=` : Tidak Sama Dengan
* `<` : Lebih Kecil
* `<=` : Lebih Kecil atau Sama Dengan
* `>` : Lebih Besar
* `>=` : Lebih Besar atau Sama Dengan

### 1.3 Logika (Logical)
* `&&` : Logical AND
* `||` : Logical OR
* `!` : Logical NOT

### 1.4 Bitwise
* `&` : Bitwise AND
* `|` : Bitwise OR
* `^` : Bitwise XOR
* `~` : Bitwise NOT
* `<<` : Left Shift
* `>>` : Right Shift

### 1.5 Penugasan & Inc/Dec (Assignment & Inc/Dec)
* `=` `+=` `-=` `*=` `/=` `%=`
* `x++` `x--` `++x` `--x`

### 1.6 Special Operators
* `|>` : Pipeline Operator
* `??` : Null Coalescing Operator
* `? :` : Ternary Operator

---

## 2. Tabel Presedensi Operator (Operator Precedence)

Tabel diurutkan dari presedensi tertinggi (1) ke terendah (13):

| Level | Operator | Deskripsi | Asosiativitas |
|---|---|---|---|
| 1 | `()` `[]` `.` | Call, Index, Member Access | Left-to-Right |
| 2 | `!` `~` `-` `+` `++` `--` `&` `*` `await` | Unary Operator, Deref, Addr-of | Right-to-Left |
| 3 | `**` | Eksponensiasi (Pangkat) | Right-to-Left |
| 4 | `*` `/` `%` | Multiplikasi, Divisi, Modulo | Left-to-Right |
| 5 | `+` `-` | Adisi, Subtraksi | Left-to-Right |
| 6 | `<<` `>>` | Bitwise Shift Left / Right | Left-to-Right |
| 7 | `<` `<=` `>` `>=` | Komparasi Relasional | Left-to-Right |
| 8 | `==` `!=` | Komparasi Kesetaraan | Left-to-Right |
| 9 | `&` `^` `\|` | Bitwise AND, XOR, OR | Left-to-Right |
| 10| `&&` `\|\|` | Logical AND, Logical OR | Left-to-Right |
| 11| `??` | Null Coalescing | Left-to-Right |
| 12| `\|>` | Pipeline Operator | Left-to-Right |
| 13| `? :` | Ternary Conditional | Right-to-Left |
| 14| `=` `+=` `-=` `*=` `/=` `%=` | Penugasan (Assignment) | Right-to-Left |

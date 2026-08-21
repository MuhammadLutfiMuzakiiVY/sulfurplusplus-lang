# Type System Specification (TYPES.md)

Dokumen ini mendefinisikan seluruh tipe data primitif, koleksi generik, dan tipe opsional di **Sulfur++**.

---

## 1. Tipe Primitif (Primitive Types)

Sulfur++ mendukung tipe data presisi spesifik untuk pemrograman sistem:

| Tipe Data | Deskripsi | Rentang / Format |
|---|---|---|
| `null` | Nilai kosong | `null` |
| `bool` | Boolean | `true`, `false` |
| `int_8` | 8-bit Signed Integer | -128 s.d. 127 |
| `int_16` | 16-bit Signed Integer | -32,768 s.d. 32,767 |
| `int_32` | 32-bit Signed Integer | -2,147,483,648 s.d. 2,147,483,647 |
| `int_64` | 64-bit Signed Integer | Standard integerdefault |
| `uint_8` | 8-bit Unsigned Integer | 0 s.d. 255 |
| `uint_16` | 16-bit Unsigned Integer | 0 s.d. 65,535 |
| `uint_32` | 32-bit Unsigned Integer | 0 s.d. 4,294,967,295 |
| `uint_64` | 64-bit Unsigned Integer | 0 s.d. 18,446,744,073,709,551,615 |
| `float_32` | 32-bit Single Precision Float | IEEE 754 float |
| `float_64` | 64-bit Double Precision Float | IEEE 754 double |
| `complex_128` | 128-bit Complex Number | Real & Imaginary (`1.0 + 2.0i`) |
| `char` | Unicode Character | Karakter 32-bit ('A', '🔥') |
| `str` | UTF-8 String | String teks dinamis |

---

## 2. Koleksi Generik (Typed Collections)

### 2.1 List / Array (`list<T>`)
```sfpp
let numbers: list<int_64> = [1, 2, 3, 4, 5];
print(numbers[0]);
numbers.push(6);
numbers.pop();

// Nested List
let matrix: list<list<int_64>> = [ [1, 2], [3, 4] ];
print(matrix[0][1]);
```

### 2.2 Dictionary (`dict<K, V>`)
```sfpp
let user = { "name": "Lutfi", "age": 21 };

// Typed Dictionary
let scores: dict<str, int_64> = { "math": 90, "english": 95 };
print(scores["math"]);
```

### 2.3 Set (`set<T>`)
```sfpp
let languages: set<str> = { "Sulfur++", "Rust", "C++" };
languages.add("Python");
languages.remove("Rust");
```

---

## 3. Optional & Nullable Types (`T?`)

Daripada membiarkan seluruh variabel rawan null-pointer, tipe nullable diidentifikasi secara eksplisit dengan `?`:

```sfpp
let name: str? = null;

if name != null {
    print(name);
}

// Null Coalescing Operator (??)
let value = name ?? "Unknown";
```

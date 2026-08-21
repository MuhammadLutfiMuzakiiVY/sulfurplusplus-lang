# Sulfur++ Control Flow Specification (CONTROL_FLOW.md)

Dokumen ini mendefinisikan struktur kontrol alur ekseksi pada bahasa **Sulfur++**.

---

## 1. Kondisional (`if`, `else if`, `else`)

Kondisi pada `if` tidak mewajibkan tanda kurung `()`, membuat sintaksis lebih bersih dan modern.

### 1.1 `if` Sederhana
```sfpp
let age: int = 20;

if age >= 18 {
    println("Adult");
}
```

### 1.2 `if` / `else if` / `else`
```sfpp
if age >= 21 {
    println("Adult (Full)");
} else if age >= 18 {
    println("Adult");
} else {
    println("Minor");
}
```

---

## 2. Perulangan (Loops)

### 2.1 `while` Loop
Menjalankan blok kode selama kondisi bernilai `true`:

```sfpp
let mut count: int = 0;

while count < 5 {
    println(count);
    count = count + 1;
}
```

### 2.2 `for ... in` Loop (Iterasi Koleksi)
Mengiterasi elemen dalam list atau range:

```sfpp
let fruits: [string] = ["Apple", "Banana", "Cherry"];

for fruit in fruits {
    println(fruit);
}
```

### 2.3 C-Style `for` Loop
Perulangan tradisional berbasis counter:

```sfpp
for let i = 0; i < 10; i++ {
    if i == 5 {
        break;
    }
    println(i);
}
```

---

## 3. Pattern Matching (`match`)

`match` menyediakan percabangan ekspresi yang aman dan deklaratif:

```sfpp
let code: int = 200;

match code {
    200 => println("OK"),
    404 => println("Not Found"),
    500 => println("Server Error"),
    _   => println("Unknown Code")
}
```

---

## 4. `break` & `continue`

* `break`: Menghentikan perulangan seketika.
* `continue`: Melompati sisa blok dan melanjutkan ke iterasi berikutnya.

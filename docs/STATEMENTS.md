# Statements Specification (STATEMENTS.md)

Dokumen ini mendefinisikan aturan statement, variabel, perulangan, dan kontrol alur di **Sulfur++**.

---

## 1. Deklarasi Variabel & Konstanta

Sulfur++ membedakan mutabilitas secara tegas:

1. **`let`**: Immutable (konstanta lokal; tidak dapat diassign ulang).
   ```sfpp
   let name: str = "Lutfi";
   let age: int_64 = 21;
   ```
2. **`var`**: Mutable (variabel yang nilainya dapat diubah).
   ```sfpp
   var counter: int_64 = 0;
   counter = counter + 1;
   ```
3. **`const`**: Compile-time constant (nilai harus dapat dievaluasi saat kompilasi).
   ```sfpp
   const MAX = 100;
   ```

---

## 2. Perulangan (Loops)

### 2.1 `while` Loop
```sfpp
while x < 10 {
    print(x);
    x++;
}
```

### 2.2 C-Style `for` Loop
```sfpp
for (let i = 0; i < 10; i++) {
    print(i);
}
```

### 2.3 `for-in` Collection Loop
```sfpp
for item in numbers {
    print(item);
}
```

### 2.4 Range Loop
```sfpp
for i in 0..10 {
    print(i);
}

for i in 0..100 step 5 {
    print(i);
}
```

---

## 3. `break` dan `continue`

```sfpp
for i in 0..100 {
    if i == 10 {
        break;
    }
    if i % 2 == 0 {
        continue;
    }
    print(i);
}
```

---

## 4. Pattern Matching (`match`)

`match` mengevaluasi ekspresi terhadap sekumpulan pola:

```sfpp
match value {
    0 => print("zero"),
    1 => print("one"),
    2 => print("two"),
    _ => print("other")
}

// Pattern matching pada struktur data / object
match user {
    { "role": "admin" } => print("Administrator"),
    { "role": "user" }  => print("User"),
    _                   => print("Unknown")
}
```

---

## 5. Defer Statement (`defer`)

Pernyataan `defer` menjadwalkan eksekusi pembersihan tepat saat blok/fungsi saat ini keluar (LIFO order):

```sfpp
fn process() {
    let file = open("data.txt");
    defer {
        file.close();
    }
    // Lakukan pemrosesan file...
}
```

---

## 6. Compile-Time Evaluation (`comptime`)

```sfpp
comptime {
    const VERSION = "0.1.0";
}
```

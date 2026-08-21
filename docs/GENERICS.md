# Generics Specification (GENERICS.md)

Dokumen ini mendefinisikan tipe dan fungsi generik (parameterized types) di **Sulfur++**.

---

## 1. Fungsi Generik (Generic Functions)

```sfpp
fn identity<T>(value: T) -> T {
    return value;
}

let num = identity<int_64>(42);
let text = identity("hello"); // Inferred type T = str
```

---

## 2. Struct Generik (Generic Structs)

```sfpp
struct Pair<T, U> {
    first: T;
    second: U;
}

// Inisialisasi
let pair: Pair<int_64, str> = Pair { 
    first: 10, 
    second: "hello" 
};
```

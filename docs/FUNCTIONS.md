# Functions & Closures Specification (FUNCTIONS.md)

Dokumen ini mendefinisikan deklarasi fungsi, argumen, lambda, dan closure di **Sulfur++**.

---

## 1. Deklarasi Fungsi

### 1.1 Fungsi Dasar dengan Return Type
```sfpp
fn add(a: int_64, b: int_64) -> int_64 {
    return a + b;
}
```

### 1.2 Fungsi Tanpa Return Value (`void`)
```sfpp
fn greet(name: str) {
    print("Hello " + name);
}
```

### 1.3 Default Parameter
```sfpp
fn greet(name: str = "World") {
    print("Hello " + name);
}
```

### 1.4 Named Arguments
Fungsi dapat dipanggil dengan menyebutkan nama parameter secara eksplisit:

```sfpp
greet(name: "Lutfi");
```

### 1.5 Variadic Functions (`...T`)
Menerima jumlah argumen variabel:

```sfpp
fn sum(values: ...int_64) -> int_64 {
    var total: int_64 = 0;
    for v in values {
        total += v;
    }
    return total;
}
```

---

## 2. Closures & Lexical Environment

Fungsi di Sulfur++ dapat menangkap lingkungan variabel leksikal (*closure capture*):

```sfpp
fn counter() -> fn {
    let value = 0;
    return || {
        value++;
        return value;
    };
}

let count = counter();
print(count()); // 1
print(count()); // 2
```

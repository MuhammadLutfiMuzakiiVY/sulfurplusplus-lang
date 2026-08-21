# Expressions Specification (EXPRESSIONS.md)

Dokumen ini mendefinisikan ekspresi, aturan evaluasi, dan fitur sintaksis ekspresi di **Sulfur++**.

---

## 1. Pipeline Operator (`|>`)

Pipeline operator menyalurkan hasil ekspresi kiri sebagai argumen pertama ke fungsi di sebelah kanan:

```sfpp
let result = data 
    |> filter(isValid) 
    |> map(transform) 
    |> sort() 
    |> collect();
```

---

## 2. Range Expressions (`..` & `step`)

Range digunakan untuk menggenerasi deret angka pada perulangan:

```sfpp
// Range standar 0 hingga 9
for i in 0..10 {
    print(i);
}

// Range dengan step
for i in 0..100 step 5 {
    print(i);
}
```

---

## 3. Ternary Operator (`? :`)

Ekspresi kondisional singkat:

```sfpp
let status = age >= 18 ? "adult" : "minor";
```

---

## 4. Lambda Expressions

Lambdas dideklarasikan menggunakan sintaksis `|params| -> ReturnType { ... }` atau bentuk ringkas `|params| expr`:

```sfpp
// Lambda eksplisit
let square = |x: int_64| -> int_64 { return x * x; };
print(square(5));

// Concise Lambda
let square_concise = |x| x * x;
```

---

## 5. Destructuring Expressions

Sulfur++ mendukung pembongkaran struktur data (*destructuring*):

### 5.1 Array / List Destructuring
```sfpp
let [a, b, c] = numbers;
```

### 5.2 Object / Struct Destructuring
```sfpp
let {name, age} = user;
```

### 5.3 Function Return Destructuring
```sfpp
fn getUser() -> (str, int_64) {
    return ("Lutfi", 21);
}

let (name, age) = getUser();
```

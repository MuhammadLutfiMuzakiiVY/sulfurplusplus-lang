# Enums & Algebraic Data Types Specification (ENUMS.md)

Dokumen ini mendefinisikan `enum` sebagai *Algebraic Data Types* (ADT) dengan dukungan payload data di **Sulfur++**.

---

## 1. Simple Enum

Enum dasar tanpa payload:

```sfpp
enum Status {
    Pending,
    Active,
    Completed
}
```

---

## 2. Enum dengan Data Payload (ADT)

Enum dapat membawa data variabel (*tagged union / payload*):

```sfpp
enum Result<T, E> {
    Ok(T),
    Err(E)
}
```

---

## 3. Penggunaan & Pattern Matching

Enum diinstansiasi dengan sintaksis `Enum::Variant` dan diproses dengan `match`:

```sfpp
let result = Result::Ok(100);

match result {
    Result::Ok(value) => print(value),
    Result::Err(error) => print(error)
}
```

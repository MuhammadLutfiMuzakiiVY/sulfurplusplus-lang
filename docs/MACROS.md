# Attributes, Annotations & Testing Specification (MACROS.md)

Dokumen ini mendefinisikan anotasi atribut (`@attribute`) dan sintaksis pengujian terintegrasi di **Sulfur++**.

---

## 1. Attributes & Annotations

Attributes diawali tanda `@` dan memberikan instruksi kepada compiler, linter, atau runtime:

```sfpp
@inline
fn add(a: int_64, b: int_64) -> int_64 {
    return a + b;
}

@deprecated("Use newFunction instead")
fn oldFunction() {
    // ...
}
```

---

## 2. Built-in Testing Syntax (`@test`)

Sulfur++ menyertakan runner pengujian tingkat pertama di dalam sintaksis bahasa:

```sfpp
@test
fn test_addition() {
    assert(add(2, 3) == 5);
}

@test
fn test_user() {
    let user = User("Lutfi", 21);
    assert(user.age == 21);
}
```

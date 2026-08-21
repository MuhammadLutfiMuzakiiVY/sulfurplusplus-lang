# Error Handling Specification (ERROR_HANDLING.md)

Dokumen ini mendefinisikan mekanisme penanganan error terstruktur (`try/catch`) dan sistem `Result<T, E>` pada **Sulfur++**.

---

## 1. Exception Handling (`try / catch / finally`)

 sulfur++ mendukung exception handling terstruktur:

```sfpp
try {
    let data = readFile("data.txt");
} catch error {
    print(error);
} finally {
    print("Done");
}
```

### Melemparkan Eksepsi (`throw`):
```sfpp
throw Error("File not found");
```

---

## 2. Result Pattern (`Result<T, E>`)

Untuk pemrograman sistem performa tinggi tanpa overhead stack unwinding, Sulfur++ menyediakan pola `Result<T, E>`:

```sfpp
fn readConfig() -> Result<Config, Error> {
    // ...
}
```

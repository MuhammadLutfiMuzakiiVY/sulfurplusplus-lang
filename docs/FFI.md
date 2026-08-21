# Foreign Function Interface Specification (FFI.md)

Dokumen ini mendefinisikan interoperabilitas C/C++ (FFI) di **Sulfur++**.

---

## 1. Extern C Declarations

Fungsi C native diimpor menggunakan blok `extern "C"`:

```sfpp
extern "C" {
    fn printf(format: ptr<char>, ...) -> int_32;
    fn malloc(size: uint_64) -> ptr<void>;
    fn free(ptr: ptr<void>);
}
```

---

## 2. Calling C Functions

```sfpp
fn main() {
    unsafe {
        printf("Hello from C FFI\n");
    }
}
```

# Memory Model & Garbage Collection Specification (MEMORY_MODEL.md)

Dokumen ini mendefinisikan aturan manajemen memori, mutabilitas, pointer, dan arsitektur **Generational Tri-color Mark-Sweep Garbage Collector (GC)** di **Sulfur++ v0.1**.

---

## 1. Memory Management Strategy (§306)

Sulfur++ v0.1 menggunakan pendekatan hybrid:
* **Stack Memory**: Primitives (`int_64`, `float_64`, `bool`, `char`, `complex_128`), Value Structs (non-escaping), dan Call Frames.
* **Heap Memory**: Strings, Dynamic Arrays/Lists, Dictionaries, Sets, Class Instances, dan Closures.
  * **Nursery (Eden)**: Alokasi objek baru (*short-lived*).
  * **Old Generation**: Objek yang bertahan setelah $N$ siklus GC (*long-lived*).

---

## 2. Object Header Layout (§307)

Setiap objek heap memiliki header 16-byte untuk metadata runtime dan pelacakan GC:

```text
+-----------------------+-----------------------+
| TypeTag (8-bit)       | GC Flags / Color (8b) |
+-----------------------+-----------------------+
| Age / Gen Counter (8b)| Reserved Padding (8b) |
+-----------------------+-----------------------+
| Hash Code (32-bit)                            |
+-----------------------------------------------+
| Next Heap Object Pointer (*mut Obj) (64-bit)  |
+-----------------------------------------------+
| Payload / Fields Data ...                     |
+-----------------------------------------------+
```

---

## 3. Heap Allocator Architecture (§308)

* **Slab Allocator**: Objek berukuran tetap/kecil ($\le 128$ bytes) untuk alokasi $O(1)$ tanpa overhead *malloc*.
* **Large Object Space (LOS)**: String panjang atau buffer besar ($> 4$ KB) yang dialokasikan langsung via virtual memory OS.
* **Tracking LinkedList**: Seluruh objek aktif terhubung dalam linked list terpusat pada VM runtime.

---

## 4. Tri-Color Marking & Roots (§309–311)

### Status Warna:
* **White**: Belum diperiksa (kandidat reclaim / unreachable).
* **Gray**: Tercapai dari root, referensi child belum selesai dipindai.
* **Black**: Aktif dan seluruh referensi child selesai dipindai.

### GC Roots (§310):
1. **VM Value Stack**: Semua Value aktif dari stack frame 0 hingga SP saat ini.
2. **Global Variables Table**: Variabel global aktif.
3. **Constant Pool**: String interns dan modul yang dimuat.
4. **Active CallFrames**: Pointer closure dan upvalues terbuka.
5. **Host/FFI Root Handles**: Variabel native C/ABI yang sedang dipinjam.

---

## 5. Sweep & Dynamic Thresholds (§312–313)

* **Sweep Phase**: Menghapus objek berstatus **White**, mengembalikan blok ke FreeList, dan mereset status **Black** ke **White**.
* **Dynamic Trigger**:
  $$\text{NextGCThreshold} = \text{CurrentHeapBytes} \times \text{GrowthFactor (1.75)}$$
* **Explicit Trigger**: `gc.collect()` melalui standard library.

---

## 6. Write Barriers, Safe Points & Escape Analysis (§315–317)

* **Safe Points**: Stop-The-World (STW) sinkron pada loop headers dan `OP_CALL`.
* **Write Barrier**: Mencegah objek Black mereferensikan objek White secara langsung selama mutasi runtime.
* **Escape Analysis**: Nilai yang tidak keluar dari scope fungsi dialokasikan langsung di stack frame.

---

## 7. Pointers & Unsafe Mode (§318)

Operasi pointer langsung hanya diizinkan di dalam blok `unsafe`:

```sfpp
let value: int_64 = 42;

unsafe {
    let p: ptr<int_64> = &value;
    *p = 100;
}
```

---

## 8. Memory Diagnostics & Profiler (§319)

```bash
combust --profile-memory app.sfpp
```
Menampilkan total heap teralokasi, siklus GC, peak memory, dan distribusi objek aktif.

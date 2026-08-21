# Concurrency Specification (CONCURRENCY.md)

Dokumen ini mendefinisikan model konkurensi, tugas asinkron, dan komunikasi antar thread pada **Sulfur++**.

---

## 1. Thread Spawning (`spawn`)

```sfpp
spawn {
    process_data();
}
```

---

## 2. Asynchronous Functions (`async` / `await`)

```sfpp
async fn fetchData(url: str) -> Result<Data, Error> {
    let response = await http.get(url);
    return response;
}
```

---

## 3. Channels (`Channel<T>`)

Komunikasi antar thread menggunakan Message Passing via Channels:

```sfpp
let channel = Channel<int_64>();

spawn {
    channel.send(42);
}

let value = channel.receive();
print(value);
```

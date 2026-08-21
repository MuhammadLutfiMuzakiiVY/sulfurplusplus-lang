# Classes & Object-Oriented Programming (CLASSES.md)

Dokumen ini mendefinisikan OOP, deklarasi kelas, enkapsulasi, dan metode di **Sulfur++**.

---

## 1. Deklarasi Class & Constructor (`init`)

Kelas adalah tipe referensi dengan dukungan konstruktor `init` dan metode:

```sfpp
class User {
    let name: str;
    var age: int_64;

    init(name: str, age: int_64) {
        this.name = name;
        this.age = age;
    }

    fn greet() {
        print("Hello " + this.name);
    }
}
```

### Penggunaan Instans Kelas:
```sfpp
let user = User("Lutfi", 21);
user.greet();
```

---

## 2. Access Visibility Modifiers

Sulfur++ menyediakan kontrol akses enkapsulasi secara tegas:

* `pub`: Dapat diakses dari luar class/modul.
* `private`: Hanya dapat diakses dari dalam class saat ini.
* `protected`: Dapat diakses dari class saat ini dan subclass turunan.

```sfpp
pub class User {
    pub let name: str;
    private let password: str;
    protected let id: int_64;

    init(name: str, password: str, id: int_64) {
        this.name = name;
        this.password = password;
        this.id = id;
    }
}
```

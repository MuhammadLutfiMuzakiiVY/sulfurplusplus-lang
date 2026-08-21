# Modules & Namespaces Specification (MODULES.md)

Dokumen ini mendefinisikan sistem modul, impor, dan namespace di **Sulfur++**.

---

## 1. Import Statements

### 1.1 Basic Import
```sfpp
import std.io;
import std.math;
import std.http;
```

### 1.2 Selective Import (Multiple Symbols)
```sfpp
import std.math::{sqrt, sin, cos};
```

### 1.3 Module Alias (`as`)
```sfpp
import std.http as http;
```

---

## 2. Namespace Declarations

Namespace digunakan untuk mengelompokkan kode dalam hirarki terisolasi:

```sfpp
namespace app.users {
    class User {
        let name: str;
        init(name: str) {
            this.name = name;
        }
    }
}
```

### Penggunaan Namespace:
```sfpp
let user = app.users.User("Lutfi");
```

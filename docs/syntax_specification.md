# Spesifikasi Sintaksis Bahasa Sulfur++ (Sulfur++ Syntax Specification)

Dokumen ini mendefinisikan tata bahasa (*grammar*) dan sintaksis formal untuk **Sulfur++** menggunakan notasi **EBNF (Extended Backus-Naur Form)**. Spesifikasi ini menjadi acuan murni untuk konstruksi sintaksis bahasa sebelum tahap kompilasi/interpretasi.

---

## 1. Notasi EBNF

* `::=` : Definisi aturan.
* `"..."` : Terminal (literal string / karakter).
* `|` : Alternatif (pilihan).
* `[ ... ]` : Opsional (0 atau 1 kali).
* `{ ... }` : Repetisi (0 atau lebih kali).
* `( ... )` : Pengelompokan aturan.

---

## 2. Tata Bahasa Leksikal (Lexical Grammar)

### 2.1 Komentar
```ebnf
LineComment  ::= "//" { any_character_except_newline } "\n" ;
BlockComment ::= "/*" { any_character } "*/" ;
Comment      ::= LineComment | BlockComment ;
```

### 2.2 Kata Kunci (Keywords)
```ebnf
Keyword ::= "var"   | "let"     | "auto"   | "fn"      | "class"    | "struct"
          | "if"    | "else"    | "while"  | "for"     | "in"       | "match"
          | "try"   | "catch"   | "finally"| "throw"   | "return"   | "defer"
          | "import"| "export"  | "expose" | "as"      | "unsafe"   | "delete"
          | "new"   | "await"   | "null"   | "true"    | "false"    | "this"
          | "break" | "continue";
```

### 2.3 Literal
```ebnf
IntegerLiteral ::= Digit { Digit } ;
FloatLiteral   ::= Digit { Digit } "." Digit { Digit } [ Exponent ] ;
Exponent       ::= ( "e" | "E" ) [ "+" | "-" ] Digit { Digit } ;
ComplexLiteral ::= ( FloatLiteral | IntegerLiteral ) "i" ;

StringLiteral  ::= '"' { InnerStringChar } '"' ;
CharLiteral    ::= "'" InnerChar "'" ;

BooleanLiteral ::= "true" | "false" ;
NullLiteral    ::= "null" ;

Literal        ::= IntegerLiteral | FloatLiteral | ComplexLiteral 
                 | StringLiteral  | CharLiteral  | BooleanLiteral 
                 | NullLiteral ;
```

### 2.4 Identifikator
```ebnf
Identifier ::= ( Letter | "_" ) { Letter | Digit | "_" } ;
Letter     ::= "a".."z" | "A".."Z" ;
Digit      ::= "0".."9" ;
```

---

## 3. Tata Bahasa Sintaksis (Syntactic Grammar)

### 3.1 Program Utama
```ebnf
Program ::= { Statement } ;
```

### 3.2 Pernyataan (Statements)
```ebnf
Statement ::= VarDecl
            | LetDecl
            | FnDecl
            | ClassDecl
            | StructDecl
            | ImportStmt
            | ExportStmt
            | ExposeStmt
            | IfStmt
            | WhileStmt
            | ForStmt
            | MatchStmt
            | TryStmt
            | ThrowStmt
            | DeferStmt
            | UnsafeStmt
            | ReturnStmt
            | BreakStmt
            | ContinueStmt
            | Block
            | ExprStmt ;

Block ::= "{" { Statement } "}" ;
ExprStmt ::= Expression ";" ;
```

### 3.3 Deklarasi Variabel & Konstanta
```ebnf
VarDecl ::= "var" Identifier [ ":" TypeAnnotation ] [ "=" Expression ] ";" ;
LetDecl ::= "let" Identifier [ ":" TypeAnnotation ] "=" Expression ";" ;

TypeAnnotation ::= Identifier | BasicType ;
BasicType ::= "int_64" | "float_64" | "complex_128" | "str" | "char" 
            | "bool"   | "list"     | "dict"        | "set" | "fn" | "ptr" ;
```

### 3.4 Fungsi (Functions & Lambdas)
```ebnf
FnDecl ::= "fn" Identifier "(" [ ParameterList ] ")" [ ":" TypeAnnotation ] Block ;

ParameterList ::= Parameter { "," Parameter } ;
Parameter     ::= Identifier [ ":" TypeAnnotation ] ;

LambdaExpr    ::= "fn" "(" [ ParameterList ] ")" [ ":" TypeAnnotation ] Block ;
```

### 3.5 Pemrograman Berorientasi Objek (Class & Struct)
```ebnf
ClassDecl  ::= "class" Identifier "{" { ClassMember } "}" ;
ClassMember::= FieldDecl | MethodDecl | ConstructorDecl | DestructorDecl ;

FieldDecl       ::= Identifier [ ":" TypeAnnotation ] ";" ;
MethodDecl      ::= "fn" Identifier "(" [ ParameterList ] ")" Block ;
ConstructorDecl ::= "+" IntegerLiteral ">" "init" ";" FnDecl ;
DestructorDecl  ::= "~" IntegerLiteral ">" "cleanup" ";" FnDecl ;

StructDecl ::= "struct" Identifier "{" StructFieldList "}" ;
StructFieldList ::= StructField { "," StructField } [ "," ] ;
StructField     ::= Identifier ":" TypeAnnotation ;
```

### 3.6 Kontrol Alur (Control Flow)
```ebnf
IfStmt ::= "if" "(" Expression ")" Statement [ "else" Statement ] ;

WhileStmt ::= "while" "(" Expression ")" Statement ;

ForStmt ::= ForInStmt | ForCStyleStmt ;
ForInStmt ::= "for" "(" Identifier "in" Expression ")" Statement ;
ForCStyleStmt ::= "for" "(" [ VarDecl | ExprStmt ] Expression ";" [ Expression ] ")" Statement ;

MatchStmt ::= "match" "(" Expression ")" "{" { MatchCase } "}" ;
MatchCase ::= ( Expression | "_" ) "=>" ( Statement | Block ) ;
```

### 3.7 Penanganan Eksepsi (Exceptions) & Keamanan Memori
```ebnf
TryStmt   ::= "try" Block "catch" "(" Identifier ")" Block [ "finally" Block ] ;
ThrowStmt ::= "throw" Expression ";" ;

DeferStmt ::= "defer" ( Block | Statement ) ;
UnsafeStmt::= "unsafe" Block ;
```

### 3.8 Impor & Ekspor Modul
```ebnf
ImportStmt ::= "import" PathIdentifier [ "as" Identifier ] ";" ;
ExportStmt ::= "export" "this" "as" PathIdentifier ";" ;
ExposeStmt ::= "expose" StringLiteral "as" Identifier ";" ;

PathIdentifier ::= Identifier { "/" Identifier } ;
```

---

## 4. Hirarki Presedensi & Evaluasi Ekspresi

Tabel presedensi operator dari tingkat tertinggi ke terendah:

| Tingkat | Operator | Deskripsi | Asosiativitas |
| :--- | :--- | :--- | :--- |
| 1 | `()` `[]` `.` `->` | Pengelompokan, Akses Anggota, Pemanggilan, Index | Kiri ke Kanan |
| 2 | `++` `--` `!` `~` `-` `&` `*` `new` `delete` `await` | Unary, Referensi Pointer, Instansiasi | Kanan ke Kiri |
| 3 | `*` `/` `%` | Multiplikasi, Divisi, Modulo | Kiri ke Kanan |
| 4 | `+` `-` | Adisi, Subtraksi | Kiri ke Kanan |
| 5 | `<` `<=` `>` `>=` | Komparasi Relasional | Kiri ke Kanan |
| 6 | `==` `!=` | Kesetaraan (Equality) | Kiri ke Kanan |
| 7 | `&` `^` `\|` | Bitwise AND, XOR, OR | Kiri ke Kanan |
| 8 | `&&` | Logika AND | Kiri ke Kanan |
| 9 | `\|\|` | Logika OR | Kiri ke Kanan |
| 10 | `??` | Null Coalescing Operator | Kiri ke Kanan |
| 11 | `\|>` | Pipeline Operator | Kiri ke Kanan |
| 12 | `? :` | Ternary Conditional | Kanan ke Kiri |
| 13 | `=` `+=` `-=` `*=` `/=` | Pengisian Nilai (Assignment) | Kanan ke Kiri |

### EBNF Ekspresi Formal:
```ebnf
Expression     ::= AssignmentExpr ;

AssignmentExpr ::= PipelineExpr [ ( "=" | "+=" | "-=" | "*=" | "/=" ) AssignmentExpr ] ;

PipelineExpr   ::= NullCoalesceExpr { "|>" NullCoalesceExpr } ;

NullCoalesceExpr ::= LogicalOrExpr { "??" LogicalOrExpr } ;

LogicalOrExpr  ::= LogicalAndExpr { "||" LogicalAndExpr } ;

LogicalAndExpr ::= EqualityExpr { "&&" EqualityExpr } ;

EqualityExpr   ::= RelationalExpr { ( "==" | "!=" ) RelationalExpr } ;

RelationalExpr ::= AdditiveExpr { ( "<" | "<=" | ">" | ">=" ) AdditiveExpr } ;

AdditiveExpr   ::= MultiplicativeExpr { ( "+" | "-" ) MultiplicativeExpr } ;

MultiplicativeExpr ::= UnaryExpr { ( "*" | "/" | "%" ) UnaryExpr } ;

UnaryExpr      ::= ( "!" | "~" | "-" | "&" | "*" | "new" | "delete" | "await" ) UnaryExpr 
                 | PostfixExpr ;

PostfixExpr    ::= PrimaryExpr { CallSuffix | IndexSuffix | MemberSuffix } ;

CallSuffix     ::= "(" [ ArgumentList ] ")" ;
IndexSuffix    ::= "[" Expression "]" ;
MemberSuffix   ::= ( "." | "->" ) Identifier ;

ArgumentList   ::= Expression { "," Expression } ;

PrimaryExpr    ::= Literal
                 | Identifier
                 | "(" Expression ")"
                 | ListLiteral
                 | DictLiteral
                 | LambdaExpr ;

ListLiteral    ::= "[" [ Expression { "," Expression } ] "]" ;
DictLiteral    ::= "{" [ DictEntry { "," DictEntry } ] "}" ;
DictEntry      ::= StringLiteral ":" Expression ;
```

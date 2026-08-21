# Sulfur++ Operator Precedence v0.1

This table defines the exact operator precedence and associativity as implemented in `src/core/parser.cpp`. The parser uses recursive descent with one function per precedence level.

---

## Precedence Table

Higher level = binds tighter (evaluated first).

| Level | Category | Operators | Associativity | Parser Method |
|:-----:|----------|-----------|:-------------:|---------------|
| **16** | Primary | literals, identifiers, `this`, `(expr)`, `[...]`, `{...}`, `new`, `delete`, `fn` lambda | — | `parsePrimary` |
| **15** | Postfix | `.member` `?.member` `::member` `->member` `[index]` `(args)` | Left → Right | `parsePostfix` |
| **14** | Pipeline | `\|>` | Left → Right | `parsePipeline` |
| **13** | Unary (prefix) | `!` `-` `~` `&` (addr-of) `*` (deref) | Right → Left | `parseUnary` |
| **12** | Multiplicative | `*` `/` `%` `**` | Left → Right | `parseMulDiv` |
| **11** | Additive | `+` `-` | Left → Right | `parseAddSub` |
| **10** | Shift / Stream | `<<` `>>` | Left → Right | `parseShift` |
| **9** | Relational | `<` `<=` `>` `>=` | Left → Right | `parseComparison` |
| **8** | Equality | `==` `!=` | Left → Right | `parseEquality` |
| **7** | Bitwise AND | `&` | Left → Right | `parseBitwiseAnd` |
| **6** | Bitwise XOR | `^` | Left → Right | `parseBitwiseXor` |
| **5** | Bitwise OR | `\|` | Left → Right | `parseBitwiseOr` |
| **4** | Logical AND | `&&` | Left → Right | `parseAnd` |
| **3** | Logical OR | `\|\|` | Left → Right | `parseOr` |
| **2** | Null Coalescing | `??` | Left → Right | `parseNullCoal` |
| **1** | Ternary | `? :` | Right → Left | `parseTernary` |
| **0** | Assignment | `=` `+=` `-=` `*=` `/=` | Right → Left | `parseAssign` |

---

## Call Chain (Parser Descent)

```text
parseExpr
  └─ parseAssign          =  +=  -=  *=  /=       (R→L)
       └─ parseTernary    ?  :                     (R→L)
            └─ parseNullCoal  ??                   (L→R)
                 └─ parseOr       ||               (L→R)
                      └─ parseAnd     &&           (L→R)
                           └─ parseBitwiseOr  |    (L→R)
                                └─ parseBitwiseXor ^   (L→R)
                                     └─ parseBitwiseAnd &   (L→R)
                                          └─ parseEquality  ==  !=   (L→R)
                                               └─ parseComparison <  <=  >  >=  (L→R)
                                                    └─ parseShift  <<  >>  (L→R)
                                                         └─ parseAddSub  +  -  (L→R)
                                                              └─ parseMulDiv  *  /  %  **  (L→R)
                                                                   └─ parseUnary  !  -  ~  &  *  (R→L)
                                                                        └─ parsePipeline  |>  (L→R)
                                                                             └─ parsePostfix  .  ?.  []  ()  (L→R)
                                                                                  └─ parsePrimary
```

---

## Notes

### Pipeline Position
The pipeline operator `|>` sits between unary (above) and postfix (below). This means:
- `!x |> f` parses as `(!x) |> f` ✓
- `x |> f.g` parses as `x |> (f.g)` ✓
- `a + b |> f` parses as `a + (b |> f)` — pipeline binds tighter than arithmetic.

### Exponentiation
`**` is grouped with `*` `/` `%` in `parseMulDiv` (left-associative). This differs from languages like Python where `**` is right-associative with higher precedence.

### Stream Operators
`<<` and `>>` are parsed at shift level (level 10). In the expression `io.Terminal.Out << "text" << value`, this chains as left-associative binary expressions producing `BinaryExpr` nodes with op `"<<"`.

### Address-of and Dereference
`&` and `*` in unary prefix position create `AddrOfExpr` and `DerefExpr` nodes respectively, distinct from their binary meanings (bitwise AND, multiplication).

### Nullable Type vs Ternary
`?` after a type annotation is the nullable suffix. `?` after an expression starts a ternary conditional. The parser disambiguates by context (type position vs expression position).

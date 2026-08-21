# Sulfur++ Core EBNF Grammar v0.1 (CORE_GRAMMAR_V0.1.md)

**Specification Version:** 0.1-EBNF  
**Status:** Frozen EBNF Grammar Specification (113 Components)  
**Target:** Lexer, Tokenizer, Parser Generator & AST Specification

---

### 68. Program Grammar
```ebnf
program         = { declaration } ;

declaration     = import_declaration
                | export_declaration
                | variable_declaration
                | function_declaration
                | class_declaration
                | struct_declaration
                | enum_declaration
                | interface_declaration
                | trait_declaration ;
```

### 69. Identifier
```ebnf
identifier      = letter , { letter | digit | "_" } ;
letter          = "A".."Z" | "a".."z" | "_" ;
digit           = "0".."9" ;
```

### 70. Literals
```ebnf
literal         = integer | float | string | character | boolean | null | array | tuple | map | set ;
```

### 71. Integer & Float
```ebnf
integer         = digit , { digit } ;
float           = digit , { digit } , "." , digit , { digit } ;
```

### 72. String & Character
```ebnf
string          = '"' , { string_character } , '"' ;
character       = "'" , character_value , "'" ;
```

### 73. Boolean & Null
```ebnf
boolean         = "true" | "false" ;
null            = "null" ;
```

### 74. Expressions
```ebnf
expression      = assignment_expression ;
assignment_expression = conditional_expression [ assignment_operator , assignment_expression ] ;
conditional_expression = logical_or_expression [ "?" , expression , ":" , expression ] ;
```

### 75. Logical Operators
```ebnf
logical_or_expression  = logical_and_expression , { "||" , logical_and_expression } ;
logical_and_expression = equality_expression , { "&&" , equality_expression } ;
```

### 76. Comparison
```ebnf
equality_expression   = comparison_expression , { equality_operator , comparison_expression } ;
comparison_expression = additive_expression , { comparison_operator , additive_expression } ;
```

### 77. Arithmetic
```ebnf
additive_expression       = multiplicative_expression , { ("+" | "-") , multiplicative_expression } ;
multiplicative_expression = unary_expression , { ("*" | "/" | "%") , unary_expression } ;
```

### 78. Unary Expression
```ebnf
unary_expression = [ "!" | "-" | "+" | "~" ] , postfix_expression ;
```

### 79. Function Call & Postfix
```ebnf
postfix_expression = primary_expression , { postfix_operator } ;
postfix_operator   = "(" , arguments? , ")" | "[" , expression , "]" | "." , identifier ;
```

### 80. Primary Expression
```ebnf
primary_expression = identifier | literal | "(" , expression , ")" | lambda_expression ;
```

### 81. Assignment Operators
```ebnf
assignment_operator = "=" | "+=" | "-=" | "*=" | "/=" | "%=" ;
```

### 82. Variable Declaration
```ebnf
variable_declaration = variable_modifier , identifier , [ ":" , type ] , [ "=" , expression ] ;
variable_modifier    = "let" | "var" | "const" ;
```

### 83. Function Declaration
```ebnf
function_declaration = [ attribute_list ] , [ visibility ] , [ "async" ] , "fn" , identifier , "(" , parameters? , ")" , [ "->" , type ] , block ;
```

### 84. Parameters
```ebnf
parameters = parameter , { "," , parameter } ;
parameter  = [ "..." ] , identifier , [ ":" , type ] , [ "=" , expression ] ;
```

### 85. Return Statement
```ebnf
return_statement = "return" , [ expression ] ;
```

### 86. Block & Statements
```ebnf
block     = "{" , { statement } , "}" ;
statement = variable_declaration | expression_statement | return_statement | if_statement | for_statement | while_statement | break_statement | continue_statement | match_statement | try_statement | block ;
```

### 87. If / Else
```ebnf
if_statement = "if" , expression , block , { "else" , "if" , expression , block } , [ "else" , block ] ;
```

### 88. For Loop
```ebnf
for_statement = "for" , identifier , "in" , expression , block ;
```

### 89. While Loop
```ebnf
while_statement = "while" , expression , block ;
```

### 90. Break & Continue
```ebnf
break_statement    = "break" ;
continue_statement = "continue" ;
```

### 91. Array Literal
```ebnf
array = "[" , [ expression , { "," , expression } ] , "]" ;
```

### 92. Tuple Literal
```ebnf
tuple = "(" , expression , "," , expression , { "," , expression } , ")" ;
```

### 93. Map Literal
```ebnf
map       = "{" , [ map_entry , { "," , map_entry } ] , "}" ;
map_entry = expression , ":" , expression ;
```

### 94. Set Literal
```ebnf
set = "{" , expression , { "," , expression } , "}" ;
```

### 95. Type Grammar
```ebnf
type           = primitive_type | identifier | generic_type | array_type | function_type | optional_type ;
primitive_type = "Int" | "Float" | "Bool" | "String" | "Char" | "Bytes" | "Any" | "Void" ;
```

### 96. Generic Type
```ebnf
generic_type = identifier , "<" , type , { "," , type } , ">" ;
```

### 97. Array Type
```ebnf
array_type = type , "[]" ;
```

### 98. Function Type
```ebnf
function_type = "(" , [ type , { "," , type } ] , ")" , "->" , type ;
```

### 99. Optional Type
```ebnf
optional_type = type , "?" ;
```

### 100. Class Declaration
```ebnf
class_declaration = [ attribute_list ] , [ visibility ] , "class" , identifier , [ inheritance ] , class_body ;
inheritance       = "extends" , identifier , { "," , identifier } ;
class_body        = "{" , { class_member } , "}" ;
```

### 101. Class Member
```ebnf
class_member = variable_declaration | function_declaration | constructor_declaration ;
```

### 102. Constructor
```ebnf
constructor_declaration = "init" , "(" , parameters? , ")" , block ;
```

### 103. Struct Declaration
```ebnf
struct_declaration = "struct" , identifier , "{" , { field_declaration } , "}" ;
field_declaration  = identifier , ":" , type ;
```

### 104. Enum Declaration
```ebnf
enum_declaration = "enum" , identifier , "{" , { enum_variant } , "}" ;
enum_variant     = identifier , [ "(" , parameters , ")" ] ;
```

### 105. Interface Declaration
```ebnf
interface_declaration = "interface" , identifier , "{" , { function_signature } , "}" ;
```

### 106. Import Declaration
```ebnf
import_declaration = "import" , module_path , [ "as" , identifier ] ;
module_path        = identifier , { "." , identifier } ;
```

### 107. Export Declaration
```ebnf
export_declaration = "export" , declaration ;
```

### 108. Lambda Expression
```ebnf
lambda_expression = "(" , [ parameters ] , ")" , "=>" , expression ;
```

### 109. Async / Await
```ebnf
async_function   = "async" , function_declaration ;
await_expression = "await" , expression ;
```

### 110. Try / Catch / Finally
```ebnf
try_statement  = "try" , block , { catch_clause } , [ finally_clause ] ;
catch_clause   = "catch" , [ identifier ] , block ;
finally_clause = "finally" , block ;
```

### 111. Match Statement
```ebnf
match_statement = "match" , expression , "{" , { case_clause } , "}" ;
case_clause     = "case" , pattern , [ guard ] , block ;
guard           = "if" , expression ;
pattern         = literal | identifier | "_" | tuple_pattern | array_pattern ;
```

### 112. Decorators / Attributes
```ebnf
attribute_list = attribute , { attribute } ;
attribute      = "@" , identifier , [ "(" , arguments? , ")" ] ;
```

### 113. Comments
```ebnf
single_comment = "//" , { character } ;
multi_comment  = "/*" , { character } , "*/" ;
doc_comment    = "///" , { character } ;
```

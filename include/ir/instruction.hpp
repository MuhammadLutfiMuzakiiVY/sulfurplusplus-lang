#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include "semantic/type.hpp"

// ---------------------------------------------------------------------------
// Sulfur-IR: SSA Three-Address Code Intermediate Representation
// ---------------------------------------------------------------------------

enum class OpCode {
    // Memory
    Alloca,         // %ptr = alloca <type>
    Load,           // %val = load <type>, ptr %src
    Store,          // store <type> %val, ptr %dst

    // Arithmetic (integer & float)
    Add, Sub, Mul, Div, Mod, Neg, Pow,

    // Bitwise
    BitAnd, BitOr, BitXor, Shl, Shr, BitNot,

    // Comparison
    ICmpEq, ICmpNe, ICmpLt, ICmpLe, ICmpGt, ICmpGe,
    FCmpEq, FCmpNe, FCmpLt, FCmpLe, FCmpGt, FCmpGe,

    // Logical
    LogAnd, LogOr, LogNot,

    // Control flow
    Br,             // br i1 %cond, label %bb_true, label %bb_false
    Jmp,            // jmp label %bb
    Ret,            // ret <type> %val  /  ret void

    // Function call
    Call,           // %res = call @fn(%a, %b, ...)

    // SSA merge
    Phi,            // %x = phi <type> [%v1, %bb1], [%v2, %bb2]

    // Object / Heap
    AllocHeap,      // %obj = alloc_heap @ClassName
    GetField,       // %f = get_field ptr %obj, <index>
    SetField,       // set_field ptr %obj, <index>, %val

    // Type conversion
    Cast,           // %r = cast <from> %val to <to>

    // Constants (pseudo-instructions for SSA value creation)
    ConstInt,       // %r = const_int <value>
    ConstFloat,     // %r = const_float <value>
    ConstBool,      // %r = const_bool <value>
    ConstString,    // %r = const_string <value>
    ConstNull,      // %r = const_null

    // String concat
    StrConcat,      // %r = str_concat %a, %b

    // Misc
    Nop,
};

std::string opCodeToString(OpCode op);

// ---------------------------------------------------------------------------
// IRValue: A typed SSA virtual register or constant
// ---------------------------------------------------------------------------
struct IRValue {
    int id = -1;                // SSA id (%0, %1, ...)
    TypePtr type = nullptr;

    // For constants embedded inline
    int64_t constInt = 0;
    double constFloat = 0.0;
    bool constBool = false;
    std::string constStr;

    bool isConst = false;
    bool isVoid = false;

    std::string toString() const;

    static IRValue makeVoid() {
        IRValue v;
        v.isVoid = true;
        v.type = makeVoidType();
        return v;
    }
};

// ---------------------------------------------------------------------------
// Instruction
// ---------------------------------------------------------------------------
struct Instruction {
    OpCode op = OpCode::Nop;
    IRValue result;                  // Destination SSA value (may be void)
    std::vector<IRValue> operands;   // Source operands

    // For Br: trueBlock, falseBlock indices; For Jmp: targetBlock index
    int targetBlock = -1;
    int falseBlock = -1;

    // For Call: function name
    std::string funcName;

    // For GetField/SetField: field index or name
    int fieldIndex = -1;
    std::string fieldName;

    // For Phi: incoming (value, block_index) pairs
    std::vector<std::pair<IRValue, int>> phiIncoming;

    // Source location
    int srcLine = 0;

    std::string toString() const;
};

// ---------------------------------------------------------------------------
// BasicBlock
// ---------------------------------------------------------------------------
struct BasicBlock {
    std::string name;
    int index = 0;
    std::vector<Instruction> instructions;

    std::string dump(const std::string& indent = "  ") const;
};

// ---------------------------------------------------------------------------
// IRFunction
// ---------------------------------------------------------------------------
struct IRFunction {
    std::string name;
    std::vector<std::pair<std::string, TypePtr>> params;  // name, type
    TypePtr returnType;
    std::vector<BasicBlock> blocks;

    std::string dump() const;
};

// ---------------------------------------------------------------------------
// IRModule (top-level compilation unit)
// ---------------------------------------------------------------------------
struct IRModule {
    std::string name;
    std::vector<IRFunction> functions;

    // Struct/class layout definitions
    struct StructDef {
        std::string name;
        std::vector<std::pair<std::string, TypePtr>> fields;
    };
    std::vector<StructDef> structDefs;

    std::string dump() const;
};

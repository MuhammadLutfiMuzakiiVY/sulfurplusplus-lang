#pragma once

#include "ir/instruction.hpp"
#include "semantic/type.hpp"
#include <string>
#include <unordered_map>

// ---------------------------------------------------------------------------
// IRBuilder: Helper for constructing IR instructions within basic blocks
// ---------------------------------------------------------------------------
class IRBuilder {
public:
    explicit IRBuilder(IRModule& module);

    // --- Block management ---
    int createBlock(IRFunction& fn, const std::string& name);
    void setInsertPoint(IRFunction& fn, int blockIndex);
    int currentBlockIndex() const { return currentBlock_; }

    // --- SSA value creation ---
    IRValue nextSSA(TypePtr type);
    IRValue constInt(int64_t val);
    IRValue constFloat(double val);
    IRValue constBool(bool val);
    IRValue constString(const std::string& val);
    IRValue constNull();

    // --- Emit instructions ---
    IRValue emitAlloca(TypePtr type, int line = 0);
    IRValue emitLoad(TypePtr type, const IRValue& ptr, int line = 0);
    void    emitStore(const IRValue& val, const IRValue& ptr, int line = 0);

    IRValue emitBinOp(OpCode op, const IRValue& lhs, const IRValue& rhs, TypePtr resultType, int line = 0);
    IRValue emitUnaryOp(OpCode op, const IRValue& operand, TypePtr resultType, int line = 0);

    IRValue emitCall(const std::string& funcName, const std::vector<IRValue>& args, TypePtr retType, int line = 0);

    void emitBr(const IRValue& cond, int trueBlock, int falseBlock, int line = 0);
    void emitJmp(int targetBlock, int line = 0);
    void emitRet(const IRValue& val, int line = 0);
    void emitRetVoid(int line = 0);

    IRValue emitAllocHeap(const std::string& className, TypePtr type, int line = 0);
    IRValue emitGetField(const IRValue& obj, int fieldIdx, const std::string& fieldName, TypePtr fieldType, int line = 0);
    void    emitSetField(const IRValue& obj, int fieldIdx, const std::string& fieldName, const IRValue& val, int line = 0);

    IRValue emitCast(const IRValue& val, TypePtr targetType, int line = 0);
    IRValue emitStrConcat(const IRValue& lhs, const IRValue& rhs, int line = 0);

    IRValue emitPhi(TypePtr type, const std::vector<std::pair<IRValue, int>>& incoming, int line = 0);

    // --- Variable tracking (name -> SSA alloca result) ---
    void    setVar(const std::string& name, const IRValue& allocaVal);
    IRValue getVar(const std::string& name) const;
    bool    hasVar(const std::string& name) const;

    void pushScope();
    void popScope();

private:
    IRModule& module_;
    IRFunction* currentFn_ = nullptr;
    int currentBlock_ = -1;
    int nextId_ = 0;

    // Scoped variable name -> alloca IRValue
    std::vector<std::unordered_map<std::string, IRValue>> varScopes_;

    void emit(Instruction inst);
};

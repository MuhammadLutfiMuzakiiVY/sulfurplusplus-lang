#include "ir/builder.hpp"

IRBuilder::IRBuilder(IRModule& module)
    : module_(module) {
    varScopes_.push_back({});  // global scope
}

// ---------------------------------------------------------------------------
// Block management
// ---------------------------------------------------------------------------
int IRBuilder::createBlock(IRFunction& fn, const std::string& name) {
    BasicBlock bb;
    bb.name = name;
    bb.index = static_cast<int>(fn.blocks.size());
    fn.blocks.push_back(std::move(bb));
    return bb.index;
}

void IRBuilder::setInsertPoint(IRFunction& fn, int blockIndex) {
    currentFn_ = &fn;
    currentBlock_ = blockIndex;
}

// ---------------------------------------------------------------------------
// SSA value creation
// ---------------------------------------------------------------------------
IRValue IRBuilder::nextSSA(TypePtr type) {
    IRValue v;
    v.id = nextId_++;
    v.type = type ? type : makeAnyType();
    return v;
}

IRValue IRBuilder::constInt(int64_t val) {
    IRValue v;
    v.id = nextId_++;
    v.type = makeIntType();
    v.constInt = val;
    v.isConst = true;

    Instruction inst;
    inst.op = OpCode::ConstInt;
    inst.result = v;
    emit(inst);
    return v;
}

IRValue IRBuilder::constFloat(double val) {
    IRValue v;
    v.id = nextId_++;
    v.type = makeFloatType();
    v.constFloat = val;
    v.isConst = true;

    Instruction inst;
    inst.op = OpCode::ConstFloat;
    inst.result = v;
    emit(inst);
    return v;
}

IRValue IRBuilder::constBool(bool val) {
    IRValue v;
    v.id = nextId_++;
    v.type = makeBoolType();
    v.constBool = val;
    v.isConst = true;

    Instruction inst;
    inst.op = OpCode::ConstBool;
    inst.result = v;
    emit(inst);
    return v;
}

IRValue IRBuilder::constString(const std::string& val) {
    IRValue v;
    v.id = nextId_++;
    v.type = makeStringType();
    v.constStr = val;
    v.isConst = true;

    Instruction inst;
    inst.op = OpCode::ConstString;
    inst.result = v;
    emit(inst);
    return v;
}

IRValue IRBuilder::constNull() {
    IRValue v;
    v.id = nextId_++;
    v.type = makeNullType();
    v.isConst = true;

    Instruction inst;
    inst.op = OpCode::ConstNull;
    inst.result = v;
    emit(inst);
    return v;
}

// ---------------------------------------------------------------------------
// Emit helpers
// ---------------------------------------------------------------------------
void IRBuilder::emit(Instruction inst) {
    if (currentFn_ && currentBlock_ >= 0 &&
        currentBlock_ < static_cast<int>(currentFn_->blocks.size())) {
        currentFn_->blocks[currentBlock_].instructions.push_back(std::move(inst));
    }
}

IRValue IRBuilder::emitAlloca(TypePtr type, int line) {
    IRValue res = nextSSA(makePointerType(type));
    Instruction inst;
    inst.op = OpCode::Alloca;
    inst.result = res;
    inst.srcLine = line;
    emit(inst);
    return res;
}

IRValue IRBuilder::emitLoad(TypePtr type, const IRValue& ptr, int line) {
    IRValue res = nextSSA(type);
    Instruction inst;
    inst.op = OpCode::Load;
    inst.result = res;
    inst.operands = {ptr};
    inst.srcLine = line;
    emit(inst);
    return res;
}

void IRBuilder::emitStore(const IRValue& val, const IRValue& ptr, int line) {
    Instruction inst;
    inst.op = OpCode::Store;
    inst.result = IRValue::makeVoid();
    inst.operands = {val, ptr};
    inst.srcLine = line;
    emit(inst);
}

IRValue IRBuilder::emitBinOp(OpCode op, const IRValue& lhs, const IRValue& rhs, TypePtr resultType, int line) {
    IRValue res = nextSSA(resultType);
    Instruction inst;
    inst.op = op;
    inst.result = res;
    inst.operands = {lhs, rhs};
    inst.srcLine = line;
    emit(inst);
    return res;
}

IRValue IRBuilder::emitUnaryOp(OpCode op, const IRValue& operand, TypePtr resultType, int line) {
    IRValue res = nextSSA(resultType);
    Instruction inst;
    inst.op = op;
    inst.result = res;
    inst.operands = {operand};
    inst.srcLine = line;
    emit(inst);
    return res;
}

IRValue IRBuilder::emitCall(const std::string& funcName, const std::vector<IRValue>& args, TypePtr retType, int line) {
    bool isVoidRet = retType && retType->isVoid();
    IRValue res = isVoidRet ? IRValue::makeVoid() : nextSSA(retType);
    Instruction inst;
    inst.op = OpCode::Call;
    inst.result = res;
    inst.funcName = funcName;
    inst.operands = args;
    inst.srcLine = line;
    emit(inst);
    return res;
}

void IRBuilder::emitBr(const IRValue& cond, int trueBlock, int falseBlock, int line) {
    Instruction inst;
    inst.op = OpCode::Br;
    inst.result = IRValue::makeVoid();
    inst.operands = {cond};
    inst.targetBlock = trueBlock;
    inst.falseBlock = falseBlock;
    inst.srcLine = line;
    emit(inst);
}

void IRBuilder::emitJmp(int targetBlock, int line) {
    Instruction inst;
    inst.op = OpCode::Jmp;
    inst.result = IRValue::makeVoid();
    inst.targetBlock = targetBlock;
    inst.srcLine = line;
    emit(inst);
}

void IRBuilder::emitRet(const IRValue& val, int line) {
    Instruction inst;
    inst.op = OpCode::Ret;
    inst.result = IRValue::makeVoid();
    inst.operands = {val};
    inst.srcLine = line;
    emit(inst);
}

void IRBuilder::emitRetVoid(int line) {
    Instruction inst;
    inst.op = OpCode::Ret;
    inst.result = IRValue::makeVoid();
    inst.srcLine = line;
    emit(inst);
}

IRValue IRBuilder::emitAllocHeap(const std::string& className, TypePtr type, int line) {
    IRValue res = nextSSA(type);
    Instruction inst;
    inst.op = OpCode::AllocHeap;
    inst.result = res;
    inst.funcName = className;
    inst.srcLine = line;
    emit(inst);
    return res;
}

IRValue IRBuilder::emitGetField(const IRValue& obj, int fieldIdx, const std::string& fieldName, TypePtr fieldType, int line) {
    IRValue res = nextSSA(fieldType);
    Instruction inst;
    inst.op = OpCode::GetField;
    inst.result = res;
    inst.operands = {obj};
    inst.fieldIndex = fieldIdx;
    inst.fieldName = fieldName;
    inst.srcLine = line;
    emit(inst);
    return res;
}

void IRBuilder::emitSetField(const IRValue& obj, int fieldIdx, const std::string& fieldName, const IRValue& val, int line) {
    Instruction inst;
    inst.op = OpCode::SetField;
    inst.result = IRValue::makeVoid();
    inst.operands = {obj, val};
    inst.fieldIndex = fieldIdx;
    inst.fieldName = fieldName;
    inst.srcLine = line;
    emit(inst);
}

IRValue IRBuilder::emitCast(const IRValue& val, TypePtr targetType, int line) {
    IRValue res = nextSSA(targetType);
    Instruction inst;
    inst.op = OpCode::Cast;
    inst.result = res;
    inst.operands = {val};
    inst.srcLine = line;
    emit(inst);
    return res;
}

IRValue IRBuilder::emitStrConcat(const IRValue& lhs, const IRValue& rhs, int line) {
    IRValue res = nextSSA(makeStringType());
    Instruction inst;
    inst.op = OpCode::StrConcat;
    inst.result = res;
    inst.operands = {lhs, rhs};
    inst.srcLine = line;
    emit(inst);
    return res;
}

IRValue IRBuilder::emitPhi(TypePtr type, const std::vector<std::pair<IRValue, int>>& incoming, int line) {
    IRValue res = nextSSA(type);
    Instruction inst;
    inst.op = OpCode::Phi;
    inst.result = res;
    inst.phiIncoming = incoming;
    inst.srcLine = line;
    emit(inst);
    return res;
}

// ---------------------------------------------------------------------------
// Variable tracking
// ---------------------------------------------------------------------------
void IRBuilder::setVar(const std::string& name, const IRValue& allocaVal) {
    if (!varScopes_.empty()) {
        varScopes_.back()[name] = allocaVal;
    }
}

IRValue IRBuilder::getVar(const std::string& name) const {
    for (auto it = varScopes_.rbegin(); it != varScopes_.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return found->second;
    }
    // Return a placeholder for unresolved (dynamically bound) variables
    IRValue v;
    v.id = -1;
    v.type = makeAnyType();
    return v;
}

bool IRBuilder::hasVar(const std::string& name) const {
    for (auto it = varScopes_.rbegin(); it != varScopes_.rend(); ++it) {
        if (it->count(name)) return true;
    }
    return false;
}

void IRBuilder::pushScope() {
    varScopes_.push_back({});
}

void IRBuilder::popScope() {
    if (varScopes_.size() > 1) {
        varScopes_.pop_back();
    }
}

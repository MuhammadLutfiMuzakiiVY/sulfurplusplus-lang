#include "ir/instruction.hpp"
#include <sstream>

std::string opCodeToString(OpCode op) {
    switch (op) {
        case OpCode::Alloca:     return "alloca";
        case OpCode::Load:       return "load";
        case OpCode::Store:      return "store";
        case OpCode::Add:        return "add";
        case OpCode::Sub:        return "sub";
        case OpCode::Mul:        return "mul";
        case OpCode::Div:        return "div";
        case OpCode::Mod:        return "mod";
        case OpCode::Neg:        return "neg";
        case OpCode::Pow:        return "pow";
        case OpCode::BitAnd:     return "bit_and";
        case OpCode::BitOr:      return "bit_or";
        case OpCode::BitXor:     return "bit_xor";
        case OpCode::Shl:        return "shl";
        case OpCode::Shr:        return "shr";
        case OpCode::BitNot:     return "bit_not";
        case OpCode::ICmpEq:     return "icmp_eq";
        case OpCode::ICmpNe:     return "icmp_ne";
        case OpCode::ICmpLt:     return "icmp_lt";
        case OpCode::ICmpLe:     return "icmp_le";
        case OpCode::ICmpGt:     return "icmp_gt";
        case OpCode::ICmpGe:     return "icmp_ge";
        case OpCode::FCmpEq:     return "fcmp_eq";
        case OpCode::FCmpNe:     return "fcmp_ne";
        case OpCode::FCmpLt:     return "fcmp_lt";
        case OpCode::FCmpLe:     return "fcmp_le";
        case OpCode::FCmpGt:     return "fcmp_gt";
        case OpCode::FCmpGe:     return "fcmp_ge";
        case OpCode::LogAnd:     return "log_and";
        case OpCode::LogOr:      return "log_or";
        case OpCode::LogNot:     return "log_not";
        case OpCode::Br:         return "br";
        case OpCode::Jmp:        return "jmp";
        case OpCode::Ret:        return "ret";
        case OpCode::Call:       return "call";
        case OpCode::Phi:        return "phi";
        case OpCode::AllocHeap:  return "alloc_heap";
        case OpCode::GetField:   return "get_field";
        case OpCode::SetField:   return "set_field";
        case OpCode::Cast:       return "cast";
        case OpCode::ConstInt:   return "const_int";
        case OpCode::ConstFloat: return "const_float";
        case OpCode::ConstBool:  return "const_bool";
        case OpCode::ConstString:return "const_string";
        case OpCode::ConstNull:  return "const_null";
        case OpCode::StrConcat:  return "str_concat";
        case OpCode::Nop:        return "nop";
    }
    return "unknown";
}

// ---------------------------------------------------------------------------
// IRValue::toString
// ---------------------------------------------------------------------------
std::string IRValue::toString() const {
    if (isVoid) return "void";
    if (isConst) {
        if (type && type->isString()) return "\"" + constStr + "\"";
        if (type && type->isBool()) return constBool ? "true" : "false";
        if (type && type->isFloat()) {
            std::ostringstream oss;
            oss << constFloat;
            return oss.str();
        }
        if (type && type->isNull()) return "null";
        // Default: integer constant
        return std::to_string(constInt);
    }
    return "%" + std::to_string(id);
}

// ---------------------------------------------------------------------------
// Instruction::toString
// ---------------------------------------------------------------------------
std::string Instruction::toString() const {
    std::ostringstream ss;

    switch (op) {
        case OpCode::Alloca:
            ss << result.toString() << " = alloca " << (result.type ? result.type->toString() : "any");
            break;

        case OpCode::Load:
            ss << result.toString() << " = load "
               << (result.type ? result.type->toString() : "any")
               << ", ptr " << (operands.empty() ? "?" : operands[0].toString());
            break;

        case OpCode::Store:
            ss << "store "
               << (operands.size() > 0 ? (operands[0].type ? operands[0].type->toString() : "any") : "?")
               << " " << (operands.size() > 0 ? operands[0].toString() : "?")
               << ", ptr " << (operands.size() > 1 ? operands[1].toString() : "?");
            break;

        case OpCode::Br:
            ss << "br i1 " << (operands.empty() ? "?" : operands[0].toString())
               << ", label %bb" << targetBlock
               << ", label %bb" << falseBlock;
            break;

        case OpCode::Jmp:
            ss << "jmp label %bb" << targetBlock;
            break;

        case OpCode::Ret:
            if (result.isVoid && operands.empty()) {
                ss << "ret void";
            } else {
                ss << "ret "
                   << (operands.empty() ? "void" :
                       (operands[0].type ? operands[0].type->toString() + " " : "") + operands[0].toString());
            }
            break;

        case OpCode::Call:
            if (!result.isVoid) {
                ss << result.toString() << " = ";
            }
            ss << "call @" << funcName << "(";
            for (size_t i = 0; i < operands.size(); i++) {
                if (i > 0) ss << ", ";
                if (operands[i].type) ss << operands[i].type->toString() << " ";
                ss << operands[i].toString();
            }
            ss << ")";
            break;

        case OpCode::Phi:
            ss << result.toString() << " = phi "
               << (result.type ? result.type->toString() : "any");
            for (const auto& [val, blk] : phiIncoming) {
                ss << " [" << val.toString() << ", %bb" << blk << "]";
            }
            break;

        case OpCode::AllocHeap:
            ss << result.toString() << " = alloc_heap @" << funcName;
            break;

        case OpCode::GetField:
            ss << result.toString() << " = get_field ptr "
               << (operands.empty() ? "?" : operands[0].toString())
               << ", " << fieldIndex;
            if (!fieldName.empty()) ss << " ; " << fieldName;
            break;

        case OpCode::SetField:
            ss << "set_field ptr "
               << (operands.size() > 0 ? operands[0].toString() : "?")
               << ", " << fieldIndex << ", "
               << (operands.size() > 1 ? operands[1].toString() : "?");
            if (!fieldName.empty()) ss << " ; " << fieldName;
            break;

        case OpCode::ConstInt:
            ss << result.toString() << " = const_int " << result.constInt;
            break;
        case OpCode::ConstFloat:
            ss << result.toString() << " = const_float " << result.constFloat;
            break;
        case OpCode::ConstBool:
            ss << result.toString() << " = const_bool " << (result.constBool ? "true" : "false");
            break;
        case OpCode::ConstString:
            ss << result.toString() << " = const_string \"" << result.constStr << "\"";
            break;
        case OpCode::ConstNull:
            ss << result.toString() << " = const_null";
            break;

        default: {
            // Binary / unary ops
            if (!result.isVoid) ss << result.toString() << " = ";
            ss << opCodeToString(op);
            if (result.type) ss << " " << result.type->toString();
            for (size_t i = 0; i < operands.size(); i++) {
                ss << " " << operands[i].toString();
                if (i + 1 < operands.size()) ss << ",";
            }
            break;
        }
    }

    return ss.str();
}

// ---------------------------------------------------------------------------
// BasicBlock::dump
// ---------------------------------------------------------------------------
std::string BasicBlock::dump(const std::string& indent) const {
    std::ostringstream ss;
    ss << name << ":\n";
    for (const auto& inst : instructions) {
        ss << indent << inst.toString() << "\n";
    }
    return ss.str();
}

// ---------------------------------------------------------------------------
// IRFunction::dump
// ---------------------------------------------------------------------------
std::string IRFunction::dump() const {
    std::ostringstream ss;
    ss << "define ";
    ss << (returnType ? returnType->toString() : "void");
    ss << " @" << name << "(";
    for (size_t i = 0; i < params.size(); i++) {
        if (i > 0) ss << ", ";
        ss << (params[i].second ? params[i].second->toString() : "any")
           << " %" << params[i].first;
    }
    ss << ") {\n";
    for (const auto& bb : blocks) {
        ss << bb.dump();
    }
    ss << "}\n";
    return ss.str();
}

// ---------------------------------------------------------------------------
// IRModule::dump
// ---------------------------------------------------------------------------
std::string IRModule::dump() const {
    std::ostringstream ss;
    ss << "; Sulfur-IR Module: " << name << "\n\n";

    for (const auto& sd : structDefs) {
        ss << "type @" << sd.name << " = { ";
        for (size_t i = 0; i < sd.fields.size(); i++) {
            if (i > 0) ss << ", ";
            ss << (sd.fields[i].second ? sd.fields[i].second->toString() : "any")
               << " " << sd.fields[i].first;
        }
        ss << " }\n";
    }
    if (!structDefs.empty()) ss << "\n";

    for (const auto& fn : functions) {
        ss << fn.dump() << "\n";
    }
    return ss.str();
}

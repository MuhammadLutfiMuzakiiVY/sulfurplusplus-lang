#include "ir/passes.hpp"
#include <unordered_set>

namespace ir_passes {

// ---------------------------------------------------------------------------
// Constant Folding
// Replaces binary ops on two integer/float constants with a single constant.
// ---------------------------------------------------------------------------
void constantFolding(IRModule& module) {
    for (auto& fn : module.functions) {
        for (auto& bb : fn.blocks) {
            // Map SSA id -> constant value (if known)
            std::unordered_map<int, IRValue> constMap;

            for (auto& inst : bb.instructions) {
                // Register constants
                if (inst.op == OpCode::ConstInt || inst.op == OpCode::ConstFloat ||
                    inst.op == OpCode::ConstBool || inst.op == OpCode::ConstString ||
                    inst.op == OpCode::ConstNull) {
                    constMap[inst.result.id] = inst.result;
                    continue;
                }

                // Fold binary integer ops
                if ((inst.op == OpCode::Add || inst.op == OpCode::Sub ||
                     inst.op == OpCode::Mul || inst.op == OpCode::Div ||
                     inst.op == OpCode::Mod) &&
                    inst.operands.size() == 2) {

                    auto* lhs = &inst.operands[0];
                    auto* rhs = &inst.operands[1];

                    // Resolve operand to constant if possible
                    if (!lhs->isConst && lhs->id >= 0 && constMap.count(lhs->id))
                        lhs = &constMap[lhs->id];
                    if (!rhs->isConst && rhs->id >= 0 && constMap.count(rhs->id))
                        rhs = &constMap[rhs->id];

                    if (lhs->isConst && rhs->isConst &&
                        lhs->type && rhs->type &&
                        lhs->type->isInt() && rhs->type->isInt()) {

                        int64_t result = 0;
                        switch (inst.op) {
                            case OpCode::Add: result = lhs->constInt + rhs->constInt; break;
                            case OpCode::Sub: result = lhs->constInt - rhs->constInt; break;
                            case OpCode::Mul: result = lhs->constInt * rhs->constInt; break;
                            case OpCode::Div:
                                if (rhs->constInt != 0) result = lhs->constInt / rhs->constInt;
                                else continue;  // Don't fold division by zero
                                break;
                            case OpCode::Mod:
                                if (rhs->constInt != 0) result = lhs->constInt % rhs->constInt;
                                else continue;
                                break;
                            default: continue;
                        }

                        // Replace with ConstInt
                        inst.op = OpCode::ConstInt;
                        inst.result.constInt = result;
                        inst.result.isConst = true;
                        inst.result.type = makeIntType();
                        inst.operands.clear();
                        constMap[inst.result.id] = inst.result;
                    }

                    else if (lhs->isConst && rhs->isConst &&
                        lhs->type && rhs->type &&
                        (lhs->type->isFloat() || rhs->type->isFloat())) {

                        double lv = lhs->type->isFloat() ? lhs->constFloat : static_cast<double>(lhs->constInt);
                        double rv = rhs->type->isFloat() ? rhs->constFloat : static_cast<double>(rhs->constInt);
                        double result = 0;
                        switch (inst.op) {
                            case OpCode::Add: result = lv + rv; break;
                            case OpCode::Sub: result = lv - rv; break;
                            case OpCode::Mul: result = lv * rv; break;
                            case OpCode::Div:
                                if (rv != 0.0) result = lv / rv;
                                else continue;
                                break;
                            default: continue;
                        }

                        inst.op = OpCode::ConstFloat;
                        inst.result.constFloat = result;
                        inst.result.isConst = true;
                        inst.result.type = makeFloatType();
                        inst.operands.clear();
                        constMap[inst.result.id] = inst.result;
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Dead Block Elimination
// Removes basic blocks that are never the target of any branch/jump.
// ---------------------------------------------------------------------------
void deadBlockElimination(IRModule& module) {
    for (auto& fn : module.functions) {
        if (fn.blocks.size() <= 1) continue;

        // Find reachable blocks
        std::unordered_set<int> reachable;
        reachable.insert(0);  // Entry block always reachable

        bool changed = true;
        while (changed) {
            changed = false;
            for (int idx : reachable) {
                if (idx < 0 || idx >= static_cast<int>(fn.blocks.size())) continue;
                for (const auto& inst : fn.blocks[idx].instructions) {
                    if (inst.op == OpCode::Jmp && inst.targetBlock >= 0) {
                        if (!reachable.count(inst.targetBlock)) {
                            reachable.insert(inst.targetBlock);
                            changed = true;
                        }
                    }
                    if (inst.op == OpCode::Br) {
                        if (inst.targetBlock >= 0 && !reachable.count(inst.targetBlock)) {
                            reachable.insert(inst.targetBlock);
                            changed = true;
                        }
                        if (inst.falseBlock >= 0 && !reachable.count(inst.falseBlock)) {
                            reachable.insert(inst.falseBlock);
                            changed = true;
                        }
                    }
                }
            }
        }

        // Remove unreachable blocks (mark as empty; preserving indices to avoid reindexing)
        for (int i = 1; i < static_cast<int>(fn.blocks.size()); i++) {
            if (!reachable.count(i)) {
                fn.blocks[i].instructions.clear();
                fn.blocks[i].name = "; dead_" + fn.blocks[i].name;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Run all passes
// ---------------------------------------------------------------------------
void runAllPasses(IRModule& module) {
    constantFolding(module);
    deadBlockElimination(module);
}

} // namespace ir_passes

#pragma once

#include "ir/instruction.hpp"

// ---------------------------------------------------------------------------
// IR Optimization Passes
// ---------------------------------------------------------------------------
namespace ir_passes {

// Constant folding: replaces `add const_a, const_b` with `const_(a+b)` etc.
void constantFolding(IRModule& module);

// Dead block elimination: removes unreachable basic blocks
void deadBlockElimination(IRModule& module);

// Run all passes in sequence
void runAllPasses(IRModule& module);

} // namespace ir_passes

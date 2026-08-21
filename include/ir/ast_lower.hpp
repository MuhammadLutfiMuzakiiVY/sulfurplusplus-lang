#pragma once

#include "ir/instruction.hpp"
#include "ir/builder.hpp"
#include "ast.hpp"
#include "semantic/type.hpp"
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// ASTLowerer: Converts Typed AST to Sulfur-IR
// ---------------------------------------------------------------------------
class ASTLowerer {
public:
    ASTLowerer();

    // Lower an entire program (list of top-level statements) into an IRModule
    IRModule lower(const std::vector<StmtPtr>& stmts, const std::string& moduleName = "main");

private:
    std::unique_ptr<IRModule> module_;
    std::unique_ptr<IRBuilder> builder_;

    // Current function being lowered
    IRFunction* currentFn_ = nullptr;
    int loopHeaderBlock_ = -1;  // For continue
    int loopExitBlock_ = -1;    // For break

    // --- Statement lowering ---
    void lowerStmt(const Stmt& stmt);
    void lowerBlock(const BlockStmt& stmt);
    void lowerVarDecl(const VarDeclStmt& stmt);
    void lowerFnDecl(const FnDeclStmt& stmt);
    void lowerClassDecl(const ClassDeclStmt& stmt);
    void lowerStructDecl(const StructDeclStmt& stmt);
    void lowerIf(const IfStmt& stmt);
    void lowerWhile(const WhileStmt& stmt);
    void lowerFor(const ForStmt& stmt);
    void lowerForCStyle(const ForCStyleStmt& stmt);
    void lowerReturn(const ReturnStmt& stmt);
    void lowerBreak(const BreakStmt& stmt);
    void lowerContinue(const ContinueStmt& stmt);
    void lowerExprStmt(const ExprStmt& stmt);
    void lowerMatch(const MatchStmt& stmt);
    void lowerTryCatch(const TryCatchStmt& stmt);
    void lowerDefer(const DeferStmt& stmt);
    void lowerThrow(const ThrowStmt& stmt);

    // --- Expression lowering ---
    IRValue lowerExpr(const Expr& expr);
    IRValue lowerBinary(const BinaryExpr& expr);
    IRValue lowerUnary(const UnaryExpr& expr);
    IRValue lowerCall(const CallExpr& expr);
    IRValue lowerAssign(const AssignExpr& expr);
    IRValue lowerMember(const MemberExpr& expr);
    IRValue lowerIndex(const IndexExpr& expr);
    IRValue lowerTernary(const TernaryExpr& expr);
    IRValue lowerLambda(const LambdaExpr& expr);

    // --- Helpers ---
    TypePtr inferExprType(const Expr& expr);
    OpCode binOpToOpCode(const std::string& op, bool isFloat);
    OpCode cmpOpToOpCode(const std::string& op, bool isFloat);

    // Ensure current block has a terminator; emit ret void if missing
    void ensureTerminator();
};

#include "ir/ast_lower.hpp"
#include "error.hpp"

ASTLowerer::ASTLowerer() {}

// ---------------------------------------------------------------------------
// Public entry point
// ---------------------------------------------------------------------------
IRModule ASTLowerer::lower(const std::vector<StmtPtr>& stmts, const std::string& moduleName) {
    module_ = std::make_unique<IRModule>();
    module_->name = moduleName;
    builder_ = std::make_unique<IRBuilder>(*module_);

    // Pass 1: Collect struct/class definitions
    for (const auto& s : stmts) {
        if (!s) continue;
        if (auto* sd = std::get_if<StructDeclStmt>(&s->data)) {
            IRModule::StructDef def;
            def.name = sd->name;
            for (const auto& f : sd->fields) {
                def.fields.push_back({f.first, parseTypeName(f.second)});
            }
            module_->structDefs.push_back(std::move(def));
        } else if (auto* cd = std::get_if<ClassDeclStmt>(&s->data)) {
            IRModule::StructDef def;
            def.name = cd->name;
            for (const auto& m : cd->members) {
                if (!m) continue;
                if (auto* vd = std::get_if<VarDeclStmt>(&m->data)) {
                    def.fields.push_back({vd->name, parseTypeName(vd->type)});
                }
            }
            module_->structDefs.push_back(std::move(def));
        }
    }

    // Pass 2: Forward-declare all functions
    for (const auto& s : stmts) {
        if (!s) continue;
        if (auto* fn = std::get_if<FnDeclStmt>(&s->data)) {
            // Register in builder's variable scope as a function pointer
            // (actual lowering happens in pass 3)
            (void)fn;
        }
    }

    // Pass 3: Create a __main__ function for top-level code, and lower all functions
    IRFunction mainFn;
    mainFn.name = "__main__";
    mainFn.returnType = makeVoidType();
    module_->functions.push_back(std::move(mainFn));
    auto& mainRef = module_->functions.back();
    int entryBlock = builder_->createBlock(mainRef, "bb_entry");
    builder_->setInsertPoint(mainRef, entryBlock);
    currentFn_ = &mainRef;

    for (const auto& s : stmts) {
        if (!s) continue;
        // Functions are lowered as separate IRFunctions
        if (std::get_if<FnDeclStmt>(&s->data)) {
            lowerStmt(*s);
        } else if (std::get_if<ClassDeclStmt>(&s->data) || std::get_if<StructDeclStmt>(&s->data)) {
            lowerStmt(*s);
        } else {
            // Top-level code goes into __main__
            builder_->setInsertPoint(mainRef, builder_->currentBlockIndex());
            currentFn_ = &mainRef;
            lowerStmt(*s);
        }
    }

    // Ensure __main__ has a terminator
    currentFn_ = &mainRef;
    ensureTerminator();

    IRModule result = std::move(*module_);
    module_.reset();
    builder_.reset();
    return result;
}

// ---------------------------------------------------------------------------
// Statement lowering
// ---------------------------------------------------------------------------
void ASTLowerer::lowerStmt(const Stmt& stmt) {
    std::visit([&](const auto& node) {
        using T = std::decay_t<decltype(node)>;
        if constexpr (std::is_same_v<T, BlockStmt>)           lowerBlock(node);
        else if constexpr (std::is_same_v<T, VarDeclStmt>)    lowerVarDecl(node);
        else if constexpr (std::is_same_v<T, FnDeclStmt>)     lowerFnDecl(node);
        else if constexpr (std::is_same_v<T, ClassDeclStmt>)  lowerClassDecl(node);
        else if constexpr (std::is_same_v<T, StructDeclStmt>) lowerStructDecl(node);
        else if constexpr (std::is_same_v<T, IfStmt>)         lowerIf(node);
        else if constexpr (std::is_same_v<T, WhileStmt>)      lowerWhile(node);
        else if constexpr (std::is_same_v<T, ForStmt>)        lowerFor(node);
        else if constexpr (std::is_same_v<T, ForCStyleStmt>)  lowerForCStyle(node);
        else if constexpr (std::is_same_v<T, ReturnStmt>)     lowerReturn(node);
        else if constexpr (std::is_same_v<T, BreakStmt>)      lowerBreak(node);
        else if constexpr (std::is_same_v<T, ContinueStmt>)   lowerContinue(node);
        else if constexpr (std::is_same_v<T, ExprStmt>)       lowerExprStmt(node);
        else if constexpr (std::is_same_v<T, MatchStmt>)      lowerMatch(node);
        else if constexpr (std::is_same_v<T, TryCatchStmt>)   lowerTryCatch(node);
        else if constexpr (std::is_same_v<T, DeferStmt>)      lowerDefer(node);
        else if constexpr (std::is_same_v<T, ThrowStmt>)      lowerThrow(node);
        else if constexpr (std::is_same_v<T, StreamOutStmt>) {
            if (node.value) {
                IRValue val = lowerExpr(*node.value);
                builder_->emitCall("__stream_out__", {val}, makeVoidType(), node.line);
            }
        }
        // Import/Export/Expose/Overwrite/Unsafe/InterfaceDecl — no-op at IR level
    }, stmt.data);
}

void ASTLowerer::lowerBlock(const BlockStmt& stmt) {
    builder_->pushScope();
    for (const auto& s : stmt.stmts) {
        if (s) lowerStmt(*s);
    }
    builder_->popScope();
}

void ASTLowerer::lowerVarDecl(const VarDeclStmt& stmt) {
    TypePtr varType = parseTypeName(stmt.type);
    if (varType->isAny() && stmt.initializer) {
        varType = inferExprType(*stmt.initializer);
    }

    IRValue alloca = builder_->emitAlloca(varType, stmt.line);
    builder_->setVar(stmt.name, alloca);

    if (stmt.initializer) {
        IRValue initVal = lowerExpr(*stmt.initializer);
        builder_->emitStore(initVal, alloca, stmt.line);
    }
}

void ASTLowerer::lowerFnDecl(const FnDeclStmt& stmt) {
    // Save current context
    IRFunction* savedFn = currentFn_;
    int savedBlock = builder_->currentBlockIndex();
    int savedLoopHeader = loopHeaderBlock_;
    int savedLoopExit = loopExitBlock_;
    loopHeaderBlock_ = -1;
    loopExitBlock_ = -1;

    // Create new IRFunction
    IRFunction fn;
    fn.name = stmt.name.empty() ? "__anon_fn__" : stmt.name;
    fn.returnType = parseTypeName(stmt.retType);
    for (const auto& p : stmt.params) {
        fn.params.push_back({p.first, parseTypeName(p.second)});
    }
    module_->functions.push_back(std::move(fn));
    auto& fnRef = module_->functions.back();

    int entryBlock = builder_->createBlock(fnRef, "bb_entry");
    builder_->setInsertPoint(fnRef, entryBlock);
    currentFn_ = &fnRef;
    builder_->pushScope();

    // Alloca + store for each parameter
    for (size_t i = 0; i < stmt.params.size(); i++) {
        TypePtr pType = parseTypeName(stmt.params[i].second);
        IRValue paramAlloca = builder_->emitAlloca(pType, stmt.line);
        // Emit a synthetic param load (in real codegen, params come from calling convention)
        IRValue paramVal = builder_->nextSSA(pType);
        builder_->emitStore(paramVal, paramAlloca, stmt.line);
        builder_->setVar(stmt.params[i].first, paramAlloca);
    }

    // Lower body
    if (stmt.body) {
        if (auto* b = std::get_if<BlockStmt>(&stmt.body->data)) {
            for (const auto& s : b->stmts) {
                if (s) lowerStmt(*s);
            }
        } else {
            lowerStmt(*stmt.body);
        }
    }

    ensureTerminator();
    builder_->popScope();

    // Restore context
    currentFn_ = savedFn;
    if (savedFn) {
        builder_->setInsertPoint(*savedFn, savedBlock);
    }
    loopHeaderBlock_ = savedLoopHeader;
    loopExitBlock_ = savedLoopExit;
}

void ASTLowerer::lowerClassDecl(const ClassDeclStmt& stmt) {
    // Lower methods as standalone functions prefixed with class name
    for (const auto& m : stmt.members) {
        if (!m) continue;
        if (auto* fn = std::get_if<FnDeclStmt>(&m->data)) {
            // Save context
            IRFunction* savedFn = currentFn_;
            int savedBlock = builder_->currentBlockIndex();
            int savedLoopHeader = loopHeaderBlock_;
            int savedLoopExit = loopExitBlock_;
            loopHeaderBlock_ = -1;
            loopExitBlock_ = -1;

            std::string methodName = stmt.name + "." + fn->name;
            IRFunction irFn;
            irFn.name = methodName;
            irFn.returnType = parseTypeName(fn->retType);
            for (const auto& p : fn->params) {
                irFn.params.push_back({p.first, parseTypeName(p.second)});
            }
            module_->functions.push_back(std::move(irFn));
            auto& fnRef = module_->functions.back();

            int entryBlock = builder_->createBlock(fnRef, "bb_entry");
            builder_->setInsertPoint(fnRef, entryBlock);
            currentFn_ = &fnRef;
            builder_->pushScope();

            for (size_t i = 0; i < fn->params.size(); i++) {
                TypePtr pType = parseTypeName(fn->params[i].second);
                IRValue paramAlloca = builder_->emitAlloca(pType, fn->line);
                builder_->setVar(fn->params[i].first, paramAlloca);
            }

            if (fn->body) {
                if (auto* b = std::get_if<BlockStmt>(&fn->body->data)) {
                    for (const auto& s : b->stmts) {
                        if (s) lowerStmt(*s);
                    }
                } else {
                    lowerStmt(*fn->body);
                }
            }

            ensureTerminator();
            builder_->popScope();

            currentFn_ = savedFn;
            if (savedFn) builder_->setInsertPoint(*savedFn, savedBlock);
            loopHeaderBlock_ = savedLoopHeader;
            loopExitBlock_ = savedLoopExit;
        }
    }
}

void ASTLowerer::lowerStructDecl(const StructDeclStmt& /*stmt*/) {
    // Already handled in pass 1 (struct definitions collected into module_->structDefs)
}

void ASTLowerer::lowerIf(const IfStmt& stmt) {
    IRValue cond = stmt.cond ? lowerExpr(*stmt.cond) : builder_->constBool(true);

    int thenBlock = builder_->createBlock(*currentFn_, "bb_then");
    int elseBlock = stmt.elseBranch ? builder_->createBlock(*currentFn_, "bb_else") : -1;
    int mergeBlock = builder_->createBlock(*currentFn_, "bb_merge");

    builder_->emitBr(cond, thenBlock, elseBlock >= 0 ? elseBlock : mergeBlock, stmt.line);

    // Then branch
    builder_->setInsertPoint(*currentFn_, thenBlock);
    if (stmt.thenBranch) lowerStmt(*stmt.thenBranch);
    ensureTerminator();
    // Only emit jmp if the block doesn't already have a terminator
    auto& thenBB = currentFn_->blocks[thenBlock];
    if (thenBB.instructions.empty() ||
        (thenBB.instructions.back().op != OpCode::Jmp &&
         thenBB.instructions.back().op != OpCode::Br &&
         thenBB.instructions.back().op != OpCode::Ret)) {
        builder_->setInsertPoint(*currentFn_, thenBlock);
        builder_->emitJmp(mergeBlock, stmt.line);
    }

    // Else branch
    if (elseBlock >= 0) {
        builder_->setInsertPoint(*currentFn_, elseBlock);
        if (stmt.elseBranch) lowerStmt(*stmt.elseBranch);
        auto& elseBB = currentFn_->blocks[elseBlock];
        if (elseBB.instructions.empty() ||
            (elseBB.instructions.back().op != OpCode::Jmp &&
             elseBB.instructions.back().op != OpCode::Br &&
             elseBB.instructions.back().op != OpCode::Ret)) {
            builder_->setInsertPoint(*currentFn_, elseBlock);
            builder_->emitJmp(mergeBlock, stmt.line);
        }
    }

    builder_->setInsertPoint(*currentFn_, mergeBlock);
}

void ASTLowerer::lowerWhile(const WhileStmt& stmt) {
    int headerBlock = builder_->createBlock(*currentFn_, "bb_loop_header");
    int bodyBlock = builder_->createBlock(*currentFn_, "bb_loop_body");
    int exitBlock = builder_->createBlock(*currentFn_, "bb_loop_exit");

    int savedHeader = loopHeaderBlock_;
    int savedExit = loopExitBlock_;
    loopHeaderBlock_ = headerBlock;
    loopExitBlock_ = exitBlock;

    builder_->emitJmp(headerBlock, stmt.line);

    // Header: evaluate condition
    builder_->setInsertPoint(*currentFn_, headerBlock);
    IRValue cond = stmt.cond ? lowerExpr(*stmt.cond) : builder_->constBool(true);
    builder_->emitBr(cond, bodyBlock, exitBlock, stmt.line);

    // Body
    builder_->setInsertPoint(*currentFn_, bodyBlock);
    if (stmt.body) lowerStmt(*stmt.body);
    // Jump back to header if no terminator
    auto& bodyBB = currentFn_->blocks[bodyBlock];
    if (bodyBB.instructions.empty() ||
        (bodyBB.instructions.back().op != OpCode::Jmp &&
         bodyBB.instructions.back().op != OpCode::Br &&
         bodyBB.instructions.back().op != OpCode::Ret)) {
        builder_->setInsertPoint(*currentFn_, bodyBlock);
        builder_->emitJmp(headerBlock, stmt.line);
    }

    builder_->setInsertPoint(*currentFn_, exitBlock);
    loopHeaderBlock_ = savedHeader;
    loopExitBlock_ = savedExit;
}

void ASTLowerer::lowerFor(const ForStmt& stmt) {
    // For-in loop: desugar to while-loop with iterator call
    int headerBlock = builder_->createBlock(*currentFn_, "bb_for_header");
    int bodyBlock = builder_->createBlock(*currentFn_, "bb_for_body");
    int exitBlock = builder_->createBlock(*currentFn_, "bb_for_exit");

    int savedHeader = loopHeaderBlock_;
    int savedExit = loopExitBlock_;
    loopHeaderBlock_ = headerBlock;
    loopExitBlock_ = exitBlock;

    // Alloca for iterator variable
    IRValue iterAlloca = builder_->emitAlloca(makeAnyType(), stmt.line);
    builder_->setVar(stmt.var, iterAlloca);

    IRValue iterableVal = stmt.iterable ? lowerExpr(*stmt.iterable) : builder_->constNull();

    // ponytail: for-in lowered as call to __iter_next__; real iterator protocol deferred to VM backend
    IRValue iterObj = builder_->emitCall("__iter_init__", {iterableVal}, makeAnyType(), stmt.line);
    builder_->emitJmp(headerBlock, stmt.line);

    builder_->setInsertPoint(*currentFn_, headerBlock);
    IRValue hasNext = builder_->emitCall("__iter_has_next__", {iterObj}, makeBoolType(), stmt.line);
    builder_->emitBr(hasNext, bodyBlock, exitBlock, stmt.line);

    builder_->setInsertPoint(*currentFn_, bodyBlock);
    builder_->pushScope();
    IRValue nextVal = builder_->emitCall("__iter_next__", {iterObj}, makeAnyType(), stmt.line);
    builder_->emitStore(nextVal, iterAlloca, stmt.line);

    if (stmt.body) lowerStmt(*stmt.body);

    auto& bodyBB = currentFn_->blocks[bodyBlock];
    if (bodyBB.instructions.empty() ||
        (bodyBB.instructions.back().op != OpCode::Jmp &&
         bodyBB.instructions.back().op != OpCode::Br &&
         bodyBB.instructions.back().op != OpCode::Ret)) {
        builder_->setInsertPoint(*currentFn_, bodyBlock);
        builder_->emitJmp(headerBlock, stmt.line);
    }
    builder_->popScope();

    builder_->setInsertPoint(*currentFn_, exitBlock);
    loopHeaderBlock_ = savedHeader;
    loopExitBlock_ = savedExit;
}

void ASTLowerer::lowerForCStyle(const ForCStyleStmt& stmt) {
    builder_->pushScope();
    if (stmt.init) lowerStmt(*stmt.init);

    int headerBlock = builder_->createBlock(*currentFn_, "bb_for_header");
    int bodyBlock = builder_->createBlock(*currentFn_, "bb_for_body");
    int postBlock = builder_->createBlock(*currentFn_, "bb_for_post");
    int exitBlock = builder_->createBlock(*currentFn_, "bb_for_exit");

    int savedHeader = loopHeaderBlock_;
    int savedExit = loopExitBlock_;
    loopHeaderBlock_ = postBlock;  // continue jumps to post-increment
    loopExitBlock_ = exitBlock;

    builder_->emitJmp(headerBlock, stmt.line);

    builder_->setInsertPoint(*currentFn_, headerBlock);
    IRValue cond = stmt.cond ? lowerExpr(*stmt.cond) : builder_->constBool(true);
    builder_->emitBr(cond, bodyBlock, exitBlock, stmt.line);

    builder_->setInsertPoint(*currentFn_, bodyBlock);
    if (stmt.body) lowerStmt(*stmt.body);
    auto& bodyBB = currentFn_->blocks[bodyBlock];
    if (bodyBB.instructions.empty() ||
        (bodyBB.instructions.back().op != OpCode::Jmp &&
         bodyBB.instructions.back().op != OpCode::Br &&
         bodyBB.instructions.back().op != OpCode::Ret)) {
        builder_->setInsertPoint(*currentFn_, bodyBlock);
        builder_->emitJmp(postBlock, stmt.line);
    }

    builder_->setInsertPoint(*currentFn_, postBlock);
    if (stmt.post) lowerExpr(*stmt.post);
    builder_->emitJmp(headerBlock, stmt.line);

    builder_->setInsertPoint(*currentFn_, exitBlock);
    loopHeaderBlock_ = savedHeader;
    loopExitBlock_ = savedExit;
    builder_->popScope();
}

void ASTLowerer::lowerReturn(const ReturnStmt& stmt) {
    if (stmt.value) {
        IRValue val = lowerExpr(*stmt.value);
        builder_->emitRet(val, stmt.line);
    } else {
        builder_->emitRetVoid(stmt.line);
    }
}

void ASTLowerer::lowerBreak(const BreakStmt& stmt) {
    if (loopExitBlock_ >= 0) {
        builder_->emitJmp(loopExitBlock_, stmt.line);
    }
}

void ASTLowerer::lowerContinue(const ContinueStmt& stmt) {
    if (loopHeaderBlock_ >= 0) {
        builder_->emitJmp(loopHeaderBlock_, stmt.line);
    }
}

void ASTLowerer::lowerExprStmt(const ExprStmt& stmt) {
    if (stmt.expr) lowerExpr(*stmt.expr);
}

void ASTLowerer::lowerMatch(const MatchStmt& stmt) {
    if (!stmt.value) return;
    IRValue matchVal = lowerExpr(*stmt.value);

    int mergeBlock = builder_->createBlock(*currentFn_, "bb_match_merge");

    for (size_t i = 0; i < stmt.cases.size(); i++) {
        const auto& c = stmt.cases[i];
        bool isDefault = !c.pattern;

        int caseBlock = builder_->createBlock(*currentFn_, "bb_case_" + std::to_string(i));
        int nextBlock = (i + 1 < stmt.cases.size())
            ? builder_->createBlock(*currentFn_, "bb_case_test_" + std::to_string(i + 1))
            : mergeBlock;

        if (isDefault) {
            builder_->emitJmp(caseBlock, c.line);
        } else {
            IRValue patternVal = lowerExpr(*c.pattern);
            IRValue cmp = builder_->emitBinOp(OpCode::ICmpEq, matchVal, patternVal, makeBoolType(), c.line);
            builder_->emitBr(cmp, caseBlock, nextBlock, c.line);
        }

        builder_->setInsertPoint(*currentFn_, caseBlock);
        if (c.body) lowerStmt(*c.body);
        auto& caseBB = currentFn_->blocks[caseBlock];
        if (caseBB.instructions.empty() ||
            (caseBB.instructions.back().op != OpCode::Jmp &&
             caseBB.instructions.back().op != OpCode::Br &&
             caseBB.instructions.back().op != OpCode::Ret)) {
            builder_->setInsertPoint(*currentFn_, caseBlock);
            builder_->emitJmp(mergeBlock, c.line);
        }

        if (!isDefault && nextBlock != mergeBlock) {
            builder_->setInsertPoint(*currentFn_, nextBlock);
        }
    }

    builder_->setInsertPoint(*currentFn_, mergeBlock);
}

void ASTLowerer::lowerTryCatch(const TryCatchStmt& stmt) {
    // ponytail: try/catch lowered as linear blocks; real unwinding deferred to runtime backend
    int tryBlock = builder_->createBlock(*currentFn_, "bb_try");
    int catchBlock = builder_->createBlock(*currentFn_, "bb_catch");
    int mergeBlock = builder_->createBlock(*currentFn_, "bb_try_merge");

    builder_->emitJmp(tryBlock, stmt.line);

    builder_->setInsertPoint(*currentFn_, tryBlock);
    if (stmt.tryBody) lowerStmt(*stmt.tryBody);
    builder_->emitJmp(mergeBlock, stmt.line);

    builder_->setInsertPoint(*currentFn_, catchBlock);
    builder_->pushScope();
    if (!stmt.catchVar.empty()) {
        IRValue errAlloca = builder_->emitAlloca(makeAnyType(), stmt.line);
        builder_->setVar(stmt.catchVar, errAlloca);
    }
    if (stmt.catchBody) lowerStmt(*stmt.catchBody);
    builder_->popScope();
    builder_->emitJmp(mergeBlock, stmt.line);

    if (stmt.finallyBody) {
        int finallyBlock = builder_->createBlock(*currentFn_, "bb_finally");
        builder_->setInsertPoint(*currentFn_, finallyBlock);
        lowerStmt(*stmt.finallyBody);
        builder_->emitJmp(mergeBlock, stmt.line);
    }

    builder_->setInsertPoint(*currentFn_, mergeBlock);
}

void ASTLowerer::lowerDefer(const DeferStmt& stmt) {
    // ponytail: defer lowered as inline; real LIFO defer stack deferred to runtime backend
    if (stmt.body) lowerStmt(*stmt.body);
}

void ASTLowerer::lowerThrow(const ThrowStmt& stmt) {
    if (stmt.value) {
        IRValue val = lowerExpr(*stmt.value);
        builder_->emitCall("__throw__", {val}, makeVoidType(), stmt.line);
    }
}

// ---------------------------------------------------------------------------
// Expression lowering
// ---------------------------------------------------------------------------
IRValue ASTLowerer::lowerExpr(const Expr& expr) {
    return std::visit([&](const auto& node) -> IRValue {
        using T = std::decay_t<decltype(node)>;

        if constexpr (std::is_same_v<T, IntLitExpr>) {
            return builder_->constInt(node.value);
        }
        else if constexpr (std::is_same_v<T, FloatLitExpr>) {
            return builder_->constFloat(node.value);
        }
        else if constexpr (std::is_same_v<T, BoolLitExpr>) {
            return builder_->constBool(node.value);
        }
        else if constexpr (std::is_same_v<T, NullLitExpr>) {
            return builder_->constNull();
        }
        else if constexpr (std::is_same_v<T, StringLitExpr>) {
            return builder_->constString(node.value);
        }
        else if constexpr (std::is_same_v<T, CharLitExpr>) {
            return builder_->constString(std::string(1, node.value));
        }
        else if constexpr (std::is_same_v<T, IdentExpr>) {
            if (builder_->hasVar(node.name)) {
                IRValue alloca = builder_->getVar(node.name);
                return builder_->emitLoad(alloca.type ? alloca.type->elementType : makeAnyType(),
                                          alloca, node.line);
            }
            // Unresolved — treat as external symbol reference
            return builder_->emitCall(node.name, {}, makeAnyType(), node.line);
        }
        else if constexpr (std::is_same_v<T, BinaryExpr>) {
            return lowerBinary(node);
        }
        else if constexpr (std::is_same_v<T, UnaryExpr>) {
            return lowerUnary(node);
        }
        else if constexpr (std::is_same_v<T, CallExpr>) {
            return lowerCall(node);
        }
        else if constexpr (std::is_same_v<T, AssignExpr>) {
            return lowerAssign(node);
        }
        else if constexpr (std::is_same_v<T, MemberExpr>) {
            return lowerMember(node);
        }
        else if constexpr (std::is_same_v<T, IndexExpr>) {
            return lowerIndex(node);
        }
        else if constexpr (std::is_same_v<T, TernaryExpr>) {
            return lowerTernary(node);
        }
        else if constexpr (std::is_same_v<T, LambdaExpr>) {
            return lowerLambda(node);
        }
        else if constexpr (std::is_same_v<T, NewExpr>) {
            TypePtr classType = makeClassType(node.className);
            IRValue obj = builder_->emitAllocHeap(node.className, classType, node.line);
            // Call constructor if args present
            if (!node.args.empty()) {
                std::vector<IRValue> args = {obj};
                for (const auto& a : node.args) {
                    args.push_back(lowerExpr(*a));
                }
                builder_->emitCall(node.className + ".init", args, makeVoidType(), node.line);
            }
            return obj;
        }
        else if constexpr (std::is_same_v<T, ListLitExpr>) {
            IRValue list = builder_->emitCall("__list_new__", {}, makeListType(makeAnyType()), node.line);
            for (const auto& e : node.elements) {
                IRValue val = lowerExpr(*e);
                builder_->emitCall("__list_push__", {list, val}, makeVoidType(), node.line);
            }
            return list;
        }
        else if constexpr (std::is_same_v<T, DictLitExpr>) {
            IRValue dict = builder_->emitCall("__dict_new__", {}, makeDictType(makeAnyType(), makeAnyType()), node.line);
            for (const auto& p : node.pairs) {
                IRValue key = lowerExpr(*p.first);
                IRValue val = lowerExpr(*p.second);
                builder_->emitCall("__dict_set__", {dict, key, val}, makeVoidType(), node.line);
            }
            return dict;
        }
        else if constexpr (std::is_same_v<T, PipelineExpr>) {
            IRValue left = lowerExpr(*node.left);
            // Pipeline: f |> g  => g(f)
            if (auto* call = std::get_if<CallExpr>(&node.right->data)) {
                std::string fnName;
                if (auto* id = std::get_if<IdentExpr>(&call->callee->data)) {
                    fnName = id->name;
                }
                std::vector<IRValue> args = {left};
                for (const auto& a : call->args) {
                    args.push_back(lowerExpr(*a));
                }
                return builder_->emitCall(fnName.empty() ? "__pipe__" : fnName, args, makeAnyType(), node.line);
            }
            // Simple ident pipeline: x |> f  => f(x)
            if (auto* id = std::get_if<IdentExpr>(&node.right->data)) {
                return builder_->emitCall(id->name, {left}, makeAnyType(), node.line);
            }
            IRValue right = lowerExpr(*node.right);
            return builder_->emitCall("__pipe__", {left, right}, makeAnyType(), node.line);
        }
        else if constexpr (std::is_same_v<T, NullCoalExpr>) {
            IRValue left = lowerExpr(*node.left);
            IRValue right = lowerExpr(*node.right);
            // ponytail: null coalescing lowered as call; real phi-based lowering when needed
            return builder_->emitCall("__null_coal__", {left, right}, makeAnyType(), node.line);
        }
        else if constexpr (std::is_same_v<T, PSStringExpr>) {
            // Process string segments
            IRValue result = builder_->constString("");
            for (const auto& seg : node.segments) {
                if (seg.isExpr && seg.expr) {
                    IRValue val = lowerExpr(*seg.expr);
                    IRValue str = builder_->emitCall("__to_string__", {val}, makeStringType(), node.line);
                    result = builder_->emitStrConcat(result, str, node.line);
                } else {
                    IRValue part = builder_->constString(seg.text);
                    result = builder_->emitStrConcat(result, part, node.line);
                }
            }
            return result;
        }
        else if constexpr (std::is_same_v<T, AddrOfExpr>) {
            IRValue val = lowerExpr(*node.operand);
            return builder_->emitUnaryOp(OpCode::Cast, val, makePointerType(val.type), node.line);
        }
        else if constexpr (std::is_same_v<T, DerefExpr>) {
            IRValue val = lowerExpr(*node.operand);
            TypePtr pointee = (val.type && val.type->elementType) ? val.type->elementType : makeAnyType();
            return builder_->emitLoad(pointee, val, node.line);
        }
        else if constexpr (std::is_same_v<T, DeleteExpr>) {
            IRValue val = lowerExpr(*node.operand);
            builder_->emitCall("__delete__", {val}, makeVoidType(), node.line);
            return IRValue::makeVoid();
        }
        else {
            return IRValue::makeVoid();
        }
    }, expr.data);
}

IRValue ASTLowerer::lowerBinary(const BinaryExpr& expr) {
    IRValue lhs = lowerExpr(*expr.left);
    IRValue rhs = lowerExpr(*expr.right);

    bool isFloat = (lhs.type && lhs.type->isFloat()) || (rhs.type && rhs.type->isFloat());
    bool isStr = (lhs.type && lhs.type->isString()) && (rhs.type && rhs.type->isString());

    // String concatenation
    if (expr.op == "+" && isStr) {
        return builder_->emitStrConcat(lhs, rhs, expr.line);
    }

    // Comparison operators
    if (expr.op == "==" || expr.op == "!=" ||
        expr.op == "<" || expr.op == "<=" ||
        expr.op == ">" || expr.op == ">=") {
        OpCode cmp = cmpOpToOpCode(expr.op, isFloat);
        return builder_->emitBinOp(cmp, lhs, rhs, makeBoolType(), expr.line);
    }

    // Logical operators
    if (expr.op == "&&") return builder_->emitBinOp(OpCode::LogAnd, lhs, rhs, makeBoolType(), expr.line);
    if (expr.op == "||") return builder_->emitBinOp(OpCode::LogOr, lhs, rhs, makeBoolType(), expr.line);

    // Arithmetic / bitwise
    OpCode op = binOpToOpCode(expr.op, isFloat);
    TypePtr resultType = isFloat ? makeFloatType() : makeIntType();
    if (expr.op == "&" || expr.op == "|" || expr.op == "^" ||
        expr.op == "<<" || expr.op == ">>") {
        resultType = makeIntType();
    }
    return builder_->emitBinOp(op, lhs, rhs, resultType, expr.line);
}

IRValue ASTLowerer::lowerUnary(const UnaryExpr& expr) {
    IRValue operand = lowerExpr(*expr.operand);

    if (expr.op == "-") {
        return builder_->emitUnaryOp(OpCode::Neg, operand,
            operand.type && operand.type->isFloat() ? makeFloatType() : makeIntType(), expr.line);
    }
    if (expr.op == "!") {
        return builder_->emitUnaryOp(OpCode::LogNot, operand, makeBoolType(), expr.line);
    }
    if (expr.op == "~") {
        return builder_->emitUnaryOp(OpCode::BitNot, operand, makeIntType(), expr.line);
    }
    return operand;
}

IRValue ASTLowerer::lowerCall(const CallExpr& expr) {
    std::string fnName;
    if (auto* id = std::get_if<IdentExpr>(&expr.callee->data)) {
        fnName = id->name;
    } else if (auto* mem = std::get_if<MemberExpr>(&expr.callee->data)) {
        // method call: obj.method(args) -> ClassName.method(obj, args)
        IRValue obj = lowerExpr(*mem->object);
        fnName = mem->member;

        std::vector<IRValue> args = {obj};
        for (const auto& a : expr.args) {
            args.push_back(lowerExpr(*a));
        }
        return builder_->emitCall(fnName, args, makeAnyType(), expr.line);
    }

    std::vector<IRValue> args;
    for (const auto& a : expr.args) {
        args.push_back(lowerExpr(*a));
    }
    return builder_->emitCall(fnName.empty() ? "__indirect_call__" : fnName, args, makeAnyType(), expr.line);
}

IRValue ASTLowerer::lowerAssign(const AssignExpr& expr) {
    IRValue val = lowerExpr(*expr.value);

    if (auto* id = std::get_if<IdentExpr>(&expr.target->data)) {
        if (builder_->hasVar(id->name)) {
            IRValue alloca = builder_->getVar(id->name);

            if (expr.op != "=") {
                // Compound assignment: load, apply op, store
                TypePtr loadType = alloca.type && alloca.type->elementType ? alloca.type->elementType : makeAnyType();
                IRValue current = builder_->emitLoad(loadType, alloca, expr.line);
                std::string binOp = expr.op.substr(0, expr.op.size() - 1); // "+=" -> "+"
                bool isFloat = (current.type && current.type->isFloat()) || (val.type && val.type->isFloat());
                OpCode op = binOpToOpCode(binOp, isFloat);
                TypePtr resultType = isFloat ? makeFloatType() : makeIntType();
                val = builder_->emitBinOp(op, current, val, resultType, expr.line);
            }

            builder_->emitStore(val, alloca, expr.line);
            return val;
        }
    }

    if (auto* mem = std::get_if<MemberExpr>(&expr.target->data)) {
        IRValue obj = lowerExpr(*mem->object);
        builder_->emitSetField(obj, -1, mem->member, val, expr.line);
        return val;
    }

    if (auto* idx = std::get_if<IndexExpr>(&expr.target->data)) {
        IRValue obj = lowerExpr(*idx->object);
        IRValue index = lowerExpr(*idx->index);
        builder_->emitCall("__index_set__", {obj, index, val}, makeVoidType(), expr.line);
        return val;
    }

    return val;
}

IRValue ASTLowerer::lowerMember(const MemberExpr& expr) {
    IRValue obj = lowerExpr(*expr.object);
    return builder_->emitGetField(obj, -1, expr.member, makeAnyType(), expr.line);
}

IRValue ASTLowerer::lowerIndex(const IndexExpr& expr) {
    IRValue obj = lowerExpr(*expr.object);
    IRValue idx = lowerExpr(*expr.index);
    return builder_->emitCall("__index_get__", {obj, idx}, makeAnyType(), expr.line);
}

IRValue ASTLowerer::lowerTernary(const TernaryExpr& expr) {
    IRValue cond = lowerExpr(*expr.cond);

    int thenBlock = builder_->createBlock(*currentFn_, "bb_ternary_then");
    int elseBlock = builder_->createBlock(*currentFn_, "bb_ternary_else");
    int mergeBlock = builder_->createBlock(*currentFn_, "bb_ternary_merge");

    builder_->emitBr(cond, thenBlock, elseBlock, expr.line);

    builder_->setInsertPoint(*currentFn_, thenBlock);
    IRValue thenVal = lowerExpr(*expr.thenExpr);
    int thenEndBlock = builder_->currentBlockIndex();
    builder_->emitJmp(mergeBlock, expr.line);

    builder_->setInsertPoint(*currentFn_, elseBlock);
    IRValue elseVal = lowerExpr(*expr.elseExpr);
    int elseEndBlock = builder_->currentBlockIndex();
    builder_->emitJmp(mergeBlock, expr.line);

    builder_->setInsertPoint(*currentFn_, mergeBlock);
    TypePtr phiType = thenVal.type ? thenVal.type : makeAnyType();
    return builder_->emitPhi(phiType, {{thenVal, thenEndBlock}, {elseVal, elseEndBlock}}, expr.line);
}

IRValue ASTLowerer::lowerLambda(const LambdaExpr& expr) {
    // Create anonymous function
    static int lambdaCounter = 0;
    std::string name = "__lambda_" + std::to_string(lambdaCounter++) + "__";

    FnDeclStmt fnDecl;
    fnDecl.name = name;
    fnDecl.params = expr.params;
    fnDecl.retType = expr.retType;
    fnDecl.body = nullptr;  // We'll lower the body directly
    fnDecl.isMethod = false;
    fnDecl.line = expr.line;

    // Save context, create function, lower body
    IRFunction* savedFn = currentFn_;
    int savedBlock = builder_->currentBlockIndex();

    IRFunction fn;
    fn.name = name;
    fn.returnType = parseTypeName(expr.retType);
    for (const auto& p : expr.params) {
        fn.params.push_back({p.first, parseTypeName(p.second)});
    }
    module_->functions.push_back(std::move(fn));
    auto& fnRef = module_->functions.back();

    int entryBlock = builder_->createBlock(fnRef, "bb_entry");
    builder_->setInsertPoint(fnRef, entryBlock);
    currentFn_ = &fnRef;
    builder_->pushScope();

    for (size_t i = 0; i < expr.params.size(); i++) {
        TypePtr pType = parseTypeName(expr.params[i].second);
        IRValue paramAlloca = builder_->emitAlloca(pType, expr.line);
        builder_->setVar(expr.params[i].first, paramAlloca);
    }

    if (expr.body) lowerStmt(*expr.body);
    ensureTerminator();
    builder_->popScope();

    currentFn_ = savedFn;
    if (savedFn) builder_->setInsertPoint(*savedFn, savedBlock);

    // Return a reference to the lambda function
    return builder_->emitCall("__fn_ref__", {}, makeFunctionType({}, makeAnyType()), expr.line);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
TypePtr ASTLowerer::inferExprType(const Expr& expr) {
    return std::visit([&](const auto& node) -> TypePtr {
        using T = std::decay_t<decltype(node)>;
        if constexpr (std::is_same_v<T, IntLitExpr>)     return makeIntType();
        if constexpr (std::is_same_v<T, FloatLitExpr>)   return makeFloatType();
        if constexpr (std::is_same_v<T, BoolLitExpr>)    return makeBoolType();
        if constexpr (std::is_same_v<T, NullLitExpr>)    return makeNullType();
        if constexpr (std::is_same_v<T, StringLitExpr>)  return makeStringType();
        if constexpr (std::is_same_v<T, CharLitExpr>)    return makePrimitiveType(TypeKind::Char);
        if constexpr (std::is_same_v<T, ListLitExpr>)    return makeListType(makeAnyType());
        if constexpr (std::is_same_v<T, DictLitExpr>)    return makeDictType(makeAnyType(), makeAnyType());
        return makeAnyType();
    }, expr.data);
}

OpCode ASTLowerer::binOpToOpCode(const std::string& op, bool isFloat) {
    if (op == "+")  return OpCode::Add;
    if (op == "-")  return OpCode::Sub;
    if (op == "*")  return OpCode::Mul;
    if (op == "/")  return OpCode::Div;
    if (op == "%")  return OpCode::Mod;
    if (op == "**") return OpCode::Pow;
    if (op == "&")  return OpCode::BitAnd;
    if (op == "|")  return OpCode::BitOr;
    if (op == "^")  return OpCode::BitXor;
    if (op == "<<") return OpCode::Shl;
    if (op == ">>") return OpCode::Shr;
    (void)isFloat;
    return OpCode::Add;
}

OpCode ASTLowerer::cmpOpToOpCode(const std::string& op, bool isFloat) {
    if (isFloat) {
        if (op == "==") return OpCode::FCmpEq;
        if (op == "!=") return OpCode::FCmpNe;
        if (op == "<")  return OpCode::FCmpLt;
        if (op == "<=") return OpCode::FCmpLe;
        if (op == ">")  return OpCode::FCmpGt;
        if (op == ">=") return OpCode::FCmpGe;
    }
    if (op == "==") return OpCode::ICmpEq;
    if (op == "!=") return OpCode::ICmpNe;
    if (op == "<")  return OpCode::ICmpLt;
    if (op == "<=") return OpCode::ICmpLe;
    if (op == ">")  return OpCode::ICmpGt;
    if (op == ">=") return OpCode::ICmpGe;
    return OpCode::ICmpEq;
}

void ASTLowerer::ensureTerminator() {
    if (!currentFn_ || currentFn_->blocks.empty()) return;
    int idx = builder_->currentBlockIndex();
    if (idx < 0 || idx >= static_cast<int>(currentFn_->blocks.size())) return;

    auto& bb = currentFn_->blocks[idx];
    if (bb.instructions.empty() ||
        (bb.instructions.back().op != OpCode::Ret &&
         bb.instructions.back().op != OpCode::Jmp &&
         bb.instructions.back().op != OpCode::Br)) {
        builder_->setInsertPoint(*currentFn_, idx);
        if (currentFn_->returnType && !currentFn_->returnType->isVoid()) {
            // Implicit return of default value
            IRValue zero = builder_->constInt(0);
            builder_->emitRet(zero, 0);
        } else {
            builder_->emitRetVoid(0);
        }
    }
}

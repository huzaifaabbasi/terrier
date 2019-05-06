#include "execution/compiler/function_builder.h"

#include "execution/ast/ast.h"
#include "execution/compiler/compiler_defs.h"
#include "execution/compiler/codegen.h"

namespace tpl::compiler {

FunctionBuilder::FunctionBuilder(CodeGen &codegen, ast::Identifier fn_name, util::RegionVector<ast::FieldDecl *> fn_params, ast::Expr *fn_ret_type)
: codegen_(codegen), prev_fn_(codegen_.GetCodeContext()->GetCurrentFunction()), fn_name_(fn_name),
fn_params_(std::move(fn_params)), fn_ret_type_(fn_ret_type), fn_body_(codegen_.GetRegion())
{
  codegen.GetCodeContext()->SetCurrentFunction(this);
}

ast::FunctionDecl *FunctionBuilder::Finish() {
  auto fn_ty = codegen_->NewFunctionType(DUMMY_POS, std::move(fn_params_), fn_ret_type_);
  auto fn_body = codegen_->NewBlockStmt(DUMMY_POS, DUMMY_POS, std::move(fn_body_));
  auto fn_lit = codegen_->NewFunctionLitExpr(fn_ty, fn_body);
  codegen_.GetCodeContext()->SetCurrentFunction(prev_fn_);
  return codegen_->NewFunctionDecl(DUMMY_POS, fn_name_, fn_lit);
}

}
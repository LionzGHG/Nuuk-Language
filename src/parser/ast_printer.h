
#ifndef NUUK_AST_PRINTER_H
#define NUUK_AST_PRINTER_H

#include "E:\THE_LANGUAGE\cstrap\parser\parser.h"

Visitor* create_ast_printer();
const char* print_expr(Visitor* self, Expr* expr);
const char* print_stmt(Visitor* self, Stmt* stmt);
const char* parenthesize(Visitor* self, const char* name, ExprArray* exprs);
void destroy_ast_printer(Visitor* visitor);

void dprint_stmt(Stmt* stmt);
void dprint_expr(Expr* expr);

// Vistor-Pattern
const char* visit_binary(Visitor* self, Binary* binary);
const char* visit_grouping(Visitor* self, Grouping* grouping);
const char* visit_literal(Visitor* self, Literal* literal);
const char* visit_unary(Visitor* self, Unary* unary);

#endif 
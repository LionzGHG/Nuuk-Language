
#ifndef NUUK_PARSER_H
#define NUUK_PARSER_H

#include "E:\THE_LANGUAGE\src\parser\ast.h"
#include <stdarg.h>
#include <stdbool.h>

typedef struct Parser {
    TokenArray* tokens;
    SymbolTable* datatypes;
    size_t current;
} Parser;

Parser* create_parser(TokenArray* tokens);
StmtArray parse(Parser* self);

bool parser_expect(Parser* self, int count, ...);
bool parser_check(Parser* self, TokenType type);
Token* parser_next(Parser* self);
bool parser_eof(Parser* self);
Token* parser_peek(Parser* self, size_t offset);
Token* parser_current(Parser* self);
Token* parser_back(Parser* self);
Token* parser_consume(Parser* self, TokenType type, const char* msg);

Stmt* declaration(Parser* self);
Stmt* statement(Parser* self);
Stmt* block(Parser* self);
Stmt* expression_stmt(Parser* self);

Stmt* return_stmt(Parser* self);
Stmt* import_stmt(Parser* self);
Stmt* expand_stmt(Parser* self);
Stmt* use_stmt(Parser* self);
Stmt* variable_decl(Parser* self);

Datatype* datatype(Parser* parser, Token* token);

Expr* expression(Parser* self);
Expr* assignment(Parser* self);
Expr* or(Parser* self);
Expr* and(Parser* self);
Expr* equality(Parser* self);
Expr* comparison(Parser* self);
Expr* term(Parser* self);
Expr* factor(Parser* self);
Expr* unary(Parser* self);
Expr* primary(Parser* self);
Expr* call(Parser* self, Expr* callee);


#endif
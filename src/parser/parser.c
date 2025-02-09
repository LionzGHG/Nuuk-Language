
#include "E:\THE_LANGUAGE\cstrap\parser\parser.h"

Parser* create_parser(TokenArray* tokens) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->current = 0;
    return parser;
}

StmtArray parse(Parser* self) {
    StmtArray stmts = create_stmt_array(2);
    while (!parser_eof(self)) {
        Stmt* stmt = declaration(self);
        if (!stmt) {
            fprintf(stderr, "%s ERROR: NULL statement encountered.\n", location(parser_current(self)));
            exit(1);
        }
        stmt_array_add(&stmts, stmt);
        parser_next(self);
    }

    printf("Parser finished!\n");
    return stmts;
}


bool parser_expect(Parser* self, int count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        TokenType type = va_arg(args, TokenType);
        if (parser_check(self, type)) {
            parser_next(self);
            va_end(args);
            return true;
        }
    }

    va_end(args);
    return false;
}

bool parser_check(Parser* self, TokenType type) {
    if (parser_eof(self)) return false;
    return parser_peek(self, 0)->type == type;
}

Token* parser_next(Parser* self) {
    if (!parser_eof(self)) self->current++;
    return parser_back(self);
}

bool parser_eof(Parser* self) {
    return parser_peek(self, 0)->type == END_OF_FILE;
}

Token* parser_peek(Parser* self, size_t offset) {
    Token* token = token_array_get(self->tokens, self->current + offset);
    if (!token) {
        fprintf(stderr, "%s ERROR: Peeked into oblivion.\n", location(parser_current(self)));
        exit(EXIT_FAILURE);
    }
    return token;
}

Token* parser_current(Parser* self) {
    return parser_peek(self, 0);
}

Token* parser_back(Parser* self) {
    Token* token = token_array_get(self->tokens, self->current - 1);
    if (!token) {
        fprintf(stderr, "%s ERROR: Failed to re-read previous token.\n", location(parser_current(self)));
        exit(EXIT_FAILURE);
    }
    return token;
}

Token* parser_consume(Parser* self, TokenType type, const char* msg) {
    if (parser_check(self, type)) return parser_next(self);
    fprintf(stderr, "%s ERROR: Unexpected Token '%s': %s", location(parser_current(self)), parser_current(self)->value, msg);
    exit(1);
}


Stmt* declaration(Parser* self) {
    // TODO: variable declarations, ...
    
    return statement(self);
}

Stmt* statement(Parser* self) {
    
    if (parser_expect(self, LBRACE)) return block(self);

    printf("parsing: %s\n", parser_current(self)->value);

    // TODO: for, while, return, ...
    if (strcmp(parser_current(self)->value, "return") == 0) {
        printf("parsing return!!!\n");
        return return_stmt(self);
    }

    if (parser_check(self, EXPAND)) {
        return expand_stmt(self);
    }

    if (parser_check(self, IMPORT)) {
        return import_stmt(self);
    }

    if (parser_check(self, USE)) {
        return use_stmt(self);
    }

    return expression_stmt(self);
}

Stmt* return_stmt(Parser* self) {
    parser_next(self);
    Expr* expr = expression(self);
    parser_consume(self, SEMICOLON, "Expected ';' after return statement.");
    return (Stmt*)create_return(expr);
}

Stmt* import_stmt(Parser* self) {
    parser_next(self);
    Expr* expr = expression(self);
    parser_consume(self, SEMICOLON, "Expected ';' after import statement.");
    return (Stmt*)create_import(expr);
}

Stmt* expand_stmt(Parser* self) {
    parser_next(self);
    Expr* expr = expression(self);
    parser_consume(self, SEMICOLON, "Expected ';' after expand statement.");
    return (Stmt*)create_expand(expr);
}

Stmt* use_stmt(Parser* self) {
    parser_next(self);
    Expr* expr = expression(self);
    parser_consume(self, SEMICOLON, "Expected ';' after use statement.");
    return (Stmt*)create_use(expr);
}

Stmt* block(Parser* self) {
    StmtArray stmts = create_stmt_array(2);

    while (!parser_check(self, RBRACE) && !parser_eof(self)) {
        stmt_array_add(&stmts, declaration(self));
    }

    parser_consume(self, RBRACE, "Expected '}' after block.");
    return (Stmt*)create_block(&stmts);
}

Stmt* expression_stmt(Parser* self) {
    Expr* expr = expression(self);
    parser_consume(self, SEMICOLON, "Expected ';' after expression.");
    Stmt* stmt = (Stmt*)create_expression(expr);
    if (!stmt) {
        fprintf(stderr, "%s FATAL ERROR: Failed to create expression statement.\n", location(parser_current(self)));
        exit(1);
    }
    return stmt;
}


Expr* expression(Parser* self) {
    return assignment(self);
}

Expr* assignment(Parser* self) {
    Expr* expr = or(self);

    if (parser_expect(self, 5, ASSIGN, PLUS_EQ, MINUS_EQ, STAR_EQ, SLASH_EQ)) {
        Token* eq = parser_back(self);
        Expr* val = assignment(self);
        
        if (expr->type == EXPR_VARIABLE) {
            Variable* var = (Variable*)expr;
            return (Expr*)create_assign(var->name, val);
        }

        fprintf(stderr, "%s ERROR: Invalid assignment target: '%s'.", location(eq), eq->value);
        exit(EXIT_FAILURE);
    }

    return expr;
}

Expr* or(Parser* self) {
    Expr* expr = and(self);

    while (parser_expect(self, 1, OR)) {
        Token* op = parser_back(self);
        Expr* rhs = and(self);
        expr = (Expr*)create_logical(expr, *op, rhs);
    }

    return expr;
}

Expr* and(Parser* self) {
    Expr* expr = equality(self);

    while (parser_expect(self, 1, AND)) {
        Token* op = parser_back(self);
        Expr* rhs = equality(self);
        expr = (Expr*)create_logical(expr, *op, rhs);
    }

    return expr;
}

Expr* equality(Parser* self) {
    Expr* expr = comparison(self);

    while (parser_expect(self, 2, NEQ, EQ)) {
        Token* op = parser_back(self);
        Expr* rhs = comparison(self);
        expr = (Expr*)create_binary(expr, *op, rhs);
    }
    
    return expr;
}

Expr* comparison(Parser* self) {
    Expr* expr = term(self);

    while (parser_expect(self, 4, GT, LT, LTE, GTE)) {
        Token* op = parser_back(self);
        Expr* rhs = term(self);
        expr = (Expr*)create_binary(expr, *op, rhs);
    }

    return expr;
}

Expr* term(Parser* self) {
    Expr* expr = factor(self);

    while (parser_expect(self, 2, MINUS, PLUS)) {
        Token* op = parser_back(self);
        Expr* rhs = factor(self);
        Expr* new_expr = (Expr*)create_binary(expr, *op, rhs);
        if (!new_expr) {
            fprintf(stderr, "%s FATAL ERROR: Failed to create new binary expression.\n", location(parser_current(self)));
            exit(1);
        }
        expr = new_expr;
    }

    return expr;
}

Expr* factor(Parser* self) {
    Expr* expr = unary(self);

    while (parser_expect(self, 2, SLASH, STAR)) {
        Token* op = parser_back(self);
        Expr* rhs = unary(self);
        expr = (Expr*)create_binary(expr, *op, rhs);
    }

    return expr;
}

Expr* unary(Parser* self) {
    if (parser_expect(self, 4, BANG, AMPERSAND, STAR, MINUS)) {
        Token* op = parser_back(self);
        Expr* rhs = unary(self);
        return (Expr*)create_unary(*op, rhs);
    }

    return primary(self);
}

Expr* primary(Parser* self) {
    if (parser_expect(self, 1, FALSE)) {
        return (Expr*)create_literal("false");
    }
    
    if (parser_expect(self, 1, TRUE)) {
        return (Expr*)create_literal("true");
    }

    if (parser_expect(self, 2, NUMBER, STRING)) {
        return (Expr*)create_literal(parser_back(self)->value);
    }

    if (parser_expect(self, 1, IDENTIFIER)) {
        Expr* expr = (Expr*)create_variable(*parser_back(self));

        for (;;) {
            if (parser_expect(self, 1, DOT)) {
                Token* property = parser_consume(self, IDENTIFIER, "Expected property name after '.'.");
                expr = (Expr*)create_get(expr, *property);
            }  else if (parser_check(self, LPAREN)) {
                expr = call(self, expr);
            } else {
                break;
            }
        }


        return expr;
    }

    if (parser_expect(self, 1, LPAREN)) {
        Expr* expr = expression(self);
        parser_consume(self, RPAREN, "Expected ')' after grouping expression.");
        return (Expr*)create_grouping(expr);
    }

    fprintf(stderr, "%s ERROR: Expected expression, got: '%s'.\n", location(parser_current(self)), parser_current(self)->value);
    exit(1);
}

Expr* call(Parser* self, Expr* callee) {
    parser_consume(self, LPAREN, "Expect '(' after function name.");
    ExprArray args = create_expr_array(2);

    if (!parser_check(self, RPAREN)) {
        for (;;) {
            if (args.size + 1 <= 255) {
                expr_array_add(&args, expression(self));
            } else {
                fprintf(stderr, "%s ERROR: Maximum amount of arguments reached (max. 255).\n", location(parser_current(self)));
                exit(1);
            } 

            if (!parser_expect(self, 1, COMMA)) break;

            if (parser_check(self, RPAREN)) {
                fprintf(stderr, "%s ERROR: Trailing comma in function call.\n", location(parser_current(self)));
                exit(1);
            }
        }
    }

    parser_consume(self, RPAREN, "Expected ')' after function arguments.");
    return (Expr*)create_call(callee, args);
}

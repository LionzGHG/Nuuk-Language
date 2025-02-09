
#ifndef NUUK_AST_H
#define NUUK_AST_H

#include "E:\THE_LANGUAGE\cstrap\utils\utils.h"

typedef struct Visitor Visitor;
typedef struct Expr Expr;
typedef struct Stmt Stmt;

typedef enum StmtType StmtType;
typedef enum ExprType ExprType;

typedef struct Binary Binary;
typedef struct Grouping Grouping;
typedef struct Literal Literal;
typedef struct Logical Logical;
typedef struct Unary Unary;
typedef struct Variable Variable;
typedef struct Assign Assign;
typedef struct Get Get;
typedef struct Call Call;

typedef struct Expression Expression;
typedef struct Block Block;
typedef struct Return Return;
typedef struct Import Import;
typedef struct Expand Expand;
typedef struct Use Use;

typedef struct Visitor {
    // Expressions
    const char* (*visit_binary)(struct Visitor* self, Binary* binary);
    const char* (*visit_grouping)(struct Visitor* self, Grouping* grouping);
    const char* (*visit_literal)(struct Visitor* self, Literal* literal);
    const char* (*visit_logical)(struct Visitor* self, Logical* logical);
    const char* (*visit_unary)(struct Visitor* self, Unary* unary);
    const char* (*visit_variable)(struct Visitor* self, Variable* variable);
    const char* (*visit_assign)(struct Visitor* self, Assign* assign);
    const char* (*visit_get)(struct Visitor* self, Get* get);
    const char* (*visit_call)(struct Visitor* self, Call* call);

    // Statements
    void (*visit_expression)(struct Visitor* self, Expression* expression);
    void (*visit_block)(struct Visitor* self, Block* block);
    void (*visit_return)(struct Visitor* self, Return* return_);
    void (*visit_import)(struct Visitor* self, Import* import);
    void (*visit_expand)(struct Visitor* self, Expand* expand);
    void (*visit_use)(struct Visitor* self, Use* use);
} Visitor;

typedef enum StmtType {
    STMT_EXPRESSION,
    STMT_BLOCK,
    STMT_IMPORT,
    STMT_EXPAND,
    STMT_RETURN,
    STMT_USE,
} StmtType;

typedef enum ExprType {
    EXPR_BINARY,
    EXPR_GROUPING,
    EXPR_LITERAL,
    EXPR_LOGICAL,
    EXPR_UNARY,
    EXPR_VARIABLE,
    EXPR_ASSIGN,
    EXPR_GET,
    EXPR_CALL
} ExprType;

typedef struct Expr {
    ExprType type;
    const char* (*accept)(struct Expr* self, Visitor* visitor);
} Expr;

typedef struct Stmt {
    StmtType type;
    void (*accept)(struct Stmt* self, Visitor* visitor);
} Stmt;

// ################################################################
// # EXPRESSIONS
// ################################################################

typedef struct ExprArray {
    Expr** elements;
    int capacity;
    int size; 
} ExprArray;

typedef struct Binary {
    Expr base;
    Expr* lhs;
    Token op;
    Expr* rhs;
} Binary;

typedef struct Grouping {
    Expr base;
    Expr* expr;
} Grouping;

typedef struct Literal {
    Expr base;
    const char* value;
} Literal;

typedef struct Logical {
    Expr base;
    Expr* lhs;
    Token op;
    Expr* rhs;
} Logical;

typedef struct Unary {
    Expr base;
    Token op;
    Expr* rhs;
} Unary;

typedef struct Variable {
    Expr base;
    Token name;
} Variable;

typedef struct Assign {
    Expr base;
    Token name;
    Expr* value;
} Assign;

typedef struct Get {
    Expr base;
    Expr* expr;
    Token property;
} Get;

typedef struct Call {
    Expr base;
    Expr* callee;
    ExprArray args;
} Call;

// ################################################################
// # STATEMENTS
// ################################################################

typedef struct StmtArray {
    Stmt** elements;
    int capacity;
    int size; 
} StmtArray;

typedef struct Expression {
    Stmt base;
    Expr* expr;
} Expression;

typedef struct Block {
    Stmt base;
    StmtArray* body;
} Block;

typedef struct Return {
    Stmt base;
    Expr* value;
} Return;

typedef struct Import {
    Stmt base;
    Expr* value;
} Import;

typedef struct Expand {
    Stmt base;
    Expr* value;
} Expand;

typedef struct Use {
    Stmt base;
    Expr* value;
} Use;

// ################################################################
// # FUNC DEFS
// ################################################################

StmtArray create_stmt_array(int capacity);
void stmt_array_add(StmtArray* array, Stmt* stmt);
void free_stmt_array(StmtArray* array);

ExprArray create_expr_array(int capacity);
void expr_array_add(ExprArray* array, Expr* expr);
void free_expr_array(ExprArray* array);

const char* id(Stmt* stmt);

Binary* create_binary(Expr* lhs, Token op, Expr* rhs);
Grouping* create_grouping(Expr* expr);
Literal* create_literal(const char* value);
Logical* create_logical(Expr* lhs, Token op, Expr* rhs);
Unary* create_unary(Token op, Expr* rhs);
Variable* create_variable(Token name);
Assign* create_assign(Token name, Expr* value);
Get* create_get(Expr* expr, Token property);
Call* create_call(Expr* callee, ExprArray args);

Expression* create_expression(Expr* expr);
Block* create_block(StmtArray* stmts);
Return* create_return(Expr* value);
Import* create_import(Expr* value);
Expand* create_expand(Expr* value);
Use* create_use(Expr* value);

const char* binary_accept(Expr* self, Visitor* visitor);
const char* grouping_accept(Expr* self, Visitor* visitor);
const char* literal_accept(Expr* self, Visitor* visitor);
const char* logical_accept(Expr* self, Visitor* visitor);
const char* unary_accept(Expr* self, Visitor* visitor);
const char* variable_accept(Expr* self, Visitor* visitor);
const char* assign_accept(Expr* self, Visitor* visitor);
const char* get_accept(Expr* self, Visitor* visitor);
const char* call_accept(Expr* self, Visitor* visitor);

void expression_accept(Stmt* expression, Visitor* visitor);
void block_accept(Stmt* block, Visitor* visitor);
void return_accept(Stmt* return_stmt, Visitor* visitor);
void import_accept(Stmt* import_stmt, Visitor* visitor);
void expand_accept(Stmt* expand_stmt, Visitor* visitor);
void use_accept(Stmt* use_stmt, Visitor* visitor);

#endif
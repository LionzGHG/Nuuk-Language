
#include "E:\THE_LANGUAGE\cstrap\parser\ast_printer.h"

void dprint_stmt(Stmt* stmt) {
    switch (stmt->type) {
        case STMT_EXPRESSION:
            printf("STMT_EXPRESSION\n");
            break;
        case STMT_RETURN:
            printf("STMT_RETURN(");
            Expr* expr = ((Return*)stmt)->value;
            dprint_expr(expr);
            printf(");\n");
            break;
        case STMT_BLOCK:
            printf("STMT_BLOCK\n");
            break;
        case STMT_IMPORT:
            printf("STMT_IMPORT\n");
            break;
        case STMT_USE:
            printf("STMT_USE\n");
            break;
        case STMT_EXPAND:
            printf("STMT_EXPAND\n");
            break;
        default:
            printf("STMT_UNKOWN\n");
            break;
    }
}

void dprint_expr(Expr* expr) {
    if (!expr) {
        printf("ERROR: NULL expr: %p\n", (void*)expr);
        return;
    }
    switch (expr->type) {
        case EXPR_LITERAL:
            Literal* literal = (Literal*)expr;
            printf("EXPR_LITERAL %s", literal->value);
            break;
        case EXPR_BINARY:
            Binary* binary = (Binary*)expr;
            printf("EXPR_BINARY(");
            dprint_expr(binary->lhs);
            printf(" %s ", binary->op.value);
            dprint_expr(binary->rhs);
            printf(")");
            break;
        default:
            printf("EXPR_UNKOWN");
            break;
    }
}

Visitor* create_ast_printer() {
    Visitor* visitor = (Visitor*)malloc(sizeof(Visitor));
    visitor->visit_binary = visit_binary;
    visitor->visit_grouping = visit_grouping;
    visitor->visit_literal = visit_literal;
    visitor->visit_unary = visit_unary;

    return visitor;
}

const char* print_expr(Visitor* self, Expr* expr) {
    return expr->accept(expr, self);
}

const char* print_stmt(Visitor* self, Stmt* stmt) {
    switch (stmt->type) {
        case STMT_EXPRESSION:
            Expression* expression = (Expression*)stmt;
            return print_expr(self, expression->expr);
        case STMT_RETURN:
            Return* return_stmt = (Return*)stmt;
            return print_expr(self, return_stmt->value);
        default:
            fprintf(stderr, "FATAL ERROR: unimplemented statement type passed to ast_printer!\n");
            exit(1);
    }
}

const char* parenthesize(Visitor* self, const char* name, ExprArray* exprs) {
    char builder[1024] = "";

    strncat(builder, "(", sizeof(builder) - sizeof("(") - 1);
    strncat(builder, name, sizeof(builder) - sizeof(name) - 1);

    for (int i = 0; i < exprs->size; i++) {
        Expr expr = *exprs->elements[i];
        strncat(builder, " ", sizeof(builder) - sizeof(" ") - 1);
        const char* result = expr.accept(&expr, self);
        snprintf(builder + strlen(builder), sizeof(builder) - strlen(builder), " %s", result);
    }

    strncat(builder, ")", sizeof(builder) - sizeof(")") - 1);

    return strdup(builder);
}


// Vistor-Pattern
const char* visit_binary(Visitor* self, Binary* binary) {
    ExprArray exprs = create_expr_array(2);
    expr_array_add(&exprs, binary->lhs);
    expr_array_add(&exprs, binary->rhs);

    return parenthesize(self, binary->op.value, &exprs);
}

const char* visit_grouping(Visitor* self, Grouping* grouping) {
    ExprArray exprs = create_expr_array(1);
    expr_array_add(&exprs, grouping->expr);

    return parenthesize(self, "group", &exprs);
}

const char* visit_literal(Visitor* self, Literal* literal) {
    return strdup(literal->value);
}

const char* visit_unary(Visitor* self, Unary* unary) {
    ExprArray exprs = create_expr_array(1);
    expr_array_add(&exprs, unary->rhs);

    return parenthesize(self, unary->op.value, &exprs);
}

void destroy_ast_printer(Visitor* visitor) {
    free(visitor);
}

#include "E:\THE_LANGUAGE\src\parser\ast_printer.h"

void dprint_stmt(Stmt* stmt) {
    switch (stmt->type) {
        case STMT_EXPRESSION:
            Expr* expr = ((Expression*)stmt)->expr;
            printf("STMT_EXPRESSION(");
            dprint_expr(expr);
            printf(");\n");
            break;
        case STMT_RETURN:
            printf("STMT_RETURN(");
            expr = ((Return*)stmt)->value;
            dprint_expr(expr);
            printf(");\n");
            break;
        case STMT_BLOCK:
            printf("STMT_BLOCK\n");
            break;
        case STMT_IMPORT:
            printf("STMT_IMPORT(");
            expr = ((Import*)stmt)->value;
            dprint_expr(expr);
            printf(");\n");
            break;
        case STMT_USE:
            printf("STMT_USE(");
            expr = ((Use*)stmt)->value;
            dprint_expr(expr);
            printf(");\n");
            break;
        case STMT_EXPAND:
            printf("STMT_EXPAND(");
            expr = ((Expand*)stmt)->value;
            dprint_expr(expr);
            printf(");\n");
            break;
        case STMT_VAR:
            VariableDecl* var = (VariableDecl*)stmt;
            printf("STMT_VAR(");
            printf(var->mutability ? "MUTABLE, " : "CONSTANT, ");
            dprint_typeid(var->type);
            printf(", %s, ", var->name->value);
            if (var->value != NULL) dprint_expr(var->value);
            else printf("NULL");
            printf(");\n");
            break;
        default:
            printf("STMT_UNKOWN\n");
            break;
    }
}

void dprint_typeid(Datatype* type) {
    switch (type->type) {
        case TYPEID_BASIC:
            BasicType* basic = (BasicType*)type;
            if (!basic->name) { 
                fprintf(stderr, "ERROR: BasicType has no name\n");
                exit(1);
            }
            printf("%s", basic->name);
            break;
        case TYPEID_POINTER:
            Pointer* inner = (Pointer*)type;
            dprint_typeid(inner->type);
            printf("*");
            break;
        default:
            printf("TYPEID_UNKOWN\n");
            return;
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
        case EXPR_VARIABLE:
            Variable* variable = (Variable*)expr;
            printf("EXPR_VARIABLE %s", variable->name.value);
            break;
        case EXPR_BINARY:
            Binary* binary = (Binary*)expr;
            printf("EXPR_BINARY(");
            dprint_expr(binary->lhs);
            printf(" %s ", binary->op.value);
            dprint_expr(binary->rhs);
            printf(")");
            break;
        case EXPR_UNARY:
            Unary* unary = (Unary*)expr;
            printf("EXPR_UNARY(%s, ", unary->op.value);
            dprint_expr(unary->rhs);
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

#include "ast.h"

StmtArray create_stmt_array(int capacity) {
    StmtArray array;
    array.elements = (Stmt**)malloc(sizeof(Stmt*) * capacity);
    array.capacity = capacity;
    array.size = 0;

    return array;
}

void stmt_array_add(StmtArray* array, Stmt* stmt) {
    if (!array || !stmt) {
        fprintf(stderr, "FATAL ERROR: Invalid array or stmt passed to stmt_array_add.\n");
        return;
    }

    if (array->size >= array->capacity) {
        array->capacity *= 2;
        array->elements = realloc(array->elements, array->capacity * sizeof(Stmt*));
        if (!array->elements) {
            fprintf(stderr, "FATAL ERROR: Failed to resize stmt array.\n");
            exit(EXIT_FAILURE);
        }
    }

    array->elements[array->size++] = stmt;
}

void free_stmt_array(StmtArray* array) {
    if (!array) return;

    for (int i = 0; i < array->size; i++) {
        free(array->elements[i]);
    }
    free(array->elements);
    free(array);
}

ExprArray create_expr_array(int capacity) {
    ExprArray array;
    array.elements = (Expr**)malloc(sizeof(Expr*) * capacity);
    array.capacity = capacity;
    array.size = 0;

    return array;
}

void expr_array_add(ExprArray* array, Expr* expr) {
    if (!array || !expr) {
        fprintf(stderr, "FATAL ERROR: Invalid array or expr passed to expr_array_add.\n");
        return;
    }

    if (array->size >= array->capacity) {
        array->capacity *= 2;
        array->elements = realloc(array->elements, array->capacity * sizeof(Expr*));
        if (!array->elements) {
            fprintf(stderr, "FATAL ERROR: Failed to resize expr array.\n");
            exit(EXIT_FAILURE);
        }
    }

    array->elements[array->size++] = expr;    
}

void free_expr_array(ExprArray* array) {
    for (int i = 0; i < array->size; i++) {
        free(array->elements[i]);
    }
    free(array->elements);
    free(array);
}

const char* id(Stmt* stmt) {
    switch (stmt->type) {
        case STMT_EXPRESSION:
            return "STMT_EXPRESSION";
        default:
            return "STMT_UNKOWN";
    }
}

Datatype* basic_type(const char* name) {
    BasicType* basic_type = (BasicType*)malloc(sizeof(BasicType));
    if (!basic_type) {
        fprintf(stderr, "ERROR: Failed to allocate memory for BasicType!\n");
        exit(1);
    }
    basic_type->base.type = TYPEID_BASIC;
    basic_type->name = strdup(name);

    return (Datatype*)basic_type;
}

Datatype* pointer(Datatype* type) {
    Pointer* ptr = (Pointer*)malloc(sizeof(Pointer));
    ptr->base.type = TYPEID_POINTER;
    ptr->type = type;

    return (Datatype*)ptr;
}

Datatype* generic(Datatype* type) {
    fprintf(stderr, "TODO\n");
    exit(1);
}

Datatype* array(size_t array_size, Datatype** types) {
    Array* array = (Array*)malloc(sizeof(Array));
    array->base.type = TYPEID_ARRAY;
    array->array_size = array_size;
    array->types = types;

    return (Datatype*)array;
}

Datatype* tuple(Datatype** types) {
    Tuple* tuple = (Tuple*)malloc(sizeof(Tuple));
    tuple->base.type = TYPEID_TUPLE;
    tuple->types = types;

    return (Datatype*)tuple;
}

VariableDecl* create_variable_stmt(bool mutability, Datatype* type, Token* name, Expr* value) {
    VariableDecl* var = (VariableDecl*)malloc(sizeof(VariableDecl));
    var->base.type = STMT_VAR;
    var->mutability = mutability;
    var->type = type;
    var->name = name;
    var->value = value;

    return var;
}

Binary* create_binary(Expr* lhs, Token op, Expr* rhs) {
    Binary* binary = (Binary*)malloc(sizeof(Binary));
    binary->base.type = EXPR_BINARY;
    binary->base.accept = binary_accept;

    binary->lhs = lhs;
    binary->op = op;
    binary->rhs = rhs;

    return binary;
}

Grouping* create_grouping(Expr* expr) {
    Grouping* grouping = (Grouping*)malloc(sizeof(Grouping));
    grouping->base.type = EXPR_GROUPING;
    grouping->base.accept = grouping_accept;

    grouping->expr = expr;

    return grouping;
}

Literal* create_literal(const char* value) {
    Literal* literal = (Literal*)malloc(sizeof(Literal));
    literal->base.type = EXPR_LITERAL;
    literal->base.accept = literal_accept;

    literal->value = value;

    return literal;
}

Logical* create_logical(Expr* lhs, Token op, Expr* rhs) {
    Logical* logical = (Logical*)malloc(sizeof(Logical));
    logical->base.type = EXPR_LOGICAL;
    logical->base.accept = logical_accept;

    logical->lhs = lhs;
    logical->op = op;
    logical->rhs = rhs;

    return logical;
}

Unary* create_unary(Token op, Expr* rhs) {
    Unary* unary = (Unary*)malloc(sizeof(Unary));
    unary->base.type = EXPR_UNARY;
    unary->base.accept = unary_accept;

    unary->op = op;
    unary->rhs = rhs;

    return unary;
}

Variable* create_variable(Token name) {
    Variable* variable = (Variable*)malloc(sizeof(Variable));
    variable->base.type = EXPR_VARIABLE;
    variable->base.accept = variable_accept;

    variable->name = name;

    return variable;
}

Assign* create_assign(Token name, Expr* value) {
    Assign* assign = (Assign*)malloc(sizeof(Assign));
    assign->base.type = EXPR_ASSIGN;
    assign->base.accept = assign_accept;

    assign->name = name;
    assign->value = value;

    return assign;
}

Get* create_get(Expr* expr, Token property) {
    Get* get = (Get*)malloc(sizeof(Get));
    get->base.type = EXPR_GET;
    get->base.accept = get_accept;

    get->expr = expr;
    get->property = property;

    return get;
}

Call* create_call(Expr* callee, ExprArray args) {
    Call* call = (Call*)malloc(sizeof(Call));
    call->base.type = EXPR_CALL;
    call->base.accept = call_accept;

    call->callee = callee;
    call->args = args;

    return call;
}

Expression* create_expression(Expr* expr) {
    Expression* expression = (Expression*)malloc(sizeof(Expression));
    if (!expression) {
        fprintf(stderr, "(MEMFLT) Memory allocation failed.\n");
        exit(1);
    }

    expression->base.type = STMT_EXPRESSION;
    expression->base.accept = expression_accept;

    expression->expr = expr;

    return expression;
}

Block* create_block(StmtArray* stmts) {
    Block* block = (Block*)malloc(sizeof(Block));
    block->base.type = STMT_BLOCK;
    block->base.accept = block_accept;

    block->body = stmts;
    return block;
}

Return* create_return(Expr* value) {
    Return* return_stmt = (Return*)malloc(sizeof(Return));
    return_stmt->base.type = STMT_RETURN;
    return_stmt->base.accept = return_accept;

    return_stmt->value = value;
    return return_stmt;
}

Import* create_import(Expr* value) {
    Import* import_stmt = (Import*)malloc(sizeof(Import));
    import_stmt->base.type = STMT_IMPORT;
    import_stmt->base.accept = import_accept;

    import_stmt->value = value;
    return import_stmt;
}

Expand* create_expand(Expr* value) {
    Expand* expand_stmt = (Expand*)malloc(sizeof(Expand));
    expand_stmt->base.type = STMT_EXPAND;
    expand_stmt->base.accept = expand_accept;

    expand_stmt->value = value;
    return expand_stmt;
}

Use* create_use(Expr* value) {
    Use* use_stmt = (Use*)malloc(sizeof(Use));
    use_stmt->base.type = STMT_USE;
    use_stmt->base.accept = use_accept;

    use_stmt->value = value;
    return use_stmt;
}

const char* binary_accept(Expr* self, Visitor* visitor) {
    return visitor->visit_binary(visitor, (Binary*)self);
}

const char* grouping_accept(Expr* self, Visitor* visitor) {
    return visitor->visit_grouping(visitor, (Grouping*)self);
}

const char* literal_accept(Expr* self, Visitor* visitor) {
    return visitor->visit_literal(visitor, (Literal*)self);
}

const char* logical_accept(Expr* self, Visitor* visitor) {
    return visitor->visit_logical(visitor, (Logical*)self);
}

const char* unary_accept(Expr* self, Visitor* visitor) {
    return visitor->visit_unary(visitor, (Unary*)self);
}

const char* variable_accept(Expr* self, Visitor* visitor) {
    return visitor->visit_variable(visitor, (Variable*)self);
}

const char* assign_accept(Expr* self, Visitor* visitor) {
    return visitor->visit_assign(visitor, (Assign*)self);
}

const char* get_accept(Expr* self, Visitor* visitor) {
    return visitor->visit_get(visitor, (Get*)self);
}

const char* call_accept(Expr* self, Visitor* visitor) {
    return visitor->visit_call(visitor, (Call*)self);
}

void expression_accept(Stmt* expression, Visitor* visitor) {
    visitor->visit_expression(visitor, (Expression*)expression);
}

void block_accept(Stmt* block, Visitor* visitor) {
    visitor->visit_block(visitor, (Block*)block);
}

void return_accept(Stmt* return_stmt, Visitor* visitor) {
    visitor->visit_return(visitor, (Return*)return_stmt);
}

void import_accept(Stmt* import_stmt, Visitor* visitor) {
    visitor->visit_import(visitor, (Import*)import_stmt);
}

void expand_accept(Stmt* expand_stmt, Visitor* visitor) {
    visitor->visit_expand(visitor, (Expand*)expand_stmt);
}

void use_accept(Stmt* use_stmt, Visitor* visitor) {
    visitor->visit_use(visitor, (Use*)use_stmt);
}
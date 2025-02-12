
#include "utils.h"

#include <ctype.h>

Token* create_token(TokenType type, const char* value, size_t ln, size_t col) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->value = value;
    token->ln = ln;
    token->col = col;
    
    return token;
}

void print_token(Token* token) {
    printf("[");
    print_token_type(token);
    printf(", %s]\n", token->value);
}

void print_token_type(Token* token) {
    switch (token->type) {
        case IDENTIFIER:
            printf("IDENTIFIER");
            break;
        case STRING:
            printf("STRING");
            break;
        case CHAR:
            printf("CHAR");
            break;
        case NUMBER:
            printf("NUMBER");
            break;
        default:
            printf("%s", to_upper(token->value));
            break;
    }
}

char* to_upper(const char* c) {
    if (!c) return NULL;

    size_t len = 0;
    while (c[len] != '\0') len++;

    char* upper = (char*)malloc(len + 1);
    if (!upper) return NULL;

    for (size_t i = 0; i < len; i++) upper[i] = toupper((unsigned char)c[i]);

    upper[len] = '\0';
    return upper;
}

char* location(Token* token) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "[%zu:%zu]", token->ln, token->col);
    return 0;
}

char* concat(const char* source, size_t start, size_t end) {
    if (!source || start >= end || start >= strlen(source)) {
        fprintf(stderr, "ERROR: Invalid substring range [%zu, %zu]\n", start, end);
        return NULL;
    }

    size_t length = end - start;
    char* result = (char*)malloc(length + 1);
    if (!result) {
        fprintf(stderr, "ERROR: Failed to allocate memory for string!\n");
        return NULL;
    }

    strncpy(result, source + start, length);
    result[length] = '\0';
    return result;
}

unsigned int hash_function(const char* key) {
    unsigned int hash = 5381;
    while (*key) {
        hash = ((hash << 5) + hash) + (unsigned char)(*key++); // hash * 33 + key[i]
    }
    return hash % HASH_TABLE_SIZE;
}

HashMap* create_hash_map() {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        map->entries[i] = NULL;
    }
    return map;
}

void hash_insert(HashMap* map, const char* key, TokenType value) {
    unsigned int index = hash_function(key);
    HashEntry* entry = (HashEntry*)malloc(sizeof(HashEntry)); 
    entry->key = strdup(key);
    entry->value = value;
    entry->next = NULL;

    if (map->entries[index] == NULL) {
        map->entries[index] = entry;
    } else {
        entry->next = map->entries[index];
        map->entries[index] = entry;
    }
}

TokenType hash_get(HashMap* map, const char* key, int* found) {
    unsigned int index = hash_function(key);
    HashEntry* entry = map->entries[index];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            *found = 1;
            return entry->value;
        }

        entry = entry->next;
    }

    *found = 0;
    return END_OF_FILE;
}

void free_hash_map(HashMap* map) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashEntry* entry = map->entries[i];
        while (entry) {
            HashEntry* tmp = entry;
            entry = entry->next;
            free(tmp->key);
            free(tmp);
        }
    }
    free(map);
}

SymbolTable* create_symbol_table() {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    memset(table->table, 0, sizeof(table->table));
    return table;
}

void symbol_insert(SymbolTable* table, const char* type_name) {
    unsigned int index = hash_function(type_name);
    TypeEntry* new_entry = (TypeEntry*)malloc(sizeof(TypeEntry));
    if (!new_entry) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    new_entry->name = strdup(type_name);
    new_entry->next = table->table[index];
    table->table[index] = new_entry;
}

int symbol_lookup(SymbolTable* table, const char* type_name) {
    unsigned int index = hash_function(type_name);
    TypeEntry* entry = table->table[index];

    while (entry) {
        if (strcmp(entry->name, type_name) == 0) {
            return 1;
        }
        entry = entry->next;
    }

    return 0;
}

void free_symbol_table(SymbolTable* table) {
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        TypeEntry* entry = table->table[i];
        while (entry) {
            TypeEntry* tmp = entry;
            entry = entry->next;
            free(tmp->name);
            free(tmp);
        }
    }
    free(table);
}

TokenArray create_token_array(int capacity) {
    TokenArray array;
    array.elements = (Token*)malloc(sizeof(Token) * capacity);
    array.capacity = capacity;
    array.size = 0;

    return array;
}

void token_array_add(TokenArray* array, Token* token) {
    if (!array || !token) {
        fprintf(stderr, "FATAL ERROR: Invalid array or token passed to token_array_add.\n");
        return;
    }
    if (array->size >= array->capacity) {
        // Double the capacity if the array is full
        array->capacity *= 2;
        array->elements = realloc(array->elements, array->capacity * sizeof(Token));
        if (!array->elements) {
            fprintf(stderr, "FATAL ERROR: Failed to resize token array.\n");
            exit(1);
        }
    }
    array->elements[array->size++] = *token;
}

Token* token_array_get(TokenArray* array, int index) {
    if (!array || index < 0 || index >= array->size) {
        fprintf(stderr, "FATAL ERROR: Invalid array or index passed to token_array_get.\n");
        return NULL;
    }

    return &array->elements[index];
}

void free_token_array(TokenArray* array) {
    for (int i = 0; i < array->size; i++) {
        free(&array->elements[i]);
    }
    free(array);
}

HashMap* map_keywords() {
    HashMap* map = create_hash_map();
    hash_insert(map, "if", IF);
    hash_insert(map, "move", MOVE);
    hash_insert(map, "else", ELSE);
    hash_insert(map, "try", TRY);
    hash_insert(map, "while", WHILE);
    hash_insert(map, "for", FOR);
    hash_insert(map, "break", BREAK);
    hash_insert(map, "continue", CONTINUE);
    hash_insert(map, "switch", SWITCH);
    hash_insert(map, "case", CASE);
    hash_insert(map, "begin", BEGIN);
    hash_insert(map, "end", END);
    hash_insert(map, "space", SPACE);
    hash_insert(map, "static", STATIC);
    hash_insert(map, "struct", STRUCT);
    hash_insert(map, "union", UNION);
    hash_insert(map, "enum", ENUM);
    hash_insert(map, "tagged", TAGGED);
    hash_insert(map, "const", CONST);
    hash_insert(map, "use", USE);
    hash_insert(map, "def", DEF);
    hash_insert(map, "new", NEW);
    hash_insert(map, "return", RETURN);
    hash_insert(map, "foreach", FOREACH);
    hash_insert(map, "in", IN);
    hash_insert(map, "default", DEFAULT);
    hash_insert(map, "extern", EXTERN);
    hash_insert(map, "macro", MACRO);
    hash_insert(map, "final", FINAL);
    hash_insert(map, "import", IMPORT);
    hash_insert(map, "nameof", NAMEOF);
    hash_insert(map, "sizeof", SIZEOF);
    hash_insert(map, "typeof", TYPEOF);
    hash_insert(map, "fall", FALL);
    hash_insert(map, "varargs", VARARGS);
    hash_insert(map, "vararg", VARARG);
    hash_insert(map, "finally", FINALLY);
    hash_insert(map, "expand", EXPAND);
    hash_insert(map, "and", AND);
    hash_insert(map, "or", OR);
    hash_insert(map, "unique", UNIQUE);
    hash_insert(map, "shared", SHARED);

    return map;
}
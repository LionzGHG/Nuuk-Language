
#ifndef NUUK_UTILS_H
#define NUUK_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASH_TABLE_SIZE 40

#define MAP(...) ({ \
    HashMap map = { .size = 0 }; \
    HashEntry temp[] = {  __VA_ARGS__ }; \
    for (int i = 0; i < sizeof(temp) / sizeof(temp[0]); i++) { \
        map.entries[i] = temp[i]; \
        map.size++; \
    } \
    map; \
})

typedef enum TokenType {
    // Symbols
    LPAREN, RPAREN, LBRACE, RBRACE, LSQUARE, RSQUARE, COMMA, DOT, SEMICOLON,
    PLUS, MINUS, STAR, SLASH, MOD, EQ, NEQ, LT, GT, LTE, GTE, AND, OR, BANG,
    FALSE, TRUE,
    COLON, COLON_COLON, ASSIGN, MINUS_EQ, PLUS_EQ, STAR_EQ, SLASH_EQ, 
    AMPERSAND, AT, 

    // Objects
    IDENTIFIER, STRING, NUMBER, CHAR,

    // Keywords
    IF, MOVE, ELSE, TRY, WHILE, FOR, BREAK, CONTINUE, SWITCH, CASE, BEGIN, END, SPACE, STATIC, STRUCT, ENUM, UNION, TAGGED,
    CONST, USE, DEF, NEW, RETURN, FOREACH, IN, DEFAULT, EXTERN, MACRO, FINAL, IMPORT,
    NAMEOF, SIZEOF, TYPEOF, FALL, VARARGS, VARARG, FINALLY, EXPAND, UNIQUE, SHARED,

    END_OF_FILE
} TokenType;

typedef struct HashEntry {
    char* key;
    TokenType value;
} HashEntry;

typedef struct HashMap {
    HashEntry* entries[HASH_TABLE_SIZE];
} HashMap;

typedef struct Token {
    TokenType type;
    const char* value;
    size_t ln;
    size_t col;
} Token;

typedef struct TokenArray {
    Token* elements;
    int capacity;
    int size; 
} TokenArray;

Token* create_token(TokenType type, const char* value, size_t ln, size_t col);

char* location(Token* token);

char* concat(const char* source, size_t start, size_t end);

TokenArray create_token_array(int capacity);
void token_array_add(TokenArray* array, Token* token);
void free_token_array(TokenArray* array);
Token* token_array_get(TokenArray* array, int index);

unsigned int hash_function(const char* key);
HashMap* create_hash_map();
void hash_insert(HashMap* map, const char* key, TokenType value);
TokenType hash_get(HashMap* map, const char* key, int* found);
void free_hash_map(HashMap* map);

HashMap* map_keywords();

#endif
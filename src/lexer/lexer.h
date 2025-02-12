#ifndef NUUK_LEXER_H
#define NUUK_LEXER_H

#include "E:\THE_LANGUAGE\src\utils\utils.h"
#include <stdbool.h>

typedef struct Lexer {
    const char* source;
    TokenArray tokens;
    size_t length;
    size_t start;
    size_t current;
    size_t ln;
    size_t col;
    HashMap* keywords;
} Lexer;

Lexer* create_lexer(const char* source);
char lexer_eof(Lexer* self);
char lexer_current(Lexer* self);
char lexer_next(Lexer* self);
char lexer_peek(Lexer* self, size_t offset);
void lexer_add(Lexer* self, TokenType id, const char* value);
void lexer_add_db(Lexer* self, TokenType id, const char* value);
TokenArray* tokenize(Lexer* self);
void lexer_scan_token(Lexer* self);
void lexer_scan_string(Lexer* self);
void lexer_scan_number(Lexer* self);
void lexer_scan_char(Lexer* self);
void lexer_scan_ident(Lexer* self);

bool is_numeric(char c);
bool is_alpha(char c);
bool is_alphanumeric(char c);

#endif

#include "lexer.h"

Lexer* create_lexer(const char* source) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->source = strdup(source);
    lexer->tokens = create_token_array(2);
    lexer->start = 0;
    lexer->current = 0;
    lexer->length = strlen(source);
    lexer->ln = 1;
    lexer->col = 0;
    lexer->keywords = map_keywords();

    return lexer;
}

char lexer_eof(Lexer* self) {
    return (self->current >= self->length);
}

char lexer_current(Lexer* self) {
    return self->source[self->current];
}

char lexer_next(Lexer* self) {
    if (lexer_eof(self)) {
        fprintf(stderr, "[%zu:%zu] ERROR: attempted to read past EOF.\n", self->ln, self->col);
        return '\0';
    }
    char c = lexer_current(self);
    self->current++;
    self->col++;
    return c;
}

char lexer_peek(Lexer* self, size_t offset) {
    if (self->current + offset >= self->length) return '\0';
    return self->source[self->current + offset];
}

void lexer_add(Lexer* self, TokenType id, const char* value) {
    token_array_add(&self->tokens, create_token(id, value, self->ln, self->col));
    lexer_next(self);
}

void lexer_add_db(Lexer* self, TokenType id, const char* value) {
    token_array_add(&self->tokens, create_token(id, value, self->ln, self->col));
    lexer_next(self);
    lexer_next(self);
}

TokenArray* tokenize(Lexer* self) {
    while (!lexer_eof(self)) {
        self->start = self->current;
        lexer_scan_token(self);
    }

    token_array_add(&self->tokens, create_token(END_OF_FILE, "END_OF_FILE", self->ln, self->col));
    return &self->tokens;
}

void lexer_scan_token(Lexer* self) {
    char current = lexer_current(self);
    printf("scanning current: %c\n", current);

    switch (current) {
        case '(': lexer_add(self, LPAREN, "("); break;
        case ')': lexer_add(self, RPAREN, ")"); break;
        case '{': lexer_add(self, LBRACE, "{"); break;
        case '}': lexer_add(self, RBRACE, "}"); break;
        case '[': lexer_add(self, LSQUARE, "["); break;
        case ']': lexer_add(self, RSQUARE, "]"); break;
        case ',': lexer_add(self, COMMA, ","); break;
        case '.': lexer_add(self, DOT, "."); break;
        case ';': lexer_add(self, SEMICOLON, ";"); break;
        case '%': lexer_add(self, MOD, "%"); break;
        case '@': lexer_add(self, AT, "@"); break;
        case '&': lexer_add(self, AMPERSAND, "&"); break;    
        case ':': switch (lexer_peek(self, 1) == ':') {
            case true: lexer_add_db(self, COLON_COLON, "::"); break;
            case false: lexer_add(self, COLON, ":"); break;
        }; break;
        case '-': switch (lexer_peek(self, 1) == '=') {
            case true: lexer_add_db(self, MINUS_EQ, "-="); break;
            case false: lexer_add(self, MINUS, "-"); break;
        }; break;
        case '+': switch (lexer_peek(self, 1) == '=') {
            case true: lexer_add_db(self, PLUS_EQ, "+="); break;
            case false: lexer_add(self, PLUS, "+"); break;
        }; break;
        case '*': switch (lexer_peek(self, 1) == '=') {
            case true: lexer_add_db(self, STAR_EQ, "*="); break;
            case false: lexer_add(self, STAR, "*"); break;
        }; break;
        case '/': switch (lexer_peek(self, 1) == '=') {
            case true: lexer_add_db(self, SLASH_EQ, "/="); break;
            case false: lexer_add(self, SLASH, "/"); break;
        }; break;
        case '!': switch (lexer_peek(self, 1) == '=') {
            case true: lexer_add_db(self, NEQ, "!="); break;
            case false: lexer_add(self, BANG, "!"); break;
        } break;
        case '=': switch (lexer_peek(self, 1) == '=') {
            case true: lexer_add_db(self, EQ, "=="); break;
            case false: lexer_add(self, ASSIGN, "="); break;
        } break;
        case '<': switch (lexer_peek(self, 1) == '=') {
            case true: lexer_add_db(self, LTE, "<="); break;
            case false: lexer_add(self, LT, "<"); break;
        }; break;
        case '>': switch (lexer_peek(self, 1) == '=') {
            case true: lexer_add_db(self, GTE, ">="); break;
            case false: lexer_add(self, GT, ">"); break;
        }; break;
        case '"': lexer_scan_string(self); break;
        case '\'': lexer_scan_char(self); break;
        default:
            if (is_numeric(current)) lexer_scan_number(self);
            else if (is_alpha(current)) lexer_scan_ident(self);
            else lexer_next(self);
    }
}

void lexer_scan_string(Lexer* self) {
    lexer_next(self);
    while (lexer_current(self) != '"' && !lexer_eof(self) && lexer_current(self) != '\n') {
        lexer_next(self);
    }

    if (lexer_eof(self)) {
        fprintf(stderr, "[%zu:%zu] ERROR: unterminated string.\n", self->ln, self->col);
        exit(EXIT_FAILURE);
    }

    lexer_next(self);

    char* value = concat(self->source, self->start, self->current);
    if (!value) {
        fprintf(stderr, "[%zu:%zu] ERROR: failed to allocate string.\n", self->ln, self->col);
        exit(EXIT_FAILURE);
    }

    lexer_add(self, STRING, value);
}

void lexer_scan_number(Lexer* self) {
    while (is_numeric(lexer_peek(self, 1))) lexer_next(self);
    lexer_next(self);
    
    char* value = concat(self->source, self->start, self->current);
    if (!value) {
        fprintf(stderr, "[%zu:%zu] ERROR: failed to allocate number.\n", self->ln, self->col);
        exit(EXIT_FAILURE);
    }

    lexer_add(self, NUMBER, value);

    if (!is_numeric(lexer_current(self))) self->current--;
}

void lexer_scan_char(Lexer* self) {
    lexer_next(self);
    char c = lexer_current(self);

    char* str = (char*)malloc(2);
    if (!str) {
        fprintf(stderr, "[%zu:%zu] ERROR: failed to allocate memory for char.\n", self->ln, self->col);
        exit(EXIT_FAILURE);
    }

    str[0] = c;
    str[1] = '\0';

    lexer_add(self, CHAR, str);
    free(str);
}

void lexer_scan_ident(Lexer* self) {
    while (is_alphanumeric(lexer_peek(self, 1)) && !lexer_eof(self)) lexer_next(self);
    lexer_next(self);

    size_t length = self->current - self->start;
    if (length == 0) {
        fprintf(stderr, "[%zu:%zu] ERROR: Empty identifier!\n", self->ln, self->col);
        return;
    }

    char* value = concat(self->source, self->start, self->current);
    if (!value) {
        fprintf(stderr, "[%zu:%zu] ERROR: concat() returned NULL!\n", self->ln, self->col);
        exit(1);
    }

    int found;
    TokenType keyword = hash_get(self->keywords, value, &found);
    
    if (found) {
        lexer_add(self, keyword, value);
        return;
    }
    lexer_add(self, IDENTIFIER, value);

    if (!is_alphanumeric(lexer_current(self))) self->current--;
}


bool is_numeric(char c) {
    return (c >= '0' && c <= '9') || c == '.';
}

bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool is_alphanumeric(char c) {
    return is_alpha(c) || is_numeric(c);
}

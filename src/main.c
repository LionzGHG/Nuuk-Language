
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "E:\THE_LANGUAGE\cstrap\lexer\lexer.h"
#include "E:\THE_LANGUAGE\cstrap\parser\parser.h"
#include "E:\THE_LANGUAGE\cstrap\parser\ast_printer.h"

void eval(char* source);
void repl();
int ends_with(const char* str, const char* suffix);
void read_file(const char* file_name);

#define MAX_LENGTH 255

int main(int argc, char** argv) {
    switch (argc) {
        case 1:
            repl();
            break;
        case 2:
            read_file(argv[1]);
            break;
        default:
            fprintf(stderr, "Usage: ion run [path]\n");
            return 1;
    }

    return 0;
}

void eval(char* source) {
    Lexer* lexer = create_lexer(source);
    TokenArray* tokens = tokenize(lexer);

    for (int i = 0; i < tokens->size; i++) {
        printf("%s\n", tokens->elements[i].value);
    }

    Parser* parser = create_parser(tokens);
    StmtArray stmts = parse(parser);

    for (int i = 0; i < stmts.size; i++) {
        dprint_stmt(stmts.elements[i]);
    }
    printf("Finished!\n");
}

void repl() {
    for (;;) {
        printf("> ");
        char buffer[MAX_LENGTH];     

        if (fgets(buffer, MAX_LENGTH, stdin)) {
            if (strcmp(buffer, "exit") == 0) break;
            eval(buffer);
        } 
        else {
            fprintf(stderr, "Failed to read line.");
            exit(EXIT_FAILURE);
        }
    }
}

int ends_with(const char* str, const char* suffix) {
    if (strlen(str) < strlen(suffix)) return 0;
    return strncmp(str + strlen(str) - strlen(suffix), suffix, strlen(suffix)) == 0;
}

void read_file(const char* file_name) {
    if (!ends_with(file_name, ".tx")) {
        fprintf(stderr, "File %s has Incorrect extension.\n", file_name);
        exit(1);
    }

    FILE* file = fopen(file_name, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file %s\n", file_name);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        fprintf(stderr, "Failed to allocate memory for file %s\n", file_name);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);
    eval(buffer);
}
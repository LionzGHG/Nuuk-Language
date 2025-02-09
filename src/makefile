# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -std=c11 -g
# Output executable name
TARGET = nuuk
# Source files
#SRCS = $(wildcard *.c)
SRCS = main.c lexer/lexer.c utils/utils.c parser/ast.c parser/parser.c parser/ast_printer.c
# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile C files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

#include "compiler/lexer.h"
#include "compiler/parser.h"
#include <stdio.h>
#include <stdlib.h>

// Function to print an AST node (for debugging)
void print_ast_node(ASTNode* node, int depth) {
    if (!node) return;

    // Print indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // Print node type and value
    printf("Type: %d", node->type);
    if (node->value) {
        printf(", Value: %s", node->value);
    }
    printf("\n");

    // Print children
    for (int i = 0; i < node->children_count; i++) {
        print_ast_node(node->children[i], depth + 1);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    // Read the source file
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer and read file
    char* source = (char*)malloc(file_size + 1);
    if (!source) {
        fclose(file);
        perror("Error allocating memory");
        return 1;
    }

    fread(source, 1, file_size, file);
    source[file_size] = '\0';
    fclose(file);

    // Create lexer and parser
    Lexer* lexer = create_lexer(source);
    Parser* parser = create_parser(lexer);

    // Parse the program
    ASTNode* ast = parse_program(parser);

    // Print the AST
    printf("Abstract Syntax Tree:\n");
    print_ast_node(ast, 0);

    // Clean up
    destroy_ast_node(ast);
    destroy_parser(parser);
    destroy_lexer(lexer);
    free(source);

    return 0;
} 
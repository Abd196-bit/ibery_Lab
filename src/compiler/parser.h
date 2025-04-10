#ifndef IBERY_PARSER_H
#define IBERY_PARSER_H

#include "lexer.h"
#include <stdbool.h>

// AST Node Types
typedef enum {
    NODE_PROGRAM,
    NODE_FUNCTION_DEF,
    NODE_FUNCTION_CALL,
    NODE_RUN_STATEMENT,
    NODE_PRINT_STATEMENT,
    NODE_RETURN_STATEMENT,
    NODE_NUMBER_LITERAL,
    NODE_STRING_LITERAL,
    NODE_IDENTIFIER,
    NODE_PARAMETERS,
    NODE_BODY
} NodeType;

// AST Node structure
typedef struct ASTNode {
    NodeType type;
    char* value;
    struct ASTNode** children;
    int children_count;
    Token* token;
} ASTNode;

// Parser structure
typedef struct {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} Parser;

// Function declarations
Parser* create_parser(Lexer* lexer);
void destroy_parser(Parser* parser);
ASTNode* parse_program(Parser* parser);
void destroy_ast_node(ASTNode* node);

// Helper functions
void advance_tokens(Parser* parser);
void expect_token(Parser* parser, TokenType expected_type);
ASTNode* parse_function_definition(Parser* parser);
ASTNode* parse_statement(Parser* parser);
ASTNode* parse_expression(Parser* parser);
ASTNode* parse_function_call(Parser* parser, char* name);
ASTNode* parse_run_statement(Parser* parser);
ASTNode* parse_print_statement(Parser* parser);
ASTNode* parse_return_statement(Parser* parser);

#endif // IBERY_PARSER_H 
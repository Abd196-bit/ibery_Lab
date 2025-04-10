#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Create a new parser
Parser* create_parser(Lexer* lexer) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }

    parser->lexer = lexer;
    parser->current_token = get_next_token(lexer);
    parser->peek_token = get_next_token(lexer);
    return parser;
}

// Destroy a parser
void destroy_parser(Parser* parser) {
    if (parser) {
        destroy_token(parser->current_token);
        destroy_token(parser->peek_token);
        free(parser);
    }
}

// Create a new AST node
ASTNode* create_ast_node(NodeType type, char* value, Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        return NULL;
    }

    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->children = NULL;
    node->children_count = 0;
    node->token = token;
    return node;
}

// Add a child to an AST node
void add_child(ASTNode* parent, ASTNode* child) {
    parent->children_count++;
    parent->children = (ASTNode**)realloc(parent->children, 
                                        parent->children_count * sizeof(ASTNode*));
    parent->children[parent->children_count - 1] = child;
}

// Destroy an AST node and its children
void destroy_ast_node(ASTNode* node) {
    if (!node) return;

    if (node->value) {
        free(node->value);
    }

    for (int i = 0; i < node->children_count; i++) {
        destroy_ast_node(node->children[i]);
    }

    if (node->children) {
        free(node->children);
    }

    free(node);
}

// Advance to the next token
void advance_tokens(Parser* parser) {
    destroy_token(parser->current_token);
    parser->current_token = parser->peek_token;
    parser->peek_token = get_next_token(parser->lexer);
}

// Expect a specific token type
void expect_token(Parser* parser, TokenType expected_type) {
    if (parser->current_token->type == expected_type) {
        advance_tokens(parser);
    } else {
        fprintf(stderr, "Expected token type %d, got %d at line %d, column %d\n",
                expected_type, parser->current_token->type,
                parser->current_token->line, parser->current_token->column);
        exit(1);
    }
}

// Parse a function definition
ASTNode* parse_function_definition(Parser* parser) {
    expect_token(parser, TOKEN_DEF);
    
    char* name = strdup(parser->current_token->value);
    expect_token(parser, TOKEN_IDENTIFIER);
    expect_token(parser, TOKEN_LPAREN);

    ASTNode* params_node = create_ast_node(NODE_PARAMETERS, NULL, NULL);
    while (parser->current_token->type != TOKEN_RPAREN) {
        ASTNode* param = create_ast_node(NODE_IDENTIFIER, 
                                       parser->current_token->value, 
                                       parser->current_token);
        add_child(params_node, param);
        expect_token(parser, TOKEN_IDENTIFIER);
        
        if (parser->current_token->type == TOKEN_COMMA) {
            expect_token(parser, TOKEN_COMMA);
        }
    }
    expect_token(parser, TOKEN_RPAREN);
    expect_token(parser, TOKEN_COLON);

    ASTNode* body_node = create_ast_node(NODE_BODY, NULL, NULL);
    while (parser->current_token->type != TOKEN_EOF) {
        ASTNode* statement = parse_statement(parser);
        add_child(body_node, statement);
    }

    ASTNode* func_def = create_ast_node(NODE_FUNCTION_DEF, name, NULL);
    add_child(func_def, params_node);
    add_child(func_def, body_node);
    return func_def;
}

// Parse a statement
ASTNode* parse_statement(Parser* parser) {
    if (parser->current_token->type == TOKEN_RUN) {
        return parse_run_statement(parser);
    } else if (parser->current_token->type == TOKEN_PRINT) {
        return parse_print_statement(parser);
    } else if (parser->current_token->type == TOKEN_RETURN) {
        return parse_return_statement(parser);
    } else {
        return parse_expression(parser);
    }
}

// Parse a run statement
ASTNode* parse_run_statement(Parser* parser) {
    expect_token(parser, TOKEN_RUN);
    bool is_quantum = parser->current_token->type == TOKEN_QUANTUM;
    if (is_quantum) {
        expect_token(parser, TOKEN_QUANTUM);
    }
    char* command = strdup(parser->current_token->value);
    expect_token(parser, TOKEN_STRING);

    ASTNode* run_node = create_ast_node(NODE_RUN_STATEMENT, command, NULL);
    if (is_quantum) {
        ASTNode* quantum_node = create_ast_node(NODE_IDENTIFIER, "quantum", NULL);
        add_child(run_node, quantum_node);
    }
    return run_node;
}

// Parse a print statement
ASTNode* parse_print_statement(Parser* parser) {
    expect_token(parser, TOKEN_PRINT);
    ASTNode* value = parse_expression(parser);
    ASTNode* print_node = create_ast_node(NODE_PRINT_STATEMENT, NULL, NULL);
    add_child(print_node, value);
    return print_node;
}

// Parse a return statement
ASTNode* parse_return_statement(Parser* parser) {
    expect_token(parser, TOKEN_RETURN);
    ASTNode* value = parse_expression(parser);
    ASTNode* return_node = create_ast_node(NODE_RETURN_STATEMENT, NULL, NULL);
    add_child(return_node, value);
    return return_node;
}

// Parse an expression
ASTNode* parse_expression(Parser* parser) {
    if (parser->current_token->type == TOKEN_NUMBER) {
        char* value = strdup(parser->current_token->value);
        expect_token(parser, TOKEN_NUMBER);
        return create_ast_node(NODE_NUMBER_LITERAL, value, NULL);
    } else if (parser->current_token->type == TOKEN_STRING) {
        char* value = strdup(parser->current_token->value);
        expect_token(parser, TOKEN_STRING);
        return create_ast_node(NODE_STRING_LITERAL, value, NULL);
    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        char* name = strdup(parser->current_token->value);
        expect_token(parser, TOKEN_IDENTIFIER);
        if (parser->current_token->type == TOKEN_LPAREN) {
            return parse_function_call(parser, name);
        } else {
            return create_ast_node(NODE_IDENTIFIER, name, NULL);
        }
    } else {
        fprintf(stderr, "Unexpected token type: %d\n", parser->current_token->type);
        exit(1);
    }
}

// Parse a function call
ASTNode* parse_function_call(Parser* parser, char* name) {
    expect_token(parser, TOKEN_LPAREN);
    ASTNode* args_node = create_ast_node(NODE_PARAMETERS, NULL, NULL);
    
    while (parser->current_token->type != TOKEN_RPAREN) {
        ASTNode* arg = parse_expression(parser);
        add_child(args_node, arg);
        if (parser->current_token->type == TOKEN_COMMA) {
            expect_token(parser, TOKEN_COMMA);
        }
    }
    expect_token(parser, TOKEN_RPAREN);

    ASTNode* call_node = create_ast_node(NODE_FUNCTION_CALL, name, NULL);
    add_child(call_node, args_node);
    return call_node;
}

// Parse the entire program
ASTNode* parse_program(Parser* parser) {
    ASTNode* program = create_ast_node(NODE_PROGRAM, NULL, NULL);
    
    while (parser->current_token->type != TOKEN_EOF) {
        if (parser->current_token->type == TOKEN_DEF) {
            ASTNode* func_def = parse_function_definition(parser);
            add_child(program, func_def);
        } else {
            ASTNode* statement = parse_statement(parser);
            add_child(program, statement);
        }
    }
    
    return program;
} 
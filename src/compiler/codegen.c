#include "codegen.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define INITIAL_CAPACITY 1024

// Create a new code generator
CodeGenerator* create_code_generator(void) {
    CodeGenerator* gen = (CodeGenerator*)malloc(sizeof(CodeGenerator));
    if (!gen) {
        return NULL;
    }

    gen->instructions = (uint8_t*)malloc(INITIAL_CAPACITY);
    if (!gen->instructions) {
        free(gen);
        return NULL;
    }

    gen->capacity = INITIAL_CAPACITY;
    gen->size = 0;
    gen->label_counter = 0;
    return gen;
}

// Destroy a code generator
void destroy_code_generator(CodeGenerator* gen) {
    if (gen) {
        free(gen->instructions);
        free(gen);
    }
}

// Ensure we have enough capacity for new instructions
static void ensure_capacity(CodeGenerator* gen, size_t needed) {
    if (gen->size + needed > gen->capacity) {
        size_t new_capacity = gen->capacity * 2;
        while (gen->size + needed > new_capacity) {
            new_capacity *= 2;
        }
        uint8_t* new_instructions = (uint8_t*)realloc(gen->instructions, new_capacity);
        if (!new_instructions) {
            fprintf(stderr, "Failed to allocate memory for code generation\n");
            exit(1);
        }
        gen->instructions = new_instructions;
        gen->capacity = new_capacity;
    }
}

// Emit a single instruction with variable arguments
void emit_instruction(CodeGenerator* gen, uint8_t opcode, ...) {
    ensure_capacity(gen, 1);
    gen->instructions[gen->size++] = opcode;

    va_list args;
    va_start(args, opcode);
    
    switch (opcode) {
        case OP_PUSH_NAME:
        case OP_RUN_COMMAND:
        case OP_CALL_FUNCTION:
        case OP_PUSH_STRING:
        case OP_PUSH_IDENTIFIER: {
            const char* str = va_arg(args, const char*);
            size_t len = strlen(str);
            ensure_capacity(gen, len + 1);
            gen->instructions[gen->size++] = (uint8_t)len;
            memcpy(gen->instructions + gen->size, str, len);
            gen->size += len;
            break;
        }
        case OP_PUSH_NUMBER:
        case OP_FUNCTION_DEF: {
            int num = va_arg(args, int);
            ensure_capacity(gen, sizeof(int));
            memcpy(gen->instructions + gen->size, &num, sizeof(int));
            gen->size += sizeof(int);
            break;
        }
    }
    
    va_end(args);
}

// Generate code from an AST node
static void generate_node(CodeGenerator* gen, ASTNode* node) {
    switch (node->type) {
        case NODE_FUNCTION_DEF: {
            // Function definition
            ASTNode* name_node = node->children[0];
            ASTNode* params_node = node->children[1];
            ASTNode* body_node = node->children[2];
            
            emit_instruction(gen, OP_FUNCTION_DEF, params_node->children_count);
            emit_instruction(gen, OP_PUSH_NAME, name_node->value);
            
            // Generate body
            for (int i = 0; i < body_node->children_count; i++) {
                generate_node(gen, body_node->children[i]);
            }
            
            emit_instruction(gen, OP_RETURN);
            break;
        }
        
        case NODE_RUN_STATEMENT: {
            // Run statement
            if (node->children_count > 0) {
                emit_instruction(gen, OP_QUANTUM_OP);
            }
            emit_instruction(gen, OP_RUN_COMMAND, node->value);
            break;
        }
        
        case NODE_PRINT_STATEMENT: {
            // Print statement
            generate_node(gen, node->children[0]);
            emit_instruction(gen, OP_PRINT);
            break;
        }
        
        case NODE_RETURN_STATEMENT: {
            // Return statement
            generate_node(gen, node->children[0]);
            emit_instruction(gen, OP_RETURN);
            break;
        }
        
        case NODE_FUNCTION_CALL: {
            // Function call
            ASTNode* args_node = node->children[0];
            
            // Push arguments
            for (int i = 0; i < args_node->children_count; i++) {
                generate_node(gen, args_node->children[i]);
            }
            
            emit_instruction(gen, OP_CALL_FUNCTION, node->value);
            break;
        }
        
        case NODE_NUMBER_LITERAL: {
            // Number literal
            int num = atoi(node->value);
            emit_instruction(gen, OP_PUSH_NUMBER, num);
            break;
        }
        
        case NODE_STRING_LITERAL: {
            // String literal
            emit_instruction(gen, OP_PUSH_STRING, node->value);
            break;
        }
        
        case NODE_IDENTIFIER: {
            // Identifier
            emit_instruction(gen, OP_PUSH_IDENTIFIER, node->value);
            break;
        }
        
        case NODE_PROGRAM: {
            // Program
            for (int i = 0; i < node->children_count; i++) {
                generate_node(gen, node->children[i]);
            }
            break;
        }
        
        default:
            fprintf(stderr, "Unknown node type: %d\n", node->type);
            exit(1);
    }
}

// Generate code from an AST
uint8_t* generate_code(CodeGenerator* gen, ASTNode* ast, size_t* output_size) {
    generate_node(gen, ast);
    *output_size = gen->size;
    return gen->instructions;
} 
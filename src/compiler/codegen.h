#ifndef IBERY_CODEGEN_H
#define IBERY_CODEGEN_H

#include "parser.h"
#include <stdint.h>

// Opcodes for the binary format
typedef enum {
    OP_FUNCTION_DEF = 0x01,
    OP_PUSH_NAME = 0x02,
    OP_RETURN = 0x03,
    OP_QUANTUM_OP = 0x04,
    OP_RUN_COMMAND = 0x05,
    OP_PRINT = 0x06,
    OP_CALL_FUNCTION = 0x07,
    OP_PUSH_NUMBER = 0x08,
    OP_PUSH_STRING = 0x09,
    OP_PUSH_IDENTIFIER = 0x0A
} Opcode;

// Code generator structure
typedef struct {
    uint8_t* instructions;
    size_t capacity;
    size_t size;
    int label_counter;
} CodeGenerator;

// Function declarations
CodeGenerator* create_code_generator(void);
void destroy_code_generator(CodeGenerator* gen);
uint8_t* generate_code(CodeGenerator* gen, ASTNode* ast, size_t* output_size);
void emit_instruction(CodeGenerator* gen, uint8_t opcode, ...);
void emit_string(CodeGenerator* gen, const char* str);
void emit_number(CodeGenerator* gen, int number);

#endif // IBERY_CODEGEN_H 
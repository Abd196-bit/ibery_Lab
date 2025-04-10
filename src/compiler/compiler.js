"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.IberyCompiler = void 0;
const lexer_1 = require("./lexer");
const parser_1 = require("./parser");
const codegen_1 = require("./codegen");
class IberyCompiler {
    constructor() {
        this.lexer = new lexer_1.Lexer('');
        this.parser = new parser_1.Parser(this.lexer);
        this.codeGenerator = new codegen_1.CodeGenerator();
    }
    compile(source) {
        // Initialize lexer with new source
        this.lexer = new lexer_1.Lexer(source);
        this.parser = new parser_1.Parser(this.lexer);
        // Parse the source code into an AST
        const ast = this.parser.parse();
        // Generate binary code from the AST
        return this.codeGenerator.generate(ast);
    }
    disassemble(binary) {
        let output = '';
        let i = 0;
        while (i < binary.length) {
            const opcode = binary[i++];
            let instruction = '';
            switch (opcode) {
                case 0x01: // FUNCTION_DEF
                    const params = binary[i++];
                    instruction = `FUNCTION_DEF ${params}`;
                    break;
                case 0x02: // PUSH_NAME
                    const nameLength = binary[i++];
                    const name = String.fromCharCode(...binary.slice(i, i + nameLength));
                    i += nameLength;
                    instruction = `PUSH_NAME "${name}"`;
                    break;
                case 0x03: // RETURN
                    instruction = 'RETURN';
                    break;
                case 0x04: // QUANTUM_OP
                    instruction = 'QUANTUM_OP';
                    break;
                case 0x05: // RUN_COMMAND
                    const cmdLength = binary[i++];
                    const command = String.fromCharCode(...binary.slice(i, i + cmdLength));
                    i += cmdLength;
                    instruction = `RUN_COMMAND "${command}"`;
                    break;
                case 0x06: // PRINT
                    instruction = 'PRINT';
                    break;
                case 0x07: // CALL_FUNCTION
                    const funcNameLength = binary[i++];
                    const funcName = String.fromCharCode(...binary.slice(i, i + funcNameLength));
                    i += funcNameLength;
                    instruction = `CALL_FUNCTION "${funcName}"`;
                    break;
                case 0x08: // PUSH_NUMBER
                    const number = binary[i++];
                    instruction = `PUSH_NUMBER ${number}`;
                    break;
                case 0x09: // PUSH_STRING
                    const strLength = binary[i++];
                    const str = String.fromCharCode(...binary.slice(i, i + strLength));
                    i += strLength;
                    instruction = `PUSH_STRING "${str}"`;
                    break;
                case 0x0A: // PUSH_IDENTIFIER
                    const idLength = binary[i++];
                    const identifier = String.fromCharCode(...binary.slice(i, i + idLength));
                    i += idLength;
                    instruction = `PUSH_IDENTIFIER "${identifier}"`;
                    break;
                default:
                    instruction = `UNKNOWN_OPCODE ${opcode}`;
            }
            output += `${i.toString(16).padStart(4, '0')}: ${instruction}\n`;
        }
        return output;
    }
}
exports.IberyCompiler = IberyCompiler;

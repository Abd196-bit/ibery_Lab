import { ASTNode } from './types';

export class CodeGenerator {
    private instructions: number[] = [];
    private labelCounter: number = 0;

    private generateLabel(): string {
        return `L${this.labelCounter++}`;
    }

    private emit(opcode: number, ...operands: number[]): void {
        this.instructions.push(opcode);
        this.instructions.push(...operands);
    }

    private generateFunctionDefinition(node: ASTNode): void {
        const name = node.children![0].value;
        const params = node.children![1].children!.length;
        const body = node.children![2];

        // Function prologue
        this.emit(0x01, params); // FUNCTION_DEF
        this.emit(0x02, name.length); // PUSH_NAME
        this.emit(...Array.from(name).map(c => c.charCodeAt(0)));

        // Generate body
        this.generateNode(body);

        // Function epilogue
        this.emit(0x03); // RETURN
    }

    private generateRunStatement(node: ASTNode): void {
        const isQuantum = node.children![0].value;
        const command = node.children![1].value;

        if (isQuantum) {
            this.emit(0x04); // QUANTUM_OP
        }
        this.emit(0x05, command.length); // RUN_COMMAND
        this.emit(...Array.from(command).map(c => c.charCodeAt(0)));
    }

    private generatePrintStatement(node: ASTNode): void {
        this.generateNode(node.children![0]);
        this.emit(0x06); // PRINT
    }

    private generateReturnStatement(node: ASTNode): void {
        this.generateNode(node.children![0]);
        this.emit(0x03); // RETURN
    }

    private generateFunctionCall(node: ASTNode): void {
        const name = node.children![0].value;
        const args = node.children![1].children!;

        // Push arguments
        for (const arg of args) {
            this.generateNode(arg);
        }

        // Call function
        this.emit(0x07, name.length); // CALL_FUNCTION
        this.emit(...Array.from(name).map(c => c.charCodeAt(0)));
    }

    private generateNode(node: ASTNode): void {
        switch (node.type) {
            case 'FunctionDefinition':
                this.generateFunctionDefinition(node);
                break;
            case 'RunStatement':
                this.generateRunStatement(node);
                break;
            case 'PrintStatement':
                this.generatePrintStatement(node);
                break;
            case 'ReturnStatement':
                this.generateReturnStatement(node);
                break;
            case 'FunctionCall':
                this.generateFunctionCall(node);
                break;
            case 'NumberLiteral':
                this.emit(0x08, parseInt(node.value)); // PUSH_NUMBER
                break;
            case 'StringLiteral':
                this.emit(0x09, node.value.length); // PUSH_STRING
                this.emit(...Array.from(node.value).map(c => c.charCodeAt(0)));
                break;
            case 'Identifier':
                this.emit(0x0A, node.value.length); // PUSH_IDENTIFIER
                this.emit(...Array.from(node.value).map(c => c.charCodeAt(0)));
                break;
            case 'Program':
                for (const child of node.children!) {
                    this.generateNode(child);
                }
                break;
            default:
                throw new Error(`Unknown node type: ${node.type}`);
        }
    }

    public generate(ast: ASTNode): Uint8Array {
        this.generateNode(ast);
        return new Uint8Array(this.instructions);
    }
} 
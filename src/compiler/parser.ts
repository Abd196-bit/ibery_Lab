import { Lexer } from './lexer';
import { Token, TokenType, ASTNode } from './types';

export class Parser {
    private lexer: Lexer;
    private currentToken: Token;
    private peekToken: Token;

    constructor(lexer: Lexer) {
        this.lexer = lexer;
        this.currentToken = this.lexer.getNextToken();
        this.peekToken = this.lexer.getNextToken();
    }

    private advanceTokens(): void {
        this.currentToken = this.peekToken;
        this.peekToken = this.lexer.getNextToken();
    }

    private expect(tokenType: TokenType): void {
        if (this.currentToken.type === tokenType) {
            this.advanceTokens();
        } else {
            throw new Error(`Expected ${tokenType}, got ${this.currentToken.type}`);
        }
    }

    private parseFunctionDefinition(): ASTNode {
        this.expect(TokenType.DEF);
        const name = this.currentToken.value;
        this.expect(TokenType.IDENTIFIER);
        this.expect(TokenType.LPAREN);

        const params: string[] = [];
        while (this.currentToken.type !== TokenType.RPAREN) {
            params.push(this.currentToken.value);
            this.expect(TokenType.IDENTIFIER);
            if (this.currentToken.type === TokenType.COMMA) {
                this.expect(TokenType.COMMA);
            }
        }
        this.expect(TokenType.RPAREN);
        this.expect(TokenType.COLON);

        const body: ASTNode[] = [];
        while (this.currentToken.type !== TokenType.EOF) {
            body.push(this.parseStatement());
        }

        return {
            type: 'FunctionDefinition',
            children: [
                { type: 'Identifier', value: name },
                { type: 'Parameters', children: params.map(p => ({ type: 'Identifier', value: p })) },
                { type: 'Body', children: body }
            ]
        };
    }

    private parseStatement(): ASTNode {
        if (this.currentToken.type === TokenType.RUN) {
            return this.parseRunStatement();
        } else if (this.currentToken.type === TokenType.PRINT) {
            return this.parsePrintStatement();
        } else if (this.currentToken.type === TokenType.RETURN) {
            return this.parseReturnStatement();
        } else {
            return this.parseExpression();
        }
    }

    private parseRunStatement(): ASTNode {
        this.expect(TokenType.RUN);
        const isQuantum = this.currentToken.type === TokenType.QUANTUM;
        if (isQuantum) {
            this.expect(TokenType.QUANTUM);
        }
        const command = this.currentToken.value;
        this.expect(TokenType.STRING);
        return {
            type: 'RunStatement',
            children: [
                { type: 'IsQuantum', value: isQuantum },
                { type: 'Command', value: command }
            ]
        };
    }

    private parsePrintStatement(): ASTNode {
        this.expect(TokenType.PRINT);
        const value = this.parseExpression();
        return {
            type: 'PrintStatement',
            children: [value]
        };
    }

    private parseReturnStatement(): ASTNode {
        this.expect(TokenType.RETURN);
        const value = this.parseExpression();
        return {
            type: 'ReturnStatement',
            children: [value]
        };
    }

    private parseExpression(): ASTNode {
        if (this.currentToken.type === TokenType.NUMBER) {
            const value = this.currentToken.value;
            this.expect(TokenType.NUMBER);
            return { type: 'NumberLiteral', value };
        } else if (this.currentToken.type === TokenType.STRING) {
            const value = this.currentToken.value;
            this.expect(TokenType.STRING);
            return { type: 'StringLiteral', value };
        } else if (this.currentToken.type === TokenType.IDENTIFIER) {
            const name = this.currentToken.value;
            this.expect(TokenType.IDENTIFIER);
            if (this.currentToken.type === TokenType.LPAREN) {
                return this.parseFunctionCall(name);
            } else {
                return { type: 'Identifier', value: name };
            }
        } else {
            throw new Error(`Unexpected token: ${this.currentToken.type}`);
        }
    }

    private parseFunctionCall(name: string): ASTNode {
        this.expect(TokenType.LPAREN);
        const args: ASTNode[] = [];
        while (this.currentToken.type !== TokenType.RPAREN) {
            args.push(this.parseExpression());
            if (this.currentToken.type === TokenType.COMMA) {
                this.expect(TokenType.COMMA);
            }
        }
        this.expect(TokenType.RPAREN);
        return {
            type: 'FunctionCall',
            children: [
                { type: 'Identifier', value: name },
                { type: 'Arguments', children: args }
            ]
        };
    }

    public parse(): ASTNode {
        const statements: ASTNode[] = [];
        while (this.currentToken.type !== TokenType.EOF) {
            if (this.currentToken.type === TokenType.DEF) {
                statements.push(this.parseFunctionDefinition());
            } else {
                statements.push(this.parseStatement());
            }
        }
        return {
            type: 'Program',
            children: statements
        };
    }
} 
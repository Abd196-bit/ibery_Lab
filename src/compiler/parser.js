"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Parser = void 0;
const types_1 = require("./types");
class Parser {
    constructor(lexer) {
        this.lexer = lexer;
        this.currentToken = this.lexer.getNextToken();
        this.peekToken = this.lexer.getNextToken();
    }
    advanceTokens() {
        this.currentToken = this.peekToken;
        this.peekToken = this.lexer.getNextToken();
    }
    expect(tokenType) {
        if (this.currentToken.type === tokenType) {
            this.advanceTokens();
        }
        else {
            throw new Error(`Expected ${tokenType}, got ${this.currentToken.type}`);
        }
    }
    parseFunctionDefinition() {
        this.expect(types_1.TokenType.DEF);
        const name = this.currentToken.value;
        this.expect(types_1.TokenType.IDENTIFIER);
        this.expect(types_1.TokenType.LPAREN);
        const params = [];
        while (this.currentToken.type !== types_1.TokenType.RPAREN) {
            params.push(this.currentToken.value);
            this.expect(types_1.TokenType.IDENTIFIER);
            if (this.currentToken.type === types_1.TokenType.COMMA) {
                this.expect(types_1.TokenType.COMMA);
            }
        }
        this.expect(types_1.TokenType.RPAREN);
        this.expect(types_1.TokenType.COLON);
        const body = [];
        while (this.currentToken.type !== types_1.TokenType.EOF) {
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
    parseStatement() {
        if (this.currentToken.type === types_1.TokenType.RUN) {
            return this.parseRunStatement();
        }
        else if (this.currentToken.type === types_1.TokenType.PRINT) {
            return this.parsePrintStatement();
        }
        else if (this.currentToken.type === types_1.TokenType.RETURN) {
            return this.parseReturnStatement();
        }
        else {
            return this.parseExpression();
        }
    }
    parseRunStatement() {
        this.expect(types_1.TokenType.RUN);
        const isQuantum = this.currentToken.type === types_1.TokenType.QUANTUM;
        if (isQuantum) {
            this.expect(types_1.TokenType.QUANTUM);
        }
        const command = this.currentToken.value;
        this.expect(types_1.TokenType.STRING);
        return {
            type: 'RunStatement',
            children: [
                { type: 'IsQuantum', value: isQuantum },
                { type: 'Command', value: command }
            ]
        };
    }
    parsePrintStatement() {
        this.expect(types_1.TokenType.PRINT);
        const value = this.parseExpression();
        return {
            type: 'PrintStatement',
            children: [value]
        };
    }
    parseReturnStatement() {
        this.expect(types_1.TokenType.RETURN);
        const value = this.parseExpression();
        return {
            type: 'ReturnStatement',
            children: [value]
        };
    }
    parseExpression() {
        if (this.currentToken.type === types_1.TokenType.NUMBER) {
            const value = this.currentToken.value;
            this.expect(types_1.TokenType.NUMBER);
            return { type: 'NumberLiteral', value };
        }
        else if (this.currentToken.type === types_1.TokenType.STRING) {
            const value = this.currentToken.value;
            this.expect(types_1.TokenType.STRING);
            return { type: 'StringLiteral', value };
        }
        else if (this.currentToken.type === types_1.TokenType.IDENTIFIER) {
            const name = this.currentToken.value;
            this.expect(types_1.TokenType.IDENTIFIER);
            if (this.currentToken.type === types_1.TokenType.LPAREN) {
                return this.parseFunctionCall(name);
            }
            else {
                return { type: 'Identifier', value: name };
            }
        }
        else {
            throw new Error(`Unexpected token: ${this.currentToken.type}`);
        }
    }
    parseFunctionCall(name) {
        this.expect(types_1.TokenType.LPAREN);
        const args = [];
        while (this.currentToken.type !== types_1.TokenType.RPAREN) {
            args.push(this.parseExpression());
            if (this.currentToken.type === types_1.TokenType.COMMA) {
                this.expect(types_1.TokenType.COMMA);
            }
        }
        this.expect(types_1.TokenType.RPAREN);
        return {
            type: 'FunctionCall',
            children: [
                { type: 'Identifier', value: name },
                { type: 'Arguments', children: args }
            ]
        };
    }
    parse() {
        const statements = [];
        while (this.currentToken.type !== types_1.TokenType.EOF) {
            if (this.currentToken.type === types_1.TokenType.DEF) {
                statements.push(this.parseFunctionDefinition());
            }
            else {
                statements.push(this.parseStatement());
            }
        }
        return {
            type: 'Program',
            children: statements
        };
    }
}
exports.Parser = Parser;

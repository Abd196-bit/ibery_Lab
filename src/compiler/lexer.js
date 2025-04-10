"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Lexer = void 0;
const types_1 = require("./types");
class Lexer {
    constructor(input) {
        this.position = 0;
        this.line = 1;
        this.column = 1;
        this.input = input;
    }
    isWhitespace(char) {
        return /\s/.test(char);
    }
    isDigit(char) {
        return /[0-9]/.test(char);
    }
    isAlpha(char) {
        return /[a-zA-Z_]/.test(char);
    }
    isAlphaNumeric(char) {
        return this.isAlpha(char) || this.isDigit(char);
    }
    peek() {
        return this.input[this.position] || '\0';
    }
    advance() {
        const char = this.peek();
        this.position++;
        if (char === '\n') {
            this.line++;
            this.column = 1;
        }
        else {
            this.column++;
        }
        return char;
    }
    skipWhitespace() {
        while (this.isWhitespace(this.peek())) {
            this.advance();
        }
    }
    readNumber() {
        let number = '';
        while (this.isDigit(this.peek())) {
            number += this.advance();
        }
        return number;
    }
    readIdentifier() {
        let identifier = '';
        while (this.isAlphaNumeric(this.peek())) {
            identifier += this.advance();
        }
        return identifier;
    }
    readString() {
        let str = '';
        this.advance(); // Skip opening quote
        while (this.peek() !== '"' && this.peek() !== '\0') {
            str += this.advance();
        }
        this.advance(); // Skip closing quote
        return str;
    }
    readComment() {
        while (this.peek() !== '\n' && this.peek() !== '\0') {
            this.advance();
        }
    }
    getNextToken() {
        this.skipWhitespace();
        if (this.position >= this.input.length) {
            return {
                type: types_1.TokenType.EOF,
                value: '',
                line: this.line,
                column: this.column
            };
        }
        const char = this.peek();
        // Handle numbers
        if (this.isDigit(char)) {
            return {
                type: types_1.TokenType.NUMBER,
                value: this.readNumber(),
                line: this.line,
                column: this.column
            };
        }
        // Handle identifiers and keywords
        if (this.isAlpha(char)) {
            const identifier = this.readIdentifier();
            const tokenType = this.getKeywordTokenType(identifier);
            return {
                type: tokenType,
                value: identifier,
                line: this.line,
                column: this.column
            };
        }
        // Handle strings
        if (char === '"') {
            return {
                type: types_1.TokenType.STRING,
                value: this.readString(),
                line: this.line,
                column: this.column
            };
        }
        // Handle comments
        if (char === '#') {
            this.readComment();
            return this.getNextToken();
        }
        // Handle operators and punctuation
        const token = this.getOperatorToken(char);
        if (token) {
            this.advance();
            return token;
        }
        throw new Error(`Unexpected character: ${char} at line ${this.line}, column ${this.column}`);
    }
    getKeywordTokenType(identifier) {
        const keywords = {
            'def': types_1.TokenType.DEF,
            'run': types_1.TokenType.RUN,
            'quantum': types_1.TokenType.QUANTUM,
            'print': types_1.TokenType.PRINT,
            'return': types_1.TokenType.RETURN
        };
        return keywords[identifier] || types_1.TokenType.IDENTIFIER;
    }
    getOperatorToken(char) {
        const operators = {
            '+': { type: types_1.TokenType.PLUS, value: '+', line: this.line, column: this.column },
            '-': { type: types_1.TokenType.MINUS, value: '-', line: this.line, column: this.column },
            '*': { type: types_1.TokenType.MULTIPLY, value: '*', line: this.line, column: this.column },
            '/': { type: types_1.TokenType.DIVIDE, value: '/', line: this.line, column: this.column },
            '=': { type: types_1.TokenType.EQUALS, value: '=', line: this.line, column: this.column },
            '(': { type: types_1.TokenType.LPAREN, value: '(', line: this.line, column: this.column },
            ')': { type: types_1.TokenType.RPAREN, value: ')', line: this.line, column: this.column },
            ',': { type: types_1.TokenType.COMMA, value: ',', line: this.line, column: this.column },
            ':': { type: types_1.TokenType.COLON, value: ':', line: this.line, column: this.column },
            ';': { type: types_1.TokenType.SEMICOLON, value: ';', line: this.line, column: this.column }
        };
        return operators[char] || null;
    }
}
exports.Lexer = Lexer;

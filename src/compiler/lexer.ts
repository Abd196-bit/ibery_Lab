import { Token, TokenType } from './types';

export class Lexer {
    private input: string;
    private position: number = 0;
    private line: number = 1;
    private column: number = 1;

    constructor(input: string) {
        this.input = input;
    }

    private isWhitespace(char: string): boolean {
        return /\s/.test(char);
    }

    private isDigit(char: string): boolean {
        return /[0-9]/.test(char);
    }

    private isAlpha(char: string): boolean {
        return /[a-zA-Z_]/.test(char);
    }

    private isAlphaNumeric(char: string): boolean {
        return this.isAlpha(char) || this.isDigit(char);
    }

    private peek(): string {
        return this.input[this.position] || '\0';
    }

    private advance(): string {
        const char = this.peek();
        this.position++;
        if (char === '\n') {
            this.line++;
            this.column = 1;
        } else {
            this.column++;
        }
        return char;
    }

    private skipWhitespace(): void {
        while (this.isWhitespace(this.peek())) {
            this.advance();
        }
    }

    private readNumber(): string {
        let number = '';
        while (this.isDigit(this.peek())) {
            number += this.advance();
        }
        return number;
    }

    private readIdentifier(): string {
        let identifier = '';
        while (this.isAlphaNumeric(this.peek())) {
            identifier += this.advance();
        }
        return identifier;
    }

    private readString(): string {
        let str = '';
        this.advance(); // Skip opening quote
        while (this.peek() !== '"' && this.peek() !== '\0') {
            str += this.advance();
        }
        this.advance(); // Skip closing quote
        return str;
    }

    private readComment(): void {
        while (this.peek() !== '\n' && this.peek() !== '\0') {
            this.advance();
        }
    }

    public getNextToken(): Token {
        this.skipWhitespace();

        if (this.position >= this.input.length) {
            return {
                type: TokenType.EOF,
                value: '',
                line: this.line,
                column: this.column
            };
        }

        const char = this.peek();

        // Handle numbers
        if (this.isDigit(char)) {
            return {
                type: TokenType.NUMBER,
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
                type: TokenType.STRING,
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

    private getKeywordTokenType(identifier: string): TokenType {
        const keywords: { [key: string]: TokenType } = {
            'def': TokenType.DEF,
            'run': TokenType.RUN,
            'quantum': TokenType.QUANTUM,
            'print': TokenType.PRINT,
            'return': TokenType.RETURN
        };
        return keywords[identifier] || TokenType.IDENTIFIER;
    }

    private getOperatorToken(char: string): Token | null {
        const operators: { [key: string]: Token } = {
            '+': { type: TokenType.PLUS, value: '+', line: this.line, column: this.column },
            '-': { type: TokenType.MINUS, value: '-', line: this.line, column: this.column },
            '*': { type: TokenType.MULTIPLY, value: '*', line: this.line, column: this.column },
            '/': { type: TokenType.DIVIDE, value: '/', line: this.line, column: this.column },
            '=': { type: TokenType.EQUALS, value: '=', line: this.line, column: this.column },
            '(': { type: TokenType.LPAREN, value: '(', line: this.line, column: this.column },
            ')': { type: TokenType.RPAREN, value: ')', line: this.line, column: this.column },
            ',': { type: TokenType.COMMA, value: ',', line: this.line, column: this.column },
            ':': { type: TokenType.COLON, value: ':', line: this.line, column: this.column },
            ';': { type: TokenType.SEMICOLON, value: ';', line: this.line, column: this.column }
        };
        return operators[char] || null;
    }
} 
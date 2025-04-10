export enum TokenType {
    // Keywords
    DEF = 'DEF',
    RUN = 'RUN',
    QUANTUM = 'QUANTUM',
    PRINT = 'PRINT',
    RETURN = 'RETURN',
    
    // Identifiers and literals
    IDENTIFIER = 'IDENTIFIER',
    NUMBER = 'NUMBER',
    STRING = 'STRING',
    
    // Operators
    PLUS = 'PLUS',
    MINUS = 'MINUS',
    MULTIPLY = 'MULTIPLY',
    DIVIDE = 'DIVIDE',
    EQUALS = 'EQUALS',
    
    // Punctuation
    LPAREN = 'LPAREN',
    RPAREN = 'RPAREN',
    COMMA = 'COMMA',
    COLON = 'COLON',
    SEMICOLON = 'SEMICOLON',
    
    // Comments
    COMMENT = 'COMMENT',
    
    // End of file
    EOF = 'EOF'
}

export interface Token {
    type: TokenType;
    value: string;
    line: number;
    column: number;
}

export interface ASTNode {
    type: string;
    children?: ASTNode[];
    value?: any;
    token?: Token;
} 
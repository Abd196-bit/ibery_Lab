"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TokenType = void 0;
var TokenType;
(function (TokenType) {
    // Keywords
    TokenType["DEF"] = "DEF";
    TokenType["RUN"] = "RUN";
    TokenType["QUANTUM"] = "QUANTUM";
    TokenType["PRINT"] = "PRINT";
    TokenType["RETURN"] = "RETURN";
    // Identifiers and literals
    TokenType["IDENTIFIER"] = "IDENTIFIER";
    TokenType["NUMBER"] = "NUMBER";
    TokenType["STRING"] = "STRING";
    // Operators
    TokenType["PLUS"] = "PLUS";
    TokenType["MINUS"] = "MINUS";
    TokenType["MULTIPLY"] = "MULTIPLY";
    TokenType["DIVIDE"] = "DIVIDE";
    TokenType["EQUALS"] = "EQUALS";
    // Punctuation
    TokenType["LPAREN"] = "LPAREN";
    TokenType["RPAREN"] = "RPAREN";
    TokenType["COMMA"] = "COMMA";
    TokenType["COLON"] = "COLON";
    TokenType["SEMICOLON"] = "SEMICOLON";
    // Comments
    TokenType["COMMENT"] = "COMMENT";
    // End of file
    TokenType["EOF"] = "EOF";
})(TokenType || (exports.TokenType = TokenType = {}));

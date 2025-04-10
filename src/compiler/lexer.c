#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// Create a new lexer
Lexer* create_lexer(const char* input) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        return NULL;
    }

    lexer->input = input;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    return lexer;
}

// Destroy a lexer
void destroy_lexer(Lexer* lexer) {
    if (lexer) {
        free(lexer);
    }
}

// Create a new token
static Token* create_token(TokenType type, const char* value, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) {
        return NULL;
    }

    token->type = type;
    token->value = value ? strdup(value) : NULL;
    token->line = line;
    token->column = column;
    return token;
}

// Destroy a token
void destroy_token(Token* token) {
    if (token) {
        if (token->value) {
            free(token->value);
        }
        free(token);
    }
}

// Check if a character is whitespace
static bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Check if a character is a digit
static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

// Check if a character is a letter or underscore
static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

// Check if a character is alphanumeric
static bool is_alphanumeric(char c) {
    return is_alpha(c) || is_digit(c);
}

// Get the current character
static char peek(Lexer* lexer) {
    return lexer->input[lexer->position] ? lexer->input[lexer->position] : '\0';
}

// Advance to the next character
static char advance(Lexer* lexer) {
    char c = peek(lexer);
    lexer->position++;
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    return c;
}

// Skip whitespace
static void skip_whitespace(Lexer* lexer) {
    while (is_whitespace(peek(lexer))) {
        advance(lexer);
    }
}

// Read a number
static char* read_number(Lexer* lexer) {
    int start = lexer->position;
    TokenType type = TOKEN_NUMBER;
    
    if (peek(lexer) == '0') {
        advance(lexer);
        if (peek(lexer) == 'x' || peek(lexer) == 'X') {
            advance(lexer);
            type = TOKEN_HEX;
            while (isxdigit(peek(lexer))) {
                advance(lexer);
            }
        } else if (peek(lexer) == 'b' || peek(lexer) == 'B') {
            advance(lexer);
            type = TOKEN_BINARY;
            while (peek(lexer) == '0' || peek(lexer) == '1') {
                advance(lexer);
            }
        } else if (isdigit(peek(lexer))) {
            type = TOKEN_OCTAL;
            while (isdigit(peek(lexer)) && peek(lexer) < '8') {
                advance(lexer);
            }
        }
    } else {
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
    }
    
    if (peek(lexer) == '.') {
        advance(lexer);
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
    }
    
    if (peek(lexer) == 'e' || peek(lexer) == 'E') {
        advance(lexer);
        if (peek(lexer) == '+' || peek(lexer) == '-') {
            advance(lexer);
        }
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
    }
    
    if (peek(lexer) == 'n') {
        advance(lexer);
        type = TOKEN_BIGINT;
    }
    
    int length = lexer->position - start;
    char* number = (char*)malloc(length + 1);
    strncpy(number, lexer->input + start, length);
    number[length] = '\0';
    
    return create_token(type, number, lexer->line, lexer->column);
}

// Read an identifier
static char* read_identifier(Lexer* lexer) {
    int start = lexer->position;
    while (is_alphanumeric(peek(lexer))) {
        advance(lexer);
    }
    int length = lexer->position - start;
    char* identifier = (char*)malloc(length + 1);
    strncpy(identifier, lexer->input + start, length);
    identifier[length] = '\0';
    return identifier;
}

// Read a string
static char* read_string(Lexer* lexer) {
    advance(lexer); // Skip opening quote
    int start = lexer->position;
    
    while (peek(lexer) != '"' && peek(lexer) != '\0') {
        advance(lexer);
    }
    
    if (peek(lexer) == '\0') {
        fprintf(stderr, "Unterminated string at line %d, column %d\n", 
                lexer->line, lexer->column);
        exit(1);
    }
    
    int length = lexer->position - start;
    char* str = (char*)malloc(length + 1);
    strncpy(str, lexer->input + start, length);
    str[length] = '\0';
    
    advance(lexer); // Skip closing quote
    return str;
}

// Read a comment
static void read_comment(Lexer* lexer) {
    while (peek(lexer) != '\n' && peek(lexer) != '\0') {
        advance(lexer);
    }
}

// Get the token type for a keyword
static TokenType get_keyword_token_type(const char* identifier) {
    static const struct {
        const char* keyword;
        TokenType type;
    } keywords[] = {
        {"def", TOKEN_DEF},
        {"run", TOKEN_RUN},
        {"quantum", TOKEN_QUANTUM},
        {"print", TOKEN_PRINT},
        {"return", TOKEN_RETURN},
        {"hoist", TOKEN_HOIST},
        {"surge", TOKEN_SURGE},
        {"domain", TOKEN_DOMAIN},
        {"import", TOKEN_IMPORT},
        {"export", TOKEN_EXPORT},
        {"async", TOKEN_ASYNC},
        {"await", TOKEN_AWAIT},
        {"stream", TOKEN_STREAM},
        {"class", TOKEN_CLASS},
        {"interface", TOKEN_INTERFACE},
        {"implements", TOKEN_IMPLEMENTS},
        {"extends", TOKEN_EXTENDS},
        {"public", TOKEN_PUBLIC},
        {"private", TOKEN_PRIVATE},
        {"protected", TOKEN_PROTECTED},
        {"static", TOKEN_STATIC},
        {"final", TOKEN_FINAL},
        {"abstract", TOKEN_ABSTRACT},
        {"virtual", TOKEN_VIRTUAL},
        {"override", TOKEN_OVERRIDE},
        {"new", TOKEN_NEW},
        {"this", TOKEN_THIS},
        {"super", TOKEN_SUPER},
        {"instanceof", TOKEN_INSTANCEOF},
        {"typeof", TOKEN_TYPEOF},
        {"delete", TOKEN_DELETE},
        {"in", TOKEN_IN},
        {"of", TOKEN_OF},
        {"yield", TOKEN_YIELD},
        {"generator", TOKEN_GENERATOR},
        {"try", TOKEN_TRY},
        {"catch", TOKEN_CATCH},
        {"finally", TOKEN_FINALLY},
        {"throw", TOKEN_THROW},
        {"raise", TOKEN_RAISE},
        {"with", TOKEN_WITH},
        {"switch", TOKEN_SWITCH},
        {"case", TOKEN_CASE},
        {"default", TOKEN_DEFAULT},
        {"break", TOKEN_BREAK},
        {"continue", TOKEN_CONTINUE},
        {"if", TOKEN_IF},
        {"else", TOKEN_ELSE},
        {"elif", TOKEN_ELIF},
        {"while", TOKEN_WHILE},
        {"do", TOKEN_DO},
        {"for", TOKEN_FOR},
        {"foreach", TOKEN_FOREACH},
        {"match", TOKEN_MATCH},
        {"when", TOKEN_WHEN},
        {"where", TOKEN_WHERE},
        {"let", TOKEN_LET},
        {"const", TOKEN_CONST},
        {"var", TOKEN_VAR},
        {"type", TOKEN_TYPE},
        {"enum", TOKEN_ENUM},
        {"union", TOKEN_UNION},
        {"intersection", TOKEN_INTERSECTION},
        {"namespace", TOKEN_NAMESPACE},
        {"using", TOKEN_USING},
        {"as", TOKEN_AS},
        {"is", TOKEN_IS},
        {"null", TOKEN_NULL},
        {"undefined", TOKEN_UNDEFINED},
        {"true", TOKEN_TRUE},
        {"false", TOKEN_FALSE},
        {"infinity", TOKEN_INFINITY},
        {"nan", TOKEN_NAN},
        
        // AI and API Integration
        {"ai", TOKEN_AI},
        {"apiKey", TOKEN_API_KEY},
        {"openai", TOKEN_OPENAI},
        {"model", TOKEN_MODEL},
        {"prompt", TOKEN_PROMPT},
        {"completion", TOKEN_COMPLETION},
        {"temperature", TOKEN_TEMPERATURE},
        {"maxTokens", TOKEN_MAX_TOKENS},
        {"response", TOKEN_RESPONSE},
        {"context", TOKEN_CONTEXT},
        {"system", TOKEN_SYSTEM},
        {"user", TOKEN_USER},
        {"assistant", TOKEN_ASSISTANT},
        
        // Server and API
        {"server", TOKEN_SERVER},
        {"route", TOKEN_ROUTE},
        {"get", TOKEN_GET},
        {"post", TOKEN_POST},
        {"put", TOKEN_PUT},
        {"patch", TOKEN_PATCH},
        {"options", TOKEN_OPTIONS},
        {"head", TOKEN_HEAD},
        {"middleware", TOKEN_MIDDLEWARE},
        {"request", TOKEN_REQUEST},
        {"params", TOKEN_PARAMS},
        {"query", TOKEN_QUERY},
        {"body", TOKEN_BODY},
        {"headers", TOKEN_HEADERS},
        {"cookies", TOKEN_COOKIES},
        {"session", TOKEN_SESSION},
        {"auth", TOKEN_AUTH},
        {"jwt", TOKEN_JWT},
        {"bearer", TOKEN_BEARER},
        {"cors", TOKEN_CORS},
        {"port", TOKEN_PORT},
        {"host", TOKEN_HOST},
        {"database", TOKEN_DATABASE},
        {"connect", TOKEN_CONNECT},
        {"disconnect", TOKEN_DISCONNECT},
        {"websocket", TOKEN_WEBSOCKET},
        {"socket", TOKEN_SOCKET},
        {"emit", TOKEN_EMIT},
        {"on", TOKEN_ON},
        {"off", TOKEN_OFF},
        
        // Styling and UI
        {"style", TOKEN_STYLE},
        {"css", TOKEN_CSS},
        {"html", TOKEN_HTML},
        {"component", TOKEN_COMPONENT},
        {"template", TOKEN_TEMPLATE},
        {"layout", TOKEN_LAYOUT},
        {"view", TOKEN_VIEW},
        {"page", TOKEN_PAGE},
        {"section", TOKEN_SECTION},
        {"div", TOKEN_DIV},
        {"span", TOKEN_SPAN},
        {"color", TOKEN_COLOR},
        {"background", TOKEN_BACKGROUND},
        {"font", TOKEN_FONT},
        {"size", TOKEN_SIZE},
        {"margin", TOKEN_MARGIN},
        {"padding", TOKEN_PADDING},
        {"border", TOKEN_BORDER},
        {"flex", TOKEN_FLEX},
        {"grid", TOKEN_GRID},
        {"align", TOKEN_ALIGN},
        {"justify", TOKEN_JUSTIFY},
        {"width", TOKEN_WIDTH},
        {"height", TOKEN_HEIGHT},
        {"position", TOKEN_POSITION},
        {"animation", TOKEN_ANIMATION},
        {"transition", TOKEN_TRANSITION},
        {"media", TOKEN_MEDIA},
        {"responsive", TOKEN_RESPONSIVE},
        {"theme", TOKEN_THEME},
        {"dark", TOKEN_DARK},
        {"light", TOKEN_LIGHT},
        
        // AI Lab Integration
        {"lab", TOKEN_LAB},
        {"experiment", TOKEN_EXPERIMENT},
        {"dataset", TOKEN_DATASET},
        {"train", TOKEN_TRAIN},
        {"test", TOKEN_TEST},
        {"validate", TOKEN_VALIDATE},
        {"metrics", TOKEN_METRICS},
        {"accuracy", TOKEN_ACCURACY},
        {"loss", TOKEN_LOSS},
        {"epoch", TOKEN_EPOCH},
        {"batch", TOKEN_BATCH},
        {"learningRate", TOKEN_LEARNING_RATE},
        {"optimizer", TOKEN_OPTIMIZER},
        {"checkpoint", TOKEN_CHECKPOINT},
        {"save", TOKEN_SAVE},
        {"load", TOKEN_LOAD},
        {"exportModel", TOKEN_EXPORT_MODEL},
        {"importModel", TOKEN_IMPORT_MODEL},
        {"predict", TOKEN_PREDICT},
        {"inference", TOKEN_INFERENCE},
        
        {NULL, TOKEN_IDENTIFIER}
    };

    for (int i = 0; keywords[i].keyword; i++) {
        if (strcmp(identifier, keywords[i].keyword) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

// Read a regex pattern
static char* read_regex(Lexer* lexer) {
    advance(lexer); // Skip opening slash
    int start = lexer->position;
    
    while (peek(lexer) != '/' && peek(lexer) != '\0') {
        if (peek(lexer) == '\\') {
            advance(lexer); // Skip escape character
        }
        advance(lexer);
    }
    
    if (peek(lexer) == '\0') {
        fprintf(stderr, "Unterminated regex at line %d, column %d\n", 
                lexer->line, lexer->column);
        exit(1);
    }
    
    int length = lexer->position - start;
    char* regex = (char*)malloc(length + 1);
    strncpy(regex, lexer->input + start, length);
    regex[length] = '\0';
    
    advance(lexer); // Skip closing slash
    return regex;
}

// Read a template string
static char* read_template_string(Lexer* lexer) {
    advance(lexer); // Skip opening backtick
    int start = lexer->position;
    
    while (peek(lexer) != '`' && peek(lexer) != '\0') {
        if (peek(lexer) == '\\') {
            advance(lexer); // Skip escape character
        }
        advance(lexer);
    }
    
    if (peek(lexer) == '\0') {
        fprintf(stderr, "Unterminated template string at line %d, column %d\n", 
                lexer->line, lexer->column);
        exit(1);
    }
    
    int length = lexer->position - start;
    char* template = (char*)malloc(length + 1);
    strncpy(template, lexer->input + start, length);
    template[length] = '\0';
    
    advance(lexer); // Skip closing backtick
    return template;
}

// Read a character literal
static char* read_char(Lexer* lexer) {
    advance(lexer); // Skip opening quote
    int start = lexer->position;
    
    if (peek(lexer) == '\\') {
        advance(lexer); // Skip escape character
    }
    advance(lexer);
    
    if (peek(lexer) != '\'') {
        fprintf(stderr, "Unterminated character literal at line %d, column %d\n", 
                lexer->line, lexer->column);
        exit(1);
    }
    
    int length = lexer->position - start;
    char* ch = (char*)malloc(length + 1);
    strncpy(ch, lexer->input + start, length);
    ch[length] = '\0';
    
    advance(lexer); // Skip closing quote
    return create_token(TOKEN_CHAR, ch, lexer->line, lexer->column);
}

// Read a multiline comment
static void read_multiline_comment(Lexer* lexer) {
    advance(lexer); // Skip opening #
    advance(lexer); // Skip opening [
    
    while (peek(lexer) != '\0') {
        if (peek(lexer) == ']' && peek_next(lexer) == '#') {
            advance(lexer);
            advance(lexer);
            return;
        }
        advance(lexer);
    }
    
    fprintf(stderr, "Unterminated multiline comment at line %d, column %d\n", 
            lexer->line, lexer->column);
    exit(1);
}

// Get the next token
Token* get_next_token(Lexer* lexer) {
    skip_whitespace(lexer);

    if (peek(lexer) == '\0') {
        return create_token(TOKEN_EOF, NULL, lexer->line, lexer->column);
    }

    char c = peek(lexer);
    int line = lexer->line;
    int column = lexer->column;

    // Handle numbers
    if (is_digit(c)) {
        return read_number(lexer);
    }

    // Handle identifiers and keywords
    if (is_alpha(c)) {
        char* identifier = read_identifier(lexer);
        TokenType type = get_keyword_token_type(identifier);
        return create_token(type, identifier, line, column);
    }

    // Handle strings
    if (c == '"' || c == '\'') {
        if (c == '\'') {
            return read_char(lexer);
        }
        char* str = read_string(lexer);
        return create_token(TOKEN_STRING, str, line, column);
    }

    // Handle regex patterns
    if (c == '/') {
        char* regex = read_regex(lexer);
        return create_token(TOKEN_REGEX, regex, line, column);
    }

    // Handle template strings
    if (c == '`') {
        char* template = read_template_string(lexer);
        return create_token(TOKEN_TEMPLATE_STRING, template, line, column);
    }

    // Handle comments
    if (c == '#') {
        if (peek_next(lexer) == '[') {
            read_multiline_comment(lexer);
            return create_token(TOKEN_MULTILINE_COMMENT, NULL, line, column);
        } else if (peek_next(lexer) == '#') {
            advance(lexer);
            read_comment(lexer);
            return create_token(TOKEN_DOC_COMMENT, NULL, line, column);
        }
        read_comment(lexer);
        return create_token(TOKEN_COMMENT, NULL, line, column);
    }

    // Handle operators and punctuation
    switch (c) {
        case '+':
            advance(lexer);
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_PLUS_EQUALS, "+=", line, column);
            } else if (peek(lexer) == '+') {
                advance(lexer);
                return create_token(TOKEN_INCREMENT, "++", line, column);
            }
            return create_token(TOKEN_PLUS, "+", line, column);
            
        case '-':
            advance(lexer);
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_MINUS_EQUALS, "-=", line, column);
            } else if (peek(lexer) == '-') {
                advance(lexer);
                return create_token(TOKEN_DECREMENT, "--", line, column);
            } else if (peek(lexer) == '>') {
                advance(lexer);
                if (peek(lexer) == '>') {
                    advance(lexer);
                    return create_token(TOKEN_DOUBLE_ARROW, "->>", line, column);
                }
                return create_token(TOKEN_ARROW, "->", line, column);
            }
            return create_token(TOKEN_MINUS, "-", line, column);
            
        case '*':
            advance(lexer);
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_MULTIPLY_EQUALS, "*=", line, column);
            } else if (peek(lexer) == '*') {
                advance(lexer);
                return create_token(TOKEN_POWER, "**", line, column);
            }
            return create_token(TOKEN_MULTIPLY, "*", line, column);
            
        case '/':
            advance(lexer);
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_DIVIDE_EQUALS, "/=", line, column);
            }
            return create_token(TOKEN_DIVIDE, "/", line, column);
            
        case '%':
            advance(lexer);
            return create_token(TOKEN_MODULO, "%", line, column);
            
        case '=':
            advance(lexer);
            if (peek(lexer) == '=') {
                advance(lexer);
                if (peek(lexer) == '=') {
                    advance(lexer);
                    return create_token(TOKEN_EQUAL_EQUAL_EQUAL, "===", line, column);
                }
                return create_token(TOKEN_EQUAL_EQUAL, "==", line, column);
            }
            return create_token(TOKEN_EQUALS, "=", line, column);
            
        case '!':
            advance(lexer);
            if (peek(lexer) == '=') {
                advance(lexer);
                if (peek(lexer) == '=') {
                    advance(lexer);
                    return create_token(TOKEN_NOT_EQUAL_EQUAL, "!==", line, column);
                }
                return create_token(TOKEN_NOT_EQUAL, "!=", line, column);
            }
            return create_token(TOKEN_NOT, "!", line, column);
            
        case '>':
            advance(lexer);
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_GREATER_EQUAL, ">=", line, column);
            } else if (peek(lexer) == '>') {
                advance(lexer);
                if (peek(lexer) == '>') {
                    advance(lexer);
                    return create_token(TOKEN_UNSIGNED_RIGHT_SHIFT, ">>>", line, column);
                }
                return create_token(TOKEN_RIGHT_SHIFT, ">>", line, column);
            }
            return create_token(TOKEN_GREATER, ">", line, column);
            
        case '<':
            advance(lexer);
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_LESS_EQUAL, "<=", line, column);
            } else if (peek(lexer) == '<') {
                advance(lexer);
                return create_token(TOKEN_LEFT_SHIFT, "<<", line, column);
            }
            return create_token(TOKEN_LESS, "<", line, column);
            
        case '&':
            advance(lexer);
            if (peek(lexer) == '&') {
                advance(lexer);
                return create_token(TOKEN_AND, "&&", line, column);
            }
            return create_token(TOKEN_BITWISE_AND, "&", line, column);
            
        case '|':
            advance(lexer);
            if (peek(lexer) == '|') {
                advance(lexer);
                return create_token(TOKEN_OR, "||", line, column);
            } else if (peek(lexer) == '>') {
                advance(lexer);
                return create_token(TOKEN_PIPE, "|>", line, column);
            }
            return create_token(TOKEN_BITWISE_OR, "|", line, column);
            
        case '^':
            advance(lexer);
            return create_token(TOKEN_BITWISE_XOR, "^", line, column);
            
        case '~':
            advance(lexer);
            return create_token(TOKEN_BITWISE_NOT, "~", line, column);
            
        case '?':
            advance(lexer);
            if (peek(lexer) == '?') {
                advance(lexer);
                return create_token(TOKEN_NULLISH_COALESCING, "??", line, column);
            } else if (peek(lexer) == '.') {
                advance(lexer);
                return create_token(TOKEN_OPTIONAL_CHAINING, "?.", line, column);
            }
            return create_token(TOKEN_QUESTION, "?", line, column);
            
        case '.':
            advance(lexer);
            if (peek(lexer) == '.') {
                advance(lexer);
                if (peek(lexer) == '.') {
                    advance(lexer);
                    return create_token(TOKEN_SPREAD, "...", line, column);
                }
                return create_token(TOKEN_DOUBLE_DOT, "..", line, column);
            }
            return create_token(TOKEN_DOT, ".", line, column);
            
        case '@':
            advance(lexer);
            return create_token(TOKEN_AT, "@", line, column);
            
        case '$':
            advance(lexer);
            return create_token(TOKEN_DOLLAR, "$", line, column);
            
        case '`':
            advance(lexer);
            return create_token(TOKEN_BACKTICK, "`", line, column);
            
        case '(': advance(lexer); return create_token(TOKEN_LEFT_PAREN, "(", line, column);
        case ')': advance(lexer); return create_token(TOKEN_RIGHT_PAREN, ")", line, column);
        case '{': advance(lexer); return create_token(TOKEN_LEFT_BRACE, "{", line, column);
        case '}': advance(lexer); return create_token(TOKEN_RIGHT_BRACE, "}", line, column);
        case '[': advance(lexer); return create_token(TOKEN_LEFT_BRACKET, "[", line, column);
        case ']': advance(lexer); return create_token(TOKEN_RIGHT_BRACKET, "]", line, column);
        case '<': advance(lexer); return create_token(TOKEN_LEFT_ANGLE, "<", line, column);
        case '>': advance(lexer); return create_token(TOKEN_RIGHT_ANGLE, ">", line, column);
        case ',': advance(lexer); return create_token(TOKEN_COMMA, ",", line, column);
        case ':': 
            advance(lexer);
            if (peek(lexer) == ':') {
                advance(lexer);
                return create_token(TOKEN_DOUBLE_COLON, "::", line, column);
            }
            return create_token(TOKEN_COLON, ":", line, column);
        case ';': advance(lexer); return create_token(TOKEN_SEMICOLON, ";", line, column);
    }

    fprintf(stderr, "Unexpected character: '%c' at line %d, column %d\n", 
            c, line, column);
    exit(1);
} 
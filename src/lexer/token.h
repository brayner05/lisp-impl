#ifndef TOKEN_H
#define TOKEN_H
#include <stdbool.h>
#include "../util_types.h"

typedef enum {
    // Operator tokens
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_ASTERISK, TOKEN_SLASH,

    // Parentheses
    TOKEN_LPAREN, TOKEN_RPAREN,

    // Keyword tokens
    TOKEN_DEFINE, TOKEN_VAR, TOKEN_LAMBDA,

    // Literal tokens
    TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_STRING,
    TOKEN_TRUE, TOKEN_FALSE, TOKEN_NIL,

    // Miscellaneous tokens
    TOKEN_EOF
} LispTokenType;


typedef struct {
    LispTokenType type;
    char *file_name;
    u32 line;
    u16 column;
    char *begin;
    char *end;
} LispToken;


inline static bool token_is_operator(LispToken *token) {
    switch (token->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_ASTERISK:
        case TOKEN_SLASH:
            return true;
    }
    return false;
}


inline static const char *token_type_to_string(LispTokenType type) {
    switch (type) {
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_ASTERISK: return "ASTERISK";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_DEFINE: return "DEFINE";
        case TOKEN_VAR: return "VAR";
        case TOKEN_LAMBDA: return "LAMBDA";
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_FLOAT: return "FLOAT";
        case TOKEN_STRING: return "STRING";
        case TOKEN_TRUE: return "BOOLEAN(TRUE)";
        case TOKEN_FALSE: return "BOOLEAN(FALSE)";
        case TOKEN_NIL: return "NIL";
        case TOKEN_EOF: return "EOF";
        default: return "<UNDEFINED>";
    }
}

#endif
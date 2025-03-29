#ifndef TOKEN_H
#define TOKEN_H
#include <stdbool.h>
#include "../util_types.h"
#include "../lisp/error.h"

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


typedef struct {
    bool failed;
    union {
        LispToken *token;
        LispError *error;
    };
} TokenResult;


/**
 * Check if a token is an operator.
 * 
 * @param token The token to check.
 * @return Whether the token is an operator token or not.
 */
inline static bool token_is_operator(LispToken *token) {
    switch (token->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_ASTERISK:
        case TOKEN_SLASH: {
            return true;
        }
        default: return false;
    }
    return false;
}


/**
 * Get the string representation of a `LispTokenType`.
 * 
 * @param type The type to get as a string.
 * @return The string representation of the token type, or
 * "<UNDEFINED>".
 */
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

/**
 * Get the string representation of a token.
 * 
 * @param token The token of which to get the string representation.
 * @return A pointer to a string representing the data in `token`.
 */
extern const char *token_to_string(LispToken *token);

#endif
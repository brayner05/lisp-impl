#include <stdlib.h>
#include <stdio.h>

#include "token.h"

extern char *token_to_string(LispToken *token) {
    if (token == NULL) {
        return NULL;
    }

    // Compute the length of the string to allocate.
    i32 lexeme_length = (i32) (token->end - token->begin);
    i32 string_length = snprintf(NULL, 0, 
        "LispToken => %s '%.*s' @ %s:%u%u", 
        token_type_to_string(token->type), lexeme_length,
        token->begin, token->file_name, 
        token->line, token->column);

    // Allocate a string buffer to store the token data.
    char *token_string = (char *) calloc(string_length, sizeof(char));
    if (token_string == NULL) {
        return NULL;
    }

    // Load the token data into the buffer.
    snprintf(token_string, string_length, 
        "LispToken => %s '%.*s' @ %s:%u%u", 
        token_type_to_string(token->type), lexeme_length, 
        token->begin, token->file_name, 
        token->line, token->column);

    // Return the buffer;
    return token_string;
}
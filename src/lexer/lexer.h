#ifndef LEXER_H
#define LEXER_H
#include <stdbool.h>

#include "token.h"
#include "../lisp/error.h"

typedef struct TokenList {
    LispToken *token;
    struct TokenList *next;
} TokenList;


typedef struct {
    bool failed;
    union {
        TokenList *tokens;
        LispError *error;
    };
} TokenListResult;


extern TokenListResult lexer_tokenize(char *source, size_t source_length);


#endif
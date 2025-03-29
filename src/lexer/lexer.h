#ifndef LEXER_H
#define LEXER_H
#include "token.h"

typedef struct TokenList {
    LispToken *token;
    struct TokenList *next;
} TokenList;


extern TokenList *lexer_tokenize(char *source, size_t source_length);


#endif
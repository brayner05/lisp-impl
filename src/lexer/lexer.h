#ifndef LEXER_H
#define LEXER_H
#include <stdbool.h>

#include "token.h"
#include "../lisp/error.h"


typedef struct {
    bool failed;
    union {
        TokenList *tokens;
        LispError *error;
    };
} TokenListResult;


/**
 * Scan `source` and create a list of tokens based on the content.
 * @return a `TokenListResult` tracking whether or not the tokenization has
 * succeeded, if not the `TokenListResult` will contain and error, otherwise
 * it will contain a `TokenList`.
 */
extern TokenListResult lexer_tokenize(char *source, size_t source_length);


#endif
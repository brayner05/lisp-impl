#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "../lisp/error.h"

typedef struct {
    bool failed;
    union {
        LispError *error;
        AstNode *ast;
    };
} AstResult;


extern AstResult parser_build_ast(TokenList *token_stream);


#endif
#include <stdlib.h>
#include <stdbool.h>

#include "../util_types.h"
#include "../lexer/token.h"
#include "ast.h"
#include "parser.h"


typedef struct Parser {
    TokenList *token_stream;
} Parser;


extern AstResult parser_build_ast(TokenList *token_stream) {
    AstResult result = { .failed = false, .error = NULL };

    Parser parser = {
        .token_stream = token_stream
    };

    return result;
}
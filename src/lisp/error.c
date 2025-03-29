#include <stdlib.h>
#include "error.h"

static LispError *lisp_create_error(char *message, LispErrorType error_type) {
    LispError *error = (LispError *) calloc(1, sizeof(LispError *));
    error->type = error_type;
    error->message = message;
    return error;
}


extern LispError *lisp_lexer_error(char *message, u32 line, u16 column) {
    LispError *error = lisp_create_error(message, LISP_LEXER_ERROR);
    error->lexer_error.line = line;
    error->lexer_error.column = column;
    return error;
}


extern LispError *lisp_parser_error(char *message, u32 line, u16 column) {
    return lisp_lexer_error(message, line, column);
}


extern LispError *lisp_internal_error(char *message, InternalErrorType type) {
    LispError *error = lisp_create_error(message, LISP_INTERNAL_ERROR);
    error->internalError.type = type;
    return error;
}

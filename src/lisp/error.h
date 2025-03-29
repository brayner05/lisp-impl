#ifndef ERROR_H
#define ERROR_H
#include <stdbool.h>

#include "../util_types.h"

typedef enum {
    LISP_LEXER_ERROR,
    LISP_PARSER_ERROR,
    LISP_INTERNAL_ERROR
} LispErrorType;


typedef enum {
    LISP_OUT_OF_MEMORY,
    LISP_NULL_REFERENCE
} InternalErrorType;


typedef struct {
    u32 line;
    u16 column;
} LispLexerError;


// For readability reasons
typedef LispLexerError LispParserError;


typedef struct {
    InternalErrorType type;
} LispInternalError;


typedef struct {
    LispErrorType type;
    char *message;
    union {
        LispLexerError lexer_error;
        LispParserError parser_error;
        LispInternalError internalError;
    };
} LispError;


typedef struct {
    bool failed;
    union {
        char *string;
        LispError *error;
    };
} StringResult;


extern LispError *lisp_lexer_error(char *message, u32 line, u16 column);

extern LispError *lisp_parser_error(char *message, u32 line, u16 column);

extern LispError *lisp_internal_error(char *message, InternalErrorType type);


#endif
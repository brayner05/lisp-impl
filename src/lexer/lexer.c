#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include "lexer.h"
#include "../util_types.h"
#include "../lisp/error.h"


typedef struct {
    char *source;
    size_t source_length;
    u64 token_start;
    u64 position;
    u64 line;
    TokenList *tokens_begin;
    TokenList *tokens_end;
} Lexer;


typedef struct ScanningError {
    bool failed;
    LispError *error;
} ScanResult;


/**
 * Check if the lexer has reached the end of the
 * source code.
 */
static bool lexer_has_next(Lexer *lexer) {
    return lexer->position < lexer->source_length;
}


static void lexer_add_token(Lexer *lexer, LispTokenType type) {
    LispToken *token = (LispToken *) calloc(1, sizeof(LispToken));

    token->type = type;
    token->line = lexer->line;
    token->column = lexer->position;
    token->file_name = "stdin",
    token->begin = &lexer->source[lexer->token_start];
    token->end = &lexer->source[lexer->position];
    
    TokenList *node = (TokenList *) calloc(1, sizeof(TokenList));
    node->next = NULL;
    node->token = token;

    if (lexer->tokens_begin == NULL) {
        lexer->tokens_begin = node;
        lexer->tokens_end = lexer->tokens_begin;
        return;
    }

    lexer->tokens_end->next = node;
    lexer->tokens_end = node;
}


static char lexer_peek(Lexer *lexer) {
    if (!lexer_has_next(lexer)) {
        return (char) 0;
    }
    return lexer->source[lexer->position];
}


static char lexer_advance(Lexer *lexer) {
    return lexer->source[lexer->position++];
}


/**
 * Scan a number from the source code and convert it to
 * a `LispToken`.
 */
static ScanResult lexer_scan_number(Lexer *lexer) {
    ScanResult result = { .failed = false, .error = NULL };

    while (lexer_has_next(lexer) && isdigit(lexer_peek(lexer))) {
        lexer_advance(lexer);
    }

    if (lexer_peek(lexer) == '.') {
        lexer_advance(lexer);
        while (lexer_has_next(lexer) && isdigit(lexer_peek(lexer))) {
            lexer_advance(lexer);
        }
        lexer_add_token(lexer, TOKEN_FLOAT);
        return result;
    }

    lexer_add_token(lexer, TOKEN_INTEGER);

    return result;
}


static ScanResult lexer_scan_next(Lexer *lexer) {
    char ch = lexer_advance(lexer);
    ScanResult result = { .failed = false, .error = NULL };

    switch (ch) {
        case '\n': {
            lexer->line++;
            break;
        }

        case ' ':
        case '\t':
        case '\r': {
            break;
        }

        case '+': {
            lexer_add_token(lexer, TOKEN_PLUS); 
            break;
        }

        case '-': {
            lexer_add_token(lexer, TOKEN_MINUS);
            break;
        }

        case '*': {
            lexer_add_token(lexer, TOKEN_ASTERISK);
            break;
        }

        case '/': {
            lexer_add_token(lexer, TOKEN_SLASH);
            break;
        }

        case '(': {
            lexer_add_token(lexer, TOKEN_LPAREN);
            break;
        }

        case ')': {
            lexer_add_token(lexer, TOKEN_RPAREN);
            break;
        }

        default: {
            if (isdigit(ch)) {
                result = lexer_scan_number(lexer);
                break;
            }

            result.failed = true;
            result.error = lisp_lexer_error(
                "Unrecognized token.", 
                lexer->line, 
                lexer->position
            );
            break;
        }
    }

    return result;
}


extern TokenListResult lexer_tokenize(char *source, size_t source_length) {
    TokenListResult result;
    
    Lexer lexer = {
        .source = source,
        .source_length = source_length,
        .token_start = 0,
        .position = 0,
        .line = 0,
        .tokens_begin = NULL,
        .tokens_end = NULL
    };

    while (lexer_has_next(&lexer)) {
        ScanResult error = lexer_scan_next(&lexer);
        lexer.token_start = lexer.position;

        if (error.failed) {
            result.failed = true;
            result.error = error.error;
            return result;
        }
    }

    lexer_add_token(&lexer, TOKEN_EOF);

    result.failed = false;
    result.tokens = lexer.tokens_begin;

    return result;
}
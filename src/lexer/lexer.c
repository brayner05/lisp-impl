#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"
#include "../util_types.h"
#include "../lisp/error.h"

#define KEYWORD_TABLE_SIZE 256


/**
 * A struct keeping track of everything related to the lexer.
 * It exists so that two lines of the lexer being run from the 
 * REPL do not share the same state. Notice however, that keywords are
 * stored statically, separate from the lexer.
 */
typedef struct {
    // A pointer to the source code.
    char *source;
    // The length of the source code.
    size_t source_length;
    // The index of the start of the current token in the source code.
    u64 token_start;
    // The current position of the lexer in the source code.
    u64 position;
    // The current line number.
    u64 line;
    // The head of the linked-list of tokens.
    TokenList *tokens_begin;
    // The tail of the linked-list of tokens.
    TokenList *tokens_end;
} Lexer;


/**
 * A basic error for lexer functions. `ScanningError` is
 * basically the implementation of something like: `Error<Void>`.
 */
typedef struct ScanningError {
    // Tracks whether or not an error has occurred.
    bool failed;
    // If `failed` is `true` then `error` contains the value of the error,
    // otherwise `error` is `NULL`.
    LispError *error;
} ScanResult;


static LispTokenType keyword_table[KEYWORD_TABLE_SIZE];

static bool keywords_initialized = false;


/**
 * Compute the hash value of a string, to be used 
 * to compute the indices in which to store keywords.
 * 
 * @return The hash value of `key`.
 */
static u64 hash_keyword_string(char *key) {
    u64 hash = 5381;
    char *iterator = key;
    while (*iterator != '\0') {
        hash = ((hash << 5) + hash) + *iterator++;
    }
    return hash;
}


/**
 * Gets the keyword value associated with the substring of the source code
 * from `key_begin` (inclusive) to `key_end` (exclusive). If no keyword exists
 * with such a key, then this function returns `TOKEN_INVALID`.
 */
static LispTokenType get_keyword_token_type(char *key_begin, char *key_end) {
    size_t lexeme_length = (size_t) (key_end - key_begin);
    char *key = (char *) calloc(lexeme_length + 1, sizeof(char));
    strncpy(key, key_begin, lexeme_length);
    u64 hash = hash_keyword_string(key);
    free(key);
    
    u64 index = hash % KEYWORD_TABLE_SIZE;
    LispTokenType keyword_type = keyword_table[index];

    return keyword_type;
}


/**
 * Add a new keyword to the keyword table.
 */
static void create_keyword(LispTokenType type, char *key) {
    u64 hash = hash_keyword_string(key);
    u64 index = hash % KEYWORD_TABLE_SIZE;
    keyword_table[index] = type;
}


/**
 * Check if the lexer has reached the end of the
 * source code.
 */
static bool lexer_has_next(Lexer *lexer) {
    return lexer->position < lexer->source_length;
}


/**
 * Create a new token on the heap and add it to the end of the
 * linked-list of tokens.
 */
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


/**
 * Get the next character in the source code without
 * advancing the lexer position.
 */
static char lexer_peek(Lexer *lexer) {
    if (!lexer_has_next(lexer)) {
        return (char) 0;
    }
    return lexer->source[lexer->position];
}


/**
 * Get the next character in the source code and then
 * advance the lexer position by 1.
 */
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


/**
 * Scan an identifier token, or if the lexeme matches any entries in the 
 * keyword table, then add a token for that keyword.
 */
static ScanResult lexer_scan_identifier(Lexer *lexer) {
    ScanResult result = { .failed = false, .error = NULL };

    while (lexer_has_next(lexer) && isalnum(lexer_peek(lexer))) {
        lexer_advance(lexer);
    }

    LispTokenType keyword = get_keyword_token_type(
        &lexer->source[lexer->token_start], 
        &lexer->source[lexer->position]);

    if (keyword == TOKEN_INVALID) {
        lexer_add_token(lexer, TOKEN_IDENTIFIER);
        return result;
    }

    lexer_add_token(lexer, keyword);

    return result;
}


/**
 * Scan a string token. Returns a lexer error in the event that
 * the string is not terminated with a terminating quote.
 */
static ScanResult lexer_scan_string(Lexer *lexer) {
    ScanResult result = { .failed = false, .error = NULL };

    // Scan until either reaching the end of the source code or
    // finding a terminating quote.
    while (lexer_has_next(lexer) && lexer_peek(lexer) != '\"') {
        lexer_advance(lexer);
    }

    // If the end of the source code is reached and a terminating
    // quote has not been encountered, an error will be returned.
    if (!lexer_has_next(lexer)) {
        result.failed = true;
        result.error = lisp_lexer_error(
            "Unterminated string.", lexer->line, lexer->position);

        return result;
    }

    // Skip the terminating quote and add the token.
    lexer_advance(lexer);
    lexer_add_token(lexer, TOKEN_STRING);

    return result;
}


/**
 * Ignore an inline comment. Any line prefixed with a ';' will
 * be interpreted as a comment and ignored by the language.
 */
static void lexer_skip_comment(Lexer *lexer) {
    // Scan until the end of the line.
    while (lexer_has_next(lexer) && lexer_peek(lexer) != '\n') {
        lexer_advance(lexer);
    }

    // If the end of the line is not the end of the file
    // then skip the newline character.
    if (lexer_has_next(lexer)) {
        lexer_advance(lexer);
    }
}


/**
 * Scan the next token in the source code. Returns an error on fail.
 * @return A `ScanResult` containing information about whether or not the scan
 * failed, in which case the `ScanResult` will contain information about the error, 
 * otherwise the `ScanResult::error` value will be `NULL`.
 */
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

        case '\"': {
            result = lexer_scan_string(lexer);
            break;
        }

        case ';': {
            lexer_skip_comment(lexer);
            break;
        }

        default: {
            if (isdigit(ch)) {
                result = lexer_scan_number(lexer);
                break;
            }

            if (isalpha(ch)) {
                result = lexer_scan_identifier(lexer);
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


static void initialize_keywords(void) {
    // Mark all empty spaces in the keyword table as invalid.
    for (u64 i = 0; i < KEYWORD_TABLE_SIZE; ++i) {
        keyword_table[i] = TOKEN_INVALID;
    }
    create_keyword(TOKEN_DEFINE, "define");
    create_keyword(TOKEN_VAR, "var");
    create_keyword(TOKEN_LAMBDA, "lambda");
    create_keyword(TOKEN_NIL, "nil");
    create_keyword(TOKEN_FALSE, "false");
    create_keyword(TOKEN_TRUE, "true");
}


// @see lexer.h
extern TokenListResult lexer_tokenize(char *source, size_t source_length) {
    TokenListResult result;

    if (!keywords_initialized) {
        initialize_keywords();
        keywords_initialized = true;
    }
    
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util_types.h"
#include "lisp/error.h"
#include "lexer/token.h"
#include "lexer/lexer.h"

#define MAX_LINE_LENGTH 0x400


static void next_line(char *buffer, char *prompt, size_t *out_length) {
    printf("%s > ", prompt);

    u16 number_read = 0;

    int ch;
    while ((ch = getchar()) != EOF && ch != '\n') {
        if (number_read == MAX_LINE_LENGTH - 1) {
            goto terminate_string;
        }
        buffer[number_read++] = ch;
    }

terminate_string:
    *out_length = number_read;
    buffer[number_read] = (char) 0;
}


static void report_error(LispError *error) {
    switch (error->type) {
        case LISP_INTERNAL_ERROR: {
            fprintf(stderr, "\x1b[31merror:\x1b[0m %s\n", error->message);
            break;
        }
        case LISP_LEXER_ERROR: {
            fprintf(stderr, "%u:%u: \x1b[31merror:\x1b[0m %s\n", 
                error->lexer_error.line, 
                error->lexer_error.column,
                error->message);
            break;
        }
        case LISP_PARSER_ERROR: {
            fprintf(stderr, "%u:%u: \x1b[31merror:\x1b[0m %s\n", 
                error->parser_error.line, 
                error->parser_error.column,
                error->message);
            break;
        }
    }
    free(error);
}


static void run_repl(void) {
    char buffer[MAX_LINE_LENGTH];
    size_t line_length = 0;
    while (1) {
        next_line(buffer, "lisp", &line_length);
        if (strncmp(buffer, ".quit", sizeof(".quit")) == 0) {
            break;
        }

        TokenListResult lexer_result = lexer_tokenize(buffer, line_length);
        if (lexer_result.failed) {
            report_error(lexer_result.error);
            continue;
        }
        TokenList *tokens = lexer_result.tokens;
        while (tokens != NULL) {
            TokenList *next = tokens->next;
            char *repr = token_to_string(tokens->token);
            puts(repr);
            free(repr);
            free(tokens->token);
            free(tokens);
            tokens = next;
        }
    }
}

i32 main(i32 argc, char *argv[]) {
    (void) argv[argc * 0];
    run_repl();
}
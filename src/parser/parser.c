#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../util_types.h"
#include "../lexer/token.h"
#include "ast.h"
#include "parser.h"


typedef struct Parser {
    TokenList *token_stream;
} Parser;


typedef struct {
    bool failed;
    union {
        LispError *error;
        AstNode *node;
    };
} ParseResult;


static LispToken *parser_advance(Parser *parser) {
    if (parser->token_stream == NULL) {
        return NULL;
    }
    LispToken *token = parser->token_stream->token;
    parser->token_stream = parser->token_stream->next;
    return token;
}


static bool parser_has_next(Parser *parser) {
    return parser->token_stream != NULL;
}


static LispToken *parser_peek(Parser *parser) {
    if (parser == NULL || parser->token_stream == NULL) {
        return NULL;
    }
    return parser->token_stream->token;
}


static ParseResult parser_parse_identifier(Parser *parser) {
    ParseResult result = { .failed = false, .error = NULL };

    LispToken *identifier = parser_advance(parser);
    AstNode *node = calloc(1, sizeof(AstNode));

    size_t identifier_length = identifier->end - identifier->begin;
    node->type = AST_IDENTIFIER;
    node->terminal.identifier_value = calloc(identifier_length + 1, sizeof(char));
    strncpy(node->terminal.identifier_value, identifier->begin, identifier_length);

    result.node = node;

    return result;
}


static ParseResult parser_parse_parameter(Parser *parser) {
    ParseResult result = { .failed = false, .error = NULL };

    if (!parser_has_next(parser) || parser_peek(parser)->type != TOKEN_IDENTIFIER) {
        result.failed = true;
        result.error = lisp_parser_error("Expected an identifier", 0, 0);
        return result;
    }

    result = parser_parse_identifier(parser);
    if (result.failed) {
        return result;
    }

    AstNode *node = result.node;

    return result;
}


static ParseResult parser_parse_function_definition(Parser *parser) {
    ParseResult result = { .failed = false, .error = NULL };

    if (!parser_has_next(parser) || parser_peek(parser)->type != TOKEN_IDENTIFIER) {
        result.failed = true;
        result.error = lisp_parser_error("Expected an identifier", 0, 0);
        return result;
    }


    AstNode *node = (AstNode *) calloc(1, sizeof(AstNode));
    LispToken *identifier = parser_advance(parser);
    node->function_definition.identifier = identifier;

    if (!parser_has_next(parser) || parser_peek(parser)->type != TOKEN_LPAREN) {
        result.failed = true;
        result.error = lisp_parser_error("Expected a (", 0, 0);
        return result;
    }

    parser_advance(parser);
    
    while (parser_has_next(parser) && parser_peek(parser)->type != TOKEN_RPAREN) {
        result = parser_parse_parameter(parser);
        if (result.failed) {
            return result;
        }
    }

    return result;
}


static ParseResult parser_parse_expression(Parser *parser) {
    ParseResult result = { .failed = false, .error = NULL };

    LispToken *token = parser_peek(parser);
    switch (token->type) {
        case TOKEN_DEFINE: {
            result = parser_parse_function_definition(parser);
            break;
        }
        case TOKEN_IF: {
            break;
        }
        case TOKEN_LAMBDA: {
            break;
        }
        case TOKEN_IDENTIFIER: {
            result = parser_parse_identifier(parser);
            break;
        }
        case TOKEN_VAR: {
            break;
        };
        case TOKEN_INTEGER: {
            break;
        }
        case TOKEN_FLOAT: {
            break;
        }
        case TOKEN_GROUP: {
            break;
        }
        default: {
            result.failed = true;
            result.error = lisp_parser_error(
                "Expected an expression: ", token->line, token->column);
            return result;
        }
    }

    return result;
}


static ParseResult parser_parse_declaration(Parser *parser) {
    ParseResult result = { .failed = false, .error = NULL };

    if (!parser_has_next(parser) || parser_peek(parser)->type != TOKEN_LPAREN) {
        result.failed = true;
        result.error = lisp_parser_error("Expected a '('", 0, 0); // TODO() fix
        return result;
    }

    // Skip the '('
    parser_advance(parser);
    result = parser_parse_expression(parser);

    if (!parser_has_next(parser) || parser_peek(parser)->type != TOKEN_RPAREN) {
        result.failed = true;
        result.error = lisp_parser_error("Expected a ')'", 0, 0); // TODO() fix
        return result;
    }

    // Skip the ')'
    parser_advance(parser);

    return result;
}


extern AstResult parser_build_ast(TokenList *token_stream) {
    AstResult result = { .failed = false, .error = NULL };

    Parser parser = {
        .token_stream = token_stream
    };

    ParseResult parse_result = parser_parse_declaration(&parser);
    if (parse_result.failed) {
        result.failed = true;
        result.error = parse_result.error;
        return result;
    }

    result.ast = parse_result.node;

    return result;
}
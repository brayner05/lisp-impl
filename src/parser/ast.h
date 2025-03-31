#ifndef AST_H
#define AST_H

#include "../lexer/token.h"

typedef enum {
    AST_FUNCTION_DEFINITION,
    AST_FUNCTION_CALL,
    AST_IF_STATEMENT,
    AST_LAMBDA_EXPRESSION,
    AST_VARIABLE_DECLARATION,
    AST_LITERAL,
    AST_IDENTIFIER
} AstNodeType;

struct AstNode;


typedef struct {
    LispToken *value;
    union {
        char *identifier_value;
        char *string_value;
        i64 int_value;
        float float_value;
        bool bool_value;
    };
} TerminalNode;


typedef struct ParameterList {
    TerminalNode *parameter_name;
    struct ParameterList *next;
} ParameterList;


typedef struct {
    LispToken *identifier;
    ParameterList *first_parameter;
    ParameterList *last_parameter;
    struct AstNode *body;
} FunctionDefinition;


typedef struct AstNode {
    AstNodeType type;
    union {
        FunctionDefinition function_definition;
        ParameterList parameter_list;
        TerminalNode terminal;
    };
} AstNode;

#endif
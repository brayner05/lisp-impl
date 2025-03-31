#ifndef AST_H
#define AST_H

#include "../lexer/token.h"

typedef enum {
    AST_FUNCTION_DEFINITION,
    AST_FUNCTION_CALL,
    AST_FUNCTION_CALL,
    AST_IF_STATEMENT,
    AST_LAMBDA_EXPRESSION,
    AST_VARIABLE_DECLARATION,
    AST_LITERAL
} AstNodeType;

struct AstNode;


typedef struct {
    LispToken *value;
} TerminalNode;


typedef TerminalNode Literal;
typedef TerminalNode Identifier;


typedef struct ParameterList {
    Identifier *parameter_name;
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
        Literal literal;
    };
} AstNode;

#endif
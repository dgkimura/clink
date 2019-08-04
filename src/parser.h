#ifndef __PARSER_H__
#define __PARSER_H__

#include "scanner.h"

enum astnode_t
{
    AST_INVALID = -1,
    AST_CONSTANT,
    AST_PRIMARY_EXPRESSION,
    AST_POSTFIX_EXPRESSION,

    /* tokens used in grammar rules */
    AST_IDENTIFIER,
    AST_ARROW,
    AST_PLUS_PLUS,
    AST_MINUS_MINUS
};

struct astnode
{
    enum astnode_t type;
    union
    {
        struct
        {
            struct token *constant;
        };
    };
};

struct astnode *
do_parsing(struct listnode *tokens);

#endif

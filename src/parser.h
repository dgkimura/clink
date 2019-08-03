#ifndef __PARSER_H__
#define __PARSER_H__

#include "scanner.h"

enum astnode_t
{
    AST_INVALID = -1,
    AST_CONSTANT,
    AST_PRIMARY_EXPRESSION
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

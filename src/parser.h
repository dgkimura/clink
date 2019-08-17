#ifndef __PARSER_H__
#define __PARSER_H__

#include "scanner.h"

enum astnode_t
{
    AST_INVALID = -1,
    AST_CONSTANT,
    AST_PRIMARY_EXPRESSION,
    AST_POSTFIX_EXPRESSION,
    AST_UNARY_EXPRESSION,
    AST_CAST_EXPRESSION,
    AST_MULTIPLICATIVE_EXPRESSION,
    AST_ADDITIVE_EXPRESSION,
    AST_SHIFT_EXPRESSION,
    AST_RELATIONAL_EXPRESSION,
    AST_EQUALITY_EXPRESSION,
    AST_AND_EXPRESSION,
    AST_EXCLUSIVE_OR_EXPRESSION,
    AST_INCLUSIVE_OR_EXPRESSION,
    AST_LOGICAL_AND_EXPRESSION,

    /* tokens used in grammar rules */
    AST_IDENTIFIER,
    AST_ARROW,
    AST_PLUS_PLUS,
    AST_MINUS_MINUS,
    AST_PLUS,
    AST_MINUS,
    AST_AMPERSAND,
    AST_AMPERSAND_AMPERSAND,
    AST_ASTERISK,
    AST_BACKSLASH,
    AST_CARET,
    AST_MOD,
    AST_VERTICALBAR,
    AST_SHIFTLEFT,
    AST_SHIFTRIGHT,
    AST_LT,
    AST_GT,
    AST_LTEQ,
    AST_GTEQ,
    AST_EQ,
    AST_NEQ
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
    struct listnode *children;
};

struct astnode *
shift(struct token * token);

struct astnode *
reduce(struct listnode **stack);

struct astnode *
do_parsing(struct listnode *tokens);

#endif

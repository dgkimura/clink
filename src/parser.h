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
    AST_LOGICAL_OR_EXPRESSION,
    AST_CONDITIONAL_EXPRESSION,
    AST_ASSIGNMENT_EXPRESSION,
    AST_EXPRESSION,
    AST_JUMP_STATEMENT,
    AST_ITERATION_STATEMENT,
    AST_SELECTION_STATEMENT,
    AST_STATEMENT_LIST,
    AST_COMPOUND_STATEMENT,
    AST_EXPRESSION_STATEMENT,
    AST_LABELED_STATEMENT,
    AST_STATEMENT,

    AST_DECLARATION_LIST,

    /* tokens used in grammar rules */
    AST_IDENTIFIER,
    AST_ARROW,
    AST_PLUS,
    AST_PLUS_PLUS,
    AST_PLUS_EQUAL,
    AST_MINUS,
    AST_MINUS_MINUS,
    AST_MINUS_EQUAL,
    AST_AMPERSAND,
    AST_AMPERSAND_AMPERSAND,
    AST_ASTERISK,
    AST_ASTERISK_EQUAL,
    AST_BACKSLASH,
    AST_BACKSLASH_EQUAL,
    AST_CARET,
    AST_COMMA,
    AST_MOD,
    AST_MOD_EQUAL,
    AST_QUESTIONMARK,
    AST_COLON,
    AST_SEMICOLON,
    AST_LPAREN,
    AST_RPAREN,
    AST_LBRACE,
    AST_RBRACE,
    AST_VERTICALBAR,
    AST_VERTICALBAR_VERTICALBAR,
    AST_SHIFTLEFT,
    AST_SHIFTRIGHT,
    AST_LT,
    AST_GT,
    AST_LTEQ,
    AST_GTEQ,
    AST_EQ,
    AST_NEQ,
    AST_EQUAL,

    /* reserved words */
    AST_INT,
    AST_CHAR,
    AST_GOTO,
    AST_CONTINUE,
    AST_BREAK,
    AST_RETURN,
    AST_FOR,
    AST_DO,
    AST_WHILE,
    AST_IF,
    AST_ELSE,
    AST_SWITCH,
    AST_CASE,
    AST_DEFAULT
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

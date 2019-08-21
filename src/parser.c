#include <stdlib.h>

#include "parser.h"

#define NUM_RULES 97
#define MAX_ASTNODES 9

struct rule
{
    enum astnode_t type;
    int length_of_nodes;
    enum astnode_t nodes[MAX_ASTNODES];
};

struct rule grammar[NUM_RULES] =
{
    /* direct-abstract-declarator: */
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        3,
        { AST_LPAREN, AST_ABSTRACT_DECLARATOR, AST_RPAREN, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        2,
        { AST_LBRACKET, AST_RBRACKET, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        3,
        { AST_DIRECT_ABSTRACT_DECLARATOR, AST_LBRACKET, AST_RBRACKET, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        3,
        /* K&R grammar seems ambiguous here: constant-expression is a conditional-expression so why make distinction? */
        { AST_LBRACKET, AST_CONDITIONAL_EXPRESSION, AST_RBRACKET, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        4,
        /* K&R grammar seems ambiguous here: constant-expression is a conditional-expression so why make distinction? */
        { AST_DIRECT_ABSTRACT_DECLARATOR, AST_LBRACKET, AST_CONDITIONAL_EXPRESSION, AST_RBRACKET, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        2,
        { AST_LPAREN, AST_RPAREN, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        3,
        { AST_DIRECT_ABSTRACT_DECLARATOR, AST_LPAREN, AST_RPAREN, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        3,
        { AST_LPAREN, AST_PARAMETER_TYPE_LIST, AST_RPAREN, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_DIRECT_ABSTRACT_DECLARATOR,
        4,
        { AST_DIRECT_ABSTRACT_DECLARATOR, AST_LPAREN, AST_PARAMETER_TYPE_LIST, AST_RPAREN, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* statement: */
    {
        AST_STATEMENT,
        1,
        { AST_LABELED_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_STATEMENT,
        1,
        { AST_EXPRESSION_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_STATEMENT,
        1,
        { AST_COMPOUND_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_STATEMENT,
        1,
        { AST_SELECTION_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_STATEMENT,
        1,
        { AST_ITERATION_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_STATEMENT,
        1,
        { AST_JUMP_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* labeled-statement: */
    {
        AST_LABELED_STATEMENT,
        3,
        { AST_IDENTIFIER, AST_COLON, AST_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_LABELED_STATEMENT,
        4,
        /* K&R grammar seems ambiguous here: constant-expression is a conditional-expression so why make distinction? */
        { AST_CASE, AST_CONDITIONAL_EXPRESSION, AST_COLON, AST_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_LABELED_STATEMENT,
        3,
        { AST_DEFAULT, AST_COLON, AST_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* expression-statement: */
    {
        AST_EXPRESSION_STATEMENT,
        1,
        { AST_SEMICOLON, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_EXPRESSION_STATEMENT,
        2,
        { AST_EXPRESSION, AST_SEMICOLON, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* compound-statement: */
    {
        AST_COMPOUND_STATEMENT,
        2,
        { AST_LBRACE, AST_RBRACE, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_COMPOUND_STATEMENT,
        3,
        { AST_LBRACE, AST_DECLARATION_LIST, AST_RBRACE, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_COMPOUND_STATEMENT,
        3,
        { AST_LBRACE, AST_STATEMENT_LIST, AST_RBRACE, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_COMPOUND_STATEMENT,
        4,
        { AST_LBRACE, AST_DECLARATION_LIST, AST_STATEMENT_LIST, AST_RBRACE, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* statement-list: */
    {
        AST_STATEMENT_LIST,
        2,
        { AST_STATEMENT_LIST, AST_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_STATEMENT_LIST,
        1,
        { AST_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* selection-statement: */
    {
        AST_SELECTION_STATEMENT,
        5,
        { AST_IF, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_SELECTION_STATEMENT,
        7,
        { AST_IF, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT, AST_ELSE, AST_STATEMENT, AST_INVALID, AST_INVALID }
    },
    {
        AST_SELECTION_STATEMENT,
        5,
        { AST_SWITCH, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* iteration-statement: */
    {
        AST_ITERATION_STATEMENT,
        5,
        { AST_WHILE, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_ITERATION_STATEMENT,
        7,
        { AST_DO, AST_STATEMENT, AST_WHILE, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_SEMICOLON, AST_INVALID, AST_INVALID }
    },
    {
        AST_ITERATION_STATEMENT,
        6,
        { AST_FOR, AST_LPAREN, AST_SEMICOLON, AST_SEMICOLON, AST_RPAREN, AST_STATEMENT, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_ITERATION_STATEMENT,
        7,
        { AST_FOR, AST_LPAREN, AST_EXPRESSION, AST_SEMICOLON, AST_SEMICOLON, AST_RPAREN, AST_STATEMENT, AST_INVALID, AST_INVALID }
    },
    {
        AST_ITERATION_STATEMENT,
        7,
        { AST_FOR, AST_LPAREN, AST_SEMICOLON, AST_EXPRESSION, AST_SEMICOLON, AST_RPAREN, AST_STATEMENT, AST_INVALID, AST_INVALID }
    },
    {
        AST_ITERATION_STATEMENT,
        7,
        { AST_FOR, AST_LPAREN, AST_SEMICOLON, AST_SEMICOLON, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT, AST_INVALID, AST_INVALID }
    },
    {
        AST_ITERATION_STATEMENT,
        8,
        { AST_FOR, AST_LPAREN, AST_EXPRESSION, AST_SEMICOLON, AST_EXPRESSION, AST_SEMICOLON, AST_RPAREN, AST_STATEMENT, AST_INVALID }
    },
    {
        AST_ITERATION_STATEMENT,
        8,
        { AST_FOR, AST_LPAREN, AST_EXPRESSION, AST_SEMICOLON, AST_SEMICOLON, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT, AST_INVALID }
    },
    {
        AST_ITERATION_STATEMENT,
        8,
        { AST_FOR, AST_LPAREN, AST_SEMICOLON, AST_EXPRESSION, AST_SEMICOLON, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT, AST_INVALID }
    },
    {
        AST_ITERATION_STATEMENT,
        9,
        { AST_FOR, AST_LPAREN, AST_EXPRESSION, AST_SEMICOLON, AST_EXPRESSION, AST_SEMICOLON, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT }
    },
    /* jump-statement: */
    {
        AST_JUMP_STATEMENT,
        3,
        { AST_GOTO, AST_IDENTIFIER, AST_SEMICOLON, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_JUMP_STATEMENT,
        2,
        { AST_CONTINUE, AST_SEMICOLON, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_JUMP_STATEMENT,
        2,
        { AST_BREAK, AST_SEMICOLON, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_JUMP_STATEMENT,
        2,
        { AST_RETURN, AST_SEMICOLON, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_JUMP_STATEMENT,
        3,
        { AST_RETURN, AST_EXPRESSION, AST_SEMICOLON, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* expression: */
    {
        AST_EXPRESSION,
        3,
        { AST_EXPRESSION, AST_COMMA, AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_EXPRESSION,
        1,
        { AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* assignment-expression: */
    /* TODO: other assignemnt operators*/
    {
        AST_ASSIGNMENT_EXPRESSION,
        3,
        { AST_UNARY_EXPRESSION, AST_EQUAL, AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        3,
        { AST_UNARY_EXPRESSION, AST_ASTERISK_EQUAL, AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        3,
        { AST_UNARY_EXPRESSION, AST_BACKSLASH_EQUAL, AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        3,
        { AST_UNARY_EXPRESSION, AST_MOD_EQUAL, AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        3,
        { AST_UNARY_EXPRESSION, AST_PLUS_EQUAL, AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        3,
        { AST_UNARY_EXPRESSION, AST_MINUS_EQUAL, AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        1,
        { AST_CONDITIONAL_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* conditional-expression: */
    {
        AST_CONDITIONAL_EXPRESSION,
        5,
        { AST_LOGICAL_OR_EXPRESSION, AST_QUESTIONMARK, AST_EXPRESSION, AST_COLON, AST_CONDITIONAL_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_CONDITIONAL_EXPRESSION,
        1,
        { AST_LOGICAL_OR_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* logical-or-expression: */
    {
        AST_LOGICAL_OR_EXPRESSION,
        3,
        { AST_LOGICAL_OR_EXPRESSION, AST_VERTICALBAR_VERTICALBAR, AST_LOGICAL_AND_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_LOGICAL_OR_EXPRESSION,
        1,
        { AST_LOGICAL_AND_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* logical-and-expression: */
    {
        AST_LOGICAL_AND_EXPRESSION,
        3,
        { AST_LOGICAL_AND_EXPRESSION, AST_AMPERSAND_AMPERSAND, AST_INCLUSIVE_OR_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_LOGICAL_AND_EXPRESSION,
        1,
        { AST_INCLUSIVE_OR_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* inclusive-or-expression: */
    {
        AST_INCLUSIVE_OR_EXPRESSION,
        3,
        { AST_INCLUSIVE_OR_EXPRESSION, AST_VERTICALBAR, AST_EXCLUSIVE_OR_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_INCLUSIVE_OR_EXPRESSION,
        1,
        { AST_EXCLUSIVE_OR_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* exclusive-or-expression: */
    {
        AST_EXCLUSIVE_OR_EXPRESSION,
        3,
        { AST_EXCLUSIVE_OR_EXPRESSION, AST_CARET, AST_AND_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_EXCLUSIVE_OR_EXPRESSION,
        1,
        { AST_AND_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* and-expression: */
    {
        AST_AND_EXPRESSION,
        3,
        { AST_AND_EXPRESSION, AST_AMPERSAND, AST_EQUALITY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_AND_EXPRESSION,
        1,
        { AST_EQUALITY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* equality-expression: */
    {
        AST_EQUALITY_EXPRESSION,
        3,
        { AST_EQUALITY_EXPRESSION, AST_EQ, AST_RELATIONAL_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_EQUALITY_EXPRESSION,
        3,
        { AST_EQUALITY_EXPRESSION, AST_NEQ, AST_RELATIONAL_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_EQUALITY_EXPRESSION,
        1,
        { AST_RELATIONAL_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* relational-expression: */
    {
        AST_RELATIONAL_EXPRESSION,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_LT, AST_SHIFT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_GT, AST_SHIFT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_LTEQ, AST_SHIFT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_GTEQ, AST_SHIFT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        1,
        { AST_SHIFT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* shift-expression: */
    {
        AST_SHIFT_EXPRESSION,
        3,
        { AST_SHIFT_EXPRESSION, AST_SHIFTLEFT, AST_ADDITIVE_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_SHIFT_EXPRESSION,
        3,
        { AST_SHIFT_EXPRESSION, AST_SHIFTRIGHT, AST_ADDITIVE_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_SHIFT_EXPRESSION,
        1,
        { AST_ADDITIVE_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* additive-expression: */
    {
        AST_ADDITIVE_EXPRESSION,
        3,
        { AST_ADDITIVE_EXPRESSION, AST_PLUS, AST_MULTIPLICATIVE_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_ADDITIVE_EXPRESSION,
        3,
        { AST_ADDITIVE_EXPRESSION, AST_MINUS, AST_MULTIPLICATIVE_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_ADDITIVE_EXPRESSION,
        1,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* multplicative-expression: */
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        3,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_ASTERISK, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        3,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_BACKSLASH, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        3,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_MOD, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        1,
        { AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* cast-expression: */
    {
        AST_CAST_EXPRESSION,
        1,
        { AST_UNARY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* unary-expression: */
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_PLUS_PLUS, AST_UNARY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_MINUS_MINUS, AST_UNARY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_AMPERSAND, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_ASTERISK, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_PLUS, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_MINUS, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        1,
        { AST_POSTFIX_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* postfix-expression: */
    {
        AST_POSTFIX_EXPRESSION,
        3,
        { AST_POSTFIX_EXPRESSION, AST_ARROW, AST_IDENTIFIER, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_POSTFIX_EXPRESSION,
        2,
        { AST_POSTFIX_EXPRESSION, AST_PLUS_PLUS, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_POSTFIX_EXPRESSION,
        2,
        { AST_POSTFIX_EXPRESSION, AST_MINUS_MINUS, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_POSTFIX_EXPRESSION,
        1,
        { AST_PRIMARY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* primary-expression: */
    {
        AST_PRIMARY_EXPRESSION,
        1,
        { AST_CONSTANT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* constant: */
    {
        AST_CONSTANT,
        1,
        { AST_CONSTANT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
};

struct astnode *
shift(struct token *token)
{
    struct astnode *node;
    if (token->type == TOK_INTEGER)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_CONSTANT;
        node->constant = token;
    }
    else if (token->type == TOK_IDENTIFIER)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_PRIMARY_EXPRESSION;
        node->constant = token;
    }
    else if (token->type == TOK_PLUS)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_PLUS;
        node->constant = token;
    }
    else if (token->type == TOK_PLUS_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_PLUS_EQUAL;
        node->constant = token;
    }
    else if (token->type == TOK_MINUS)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MINUS;
        node->constant = token;
    }
    else if (token->type == TOK_MINUS_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MINUS_EQUAL;
        node->constant = token;
    }
    else if (token->type == TOK_AMPERSAND)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_AMPERSAND;
        node->constant = token;
    }
    else if (token->type == TOK_AMPERSAND_AMPERSAND)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_AMPERSAND_AMPERSAND;
        node->constant = token;
    }
    else if (token->type == TOK_ASTERISK)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_ASTERISK;
        node->constant = token;
    }
    else if (token->type == TOK_ASTERISK_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_ASTERISK_EQUAL;
        node->constant = token;
    }
    else if (token->type == TOK_BACKSLASH)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_BACKSLASH;
        node->constant = token;
    }
    else if (token->type == TOK_BACKSLASH_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_BACKSLASH_EQUAL;
        node->constant = token;
    }
    else if (token->type == TOK_CARET)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_CARET;
        node->constant = token;
    }
    else if (token->type == TOK_COMMA)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_COMMA;
        node->constant = token;
    }
    else if (token->type == TOK_MOD)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MOD;
        node->constant = token;
    }
    else if (token->type == TOK_MOD_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MOD_EQUAL;
        node->constant = token;
    }
    else if (token->type == TOK_QUESTIONMARK)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_QUESTIONMARK;
        node->constant = token;
    }
    else if (token->type == TOK_COLON)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_COLON;
        node->constant = token;
    }
    else if (token->type == TOK_SEMICOLON)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_SEMICOLON;
        node->constant = token;
    }
    else if (token->type == TOK_LPAREN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LPAREN;
        node->constant = token;
    }
    else if (token->type == TOK_RPAREN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_RPAREN;
        node->constant = token;
    }
    else if (token->type == TOK_LBRACKET)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LBRACKET;
        node->constant = token;
    }
    else if (token->type == TOK_RBRACKET)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_RBRACKET;
        node->constant = token;
    }
    else if (token->type == TOK_LBRACE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LBRACE;
        node->constant = token;
    }
    else if (token->type == TOK_RBRACE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_RBRACE;
        node->constant = token;
    }
    else if (token->type == TOK_VERTICALBAR)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_VERTICALBAR;
        node->constant = token;
    }
    else if (token->type == TOK_VERTICALBAR_VERTICALBAR)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_VERTICALBAR_VERTICALBAR;
        node->constant = token;
    }
    else if (token->type == TOK_SHIFTLEFT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_SHIFTLEFT;
        node->constant = token;
    }
    else if (token->type == TOK_SHIFTRIGHT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_SHIFTRIGHT;
        node->constant = token;
    }
    else if (token->type == TOK_LESSTHAN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LT;
        node->constant = token;
    }
    else if (token->type == TOK_GREATERTHAN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_GT;
        node->constant = token;
    }
    else if (token->type == TOK_LESSTHANEQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LTEQ;
        node->constant = token;
    }
    else if (token->type == TOK_GREATERTHANEQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_GTEQ;
        node->constant = token;
    }
    else if (token->type == TOK_EQ)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_EQ;
        node->constant = token;
    }
    else if (token->type == TOK_NEQ)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_NEQ;
        node->constant = token;
    }
    else if (token->type == TOK_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_EQUAL;
        node->constant = token;
    }
    else if (token->type == TOK_INT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_INT;
        node->constant = token;
    }
    else if (token->type == TOK_CHAR)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_CHAR;
        node->constant = token;
    }
    else if (token->type == TOK_GOTO)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_GOTO;
        node->constant = token;
    }
    else if (token->type == TOK_CONTINUE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_CONTINUE;
        node->constant = token;
    }
    else if (token->type == TOK_BREAK)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_BREAK;
        node->constant = token;
    }
    else if (token->type == TOK_RETURN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_RETURN;
        node->constant = token;
    }
    else if (token->type == TOK_FOR)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_FOR;
        node->constant = token;
    }
    else if (token->type == TOK_DO)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_DO;
        node->constant = token;
    }
    else if (token->type == TOK_WHILE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_WHILE;
        node->constant = token;
    }
    else if (token->type == TOK_IF)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_IF;
        node->constant = token;
    }
    else if (token->type == TOK_ELSE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_ELSE;
        node->constant = token;
    }
    else if (token->type == TOK_SWITCH)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_SWITCH;
        node->constant = token;
    }
    else if (token->type == TOK_CASE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_CASE;
        node->constant = token;
    }
    else if (token->type == TOK_DEFAULT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_DEFAULT;
        node->constant = token;
    }

    return node;
}

struct astnode *
reduce(struct listnode **stack)
{
    int i, j, k, skip;
    struct astnode *result, *inode;
    struct listnode *iterator;

    result = NULL;

    /*
     * Look through the grammar rules and check if astnode completes a rule.
     * Rule check is ordered from longest to shortest.
     */
    for (i=MAX_ASTNODES; i>=1; i--)
    {
        for (j=0; j<NUM_RULES-1; j++)
        {
            if (grammar[j].length_of_nodes != i)
            {
                continue;
            }

            /*
             * Peek through the stack and check if rules match
             */
            skip = 0;
            iterator = *stack;
            for (k=grammar[j].length_of_nodes-1; k>=0; k--)
            {
                if (iterator == NULL)
                {
                    skip = 1;
                    break;
                }

                inode = iterator->data;
                if (inode->type != grammar[j].nodes[k])
                {
                    skip = 1;
                    break;
                }
                iterator = iterator->next;
            }
            if (skip)
            {
                continue;
            }

            break;
        }

        if (!skip)
        {
            break;
        }
    }

    /* Create the new rule. */
    result = malloc(sizeof(struct astnode));
    result->type = grammar[j].type;
    list_init(&result->children);

    /* Remove reduced rules from the stack. */
    for (k=0; k<grammar[j].length_of_nodes; k++)
    {
        list_prepend(&result->children, (*stack)->data);
        *stack = (*stack)->next;
    }

    return result;
}

struct astnode *
do_parsing(struct listnode *tokens)
{
    struct astnode *result, *ast_current, *ast_next;
    struct listnode *current = tokens;
    struct listnode *stack;

    list_init(&stack);

    for(current = tokens; current != NULL; current = current->next)
    {
        ast_current = shift((struct token *)current->data);
        list_append(&stack, ast_current);

        ast_next = reduce(&stack);
        list_append(&stack, ast_next);
    }

    return result;
}

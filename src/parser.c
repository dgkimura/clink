#include <stdlib.h>

#include "parser.h"

#define NUM_RULES 6
#define MAX_ASTNODES 5

struct rule
{
    enum astnode_t type;
    int length;
    enum astnode_t nodes[MAX_ASTNODES];
};

struct rule grammer[NUM_RULES] =
{
    /* postfix-expression: */
    {
        AST_POSTFIX_EXPRESSION,
        1,
        { AST_PRIMARY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_POSTFIX_EXPRESSION,
        3,
        { AST_POSTFIX_EXPRESSION, AST_ARROW, AST_IDENTIFIER, AST_INVALID, AST_INVALID }
    },
    {
        AST_POSTFIX_EXPRESSION,
        2,
        { AST_POSTFIX_EXPRESSION, AST_PLUS_PLUS, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_POSTFIX_EXPRESSION,
        2,
        { AST_POSTFIX_EXPRESSION, AST_MINUS_MINUS, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* primary-expression: */
    {
        AST_PRIMARY_EXPRESSION,
        1,
        { AST_CONSTANT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* constant: */
    {
        AST_CONSTANT,
        1,
        { AST_CONSTANT, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
};

struct astnode *
do_parsing(struct listnode *tokens)
{
    struct astnode *result, *ast_current;
    struct token *tok_current;
    struct listnode *current = tokens;
    struct listnode *stack;

    list_init(&stack);

    for(current = tokens; current != NULL; current = current->next)
    {
        tok_current = (struct token *)current->data;
        if (tok_current->type == TOK_INTEGER)
        {
            ast_current = malloc(sizeof(struct astnode));
            ast_current->type = AST_CONSTANT;
            ast_current->constant = tok_current;
            list_append(&stack, ast_current);
        }
        if (tok_current->type == TOK_IDENTIFIER)
        {
            ast_current = malloc(sizeof(struct astnode));
            ast_current->type = AST_PRIMARY_EXPRESSION;
            ast_current->constant = tok_current;
            list_append(&stack, ast_current);
        }
    }

    return result;
}

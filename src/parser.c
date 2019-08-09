#include <stdlib.h>

#include "parser.h"

#define NUM_RULES 9
#define MAX_ASTNODES 5

struct rule
{
    enum astnode_t type;
    int length_of_nodes;
    enum astnode_t nodes[MAX_ASTNODES];
};

struct rule grammar[NUM_RULES] =
{
    /* unary-expression: */
    {
        AST_UNARY_EXPRESSION,
        1,
        { AST_POSTFIX_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_PLUS_PLUS, AST_UNARY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_MINUS_MINUS, AST_UNARY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID }
    },
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
    else if (token->type == TOK_MINUS)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MINUS;
        node->constant = token;
    }

    return node;
}

struct astnode *
reduce(struct astnode *node, struct listnode **stack)
{
    int i, j, skip;
    struct astnode *result, *inode;
    struct listnode *iterator;

    result = node;

    /*
     * Look through the grammar rules and check if astnode completes a rule.
     */
    for (i=0; i<NUM_RULES; i++)
    {
        if (grammar[i].nodes[grammar[i].length_of_nodes-1] != node->type)
        {
            /* It doesn't complete a rule. */
            continue;
        }

        /*
         * Peek through the stack and check if rules match
         */
        skip = 0;
        iterator = *stack;
        for (j=grammar[i].length_of_nodes-1; j>1; j--)
        {
            inode = iterator->data;
            if (inode->type == grammar[i].nodes[j])
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

        /* Create the new rule. */
        result = malloc(sizeof(struct astnode));
        result->type = grammar[i].type;
        list_init(&result->children);

        /* Remove reduced rules from the stack. */
        for (j=0; j<grammar[i].length_of_nodes; j++)
        {
            list_append(&result->children, *stack);
            *stack = (*stack)->next;
        }

        break;
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
        ast_next = reduce(ast_current, &stack);

        list_append(&stack, ast_next);
    }

    return result;
}

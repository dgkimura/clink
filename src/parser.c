#include <stdlib.h>

#include "parser.h"

#define NUM_RULES 24
#define MAX_ASTNODES 5

struct rule
{
    enum astnode_t type;
    int length_of_nodes;
    enum astnode_t nodes[MAX_ASTNODES];
};

struct rule grammar[NUM_RULES] =
{
    /* shift-expression: */
    {
        AST_SHIFT_EXPRESSION,
        3,
        { AST_SHIFT_EXPRESSION, AST_SHIFTLEFT, AST_ADDITIVE_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_SHIFT_EXPRESSION,
        3,
        { AST_SHIFT_EXPRESSION, AST_SHIFTRIGHT, AST_ADDITIVE_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_SHIFT_EXPRESSION,
        1,
        { AST_ADDITIVE_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* additive-expression: */
    {
        AST_ADDITIVE_EXPRESSION,
        3,
        { AST_ADDITIVE_EXPRESSION, AST_PLUS, AST_MULTIPLICATIVE_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_ADDITIVE_EXPRESSION,
        3,
        { AST_ADDITIVE_EXPRESSION, AST_MINUS, AST_MULTIPLICATIVE_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_ADDITIVE_EXPRESSION,
        1,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* multplicative-expression: */
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        3,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_ASTERISK, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        3,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_BACKSLASH, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        3,
        { AST_MULTIPLICATIVE_EXPRESSION, AST_MOD, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_MULTIPLICATIVE_EXPRESSION,
        1,
        { AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* cast-expression: */
    {
        AST_CAST_EXPRESSION,
        1,
        { AST_UNARY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* unary-expression: */
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
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_AMPERSAND, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_ASTERISK, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_PLUS, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        2,
        { AST_MINUS, AST_CAST_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    {
        AST_UNARY_EXPRESSION,
        1,
        { AST_POSTFIX_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* postfix-expression: */
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
    {
        AST_POSTFIX_EXPRESSION,
        1,
        { AST_PRIMARY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
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
    else if (token->type == TOK_AMPERSAND)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_AMPERSAND;
        node->constant = token;
    }
    else if (token->type == TOK_ASTERISK)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_ASTERISK;
        node->constant = token;
    }
    else if (token->type == TOK_BACKSLASH)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_BACKSLASH;
        node->constant = token;
    }
    else if (token->type == TOK_MOD)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MOD;
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

    return node;
}

struct astnode *
reduce(struct listnode **stack)
{
    int i, j, skip;
    struct astnode *result, *inode;
    struct listnode *iterator;

    result = NULL;

    /*
     * Look through the grammar rules and check if astnode completes a rule.
     */
    for (i=0; i<NUM_RULES; i++)
    {
        /*
         * Peek through the stack and check if rules match
         */
        skip = 0;
        iterator = *stack;
        for (j=grammar[i].length_of_nodes-1; j>=0; j--)
        {
            if (iterator == NULL)
            {
                skip = 1;
                break;
            }

            inode = iterator->data;
            if (inode->type != grammar[i].nodes[j])
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
            list_prepend(&result->children, (*stack)->data);
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
        list_append(&stack, ast_current);

        ast_next = reduce(&stack);
        list_append(&stack, ast_next);
    }

    return result;
}

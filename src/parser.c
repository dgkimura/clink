#include <stdlib.h>

#include "parser.h"

#define NUM_RULES 48
#define MAX_ASTNODES 5

struct rule
{
    enum astnode_t type;
    int length_of_nodes;
    enum astnode_t nodes[MAX_ASTNODES];
};

struct rule grammar[NUM_RULES] =
{
    /* conditional-expression: */
    {
        AST_CONDITIONAL_EXPRESSION,
        5,
        { AST_LOGICAL_OR_EXPRESSION, AST_QUESTIONMARK, AST_EXPRESSION, AST_COLON, AST_CONDITIONAL_EXPRESSION }
    },
    {
        AST_CONDITIONAL_EXPRESSION,
        1,
        { AST_LOGICAL_OR_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* assignment-expression: */
    /*
     * FIXME: scan rules longest to shortest to ignore order of grammar rules.
     *
     * XXX: assignment-expression must be after conditional-expression so that
     * a check whether AST_CONDITIONAL_EXPRESSION is part of a rule > size 1.
     * Otherwise reduce will first match it to AST_ASSIGNMENT_EXPRESSION.
     */
    /* TODO: other assignemnt operators*/
    {
        AST_ASSIGNMENT_EXPRESSION,
        3,
        { AST_UNARY_EXPRESSION, AST_EQUAL, AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_ASSIGNMENT_EXPRESSION,
        1,
        { AST_CONDITIONAL_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* expression: */
    /*
     * FIXME: scan rules longest to shortest to ignore order of grammar rules.
     *
     * XXX: expression must be after assignment-expression so that a check
     * whether AST_ASSIGNMENT_EXPRESSION is part of a rule > size 1.  Otherwise
     * reduce will first match it to AST_EXPRESSION.
     */
    {
        AST_EXPRESSION,
        3,
        { AST_EXPRESSION, AST_COMMA, AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_EXPRESSION,
        1,
        { AST_ASSIGNMENT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* logical-or-expression: */
    {
        AST_LOGICAL_OR_EXPRESSION,
        3,
        { AST_LOGICAL_OR_EXPRESSION, AST_VERTICALBAR_VERTICALBAR, AST_LOGICAL_AND_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_LOGICAL_OR_EXPRESSION,
        1,
        { AST_LOGICAL_AND_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* logical-and-expression: */
    {
        AST_LOGICAL_AND_EXPRESSION,
        3,
        { AST_LOGICAL_AND_EXPRESSION, AST_AMPERSAND_AMPERSAND, AST_INCLUSIVE_OR_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_LOGICAL_AND_EXPRESSION,
        1,
        { AST_INCLUSIVE_OR_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* inclusive-or-expression: */
    {
        AST_INCLUSIVE_OR_EXPRESSION,
        3,
        { AST_INCLUSIVE_OR_EXPRESSION, AST_VERTICALBAR, AST_EXCLUSIVE_OR_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_INCLUSIVE_OR_EXPRESSION,
        1,
        { AST_EXCLUSIVE_OR_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* exclusive-or-expression: */
    {
        AST_EXCLUSIVE_OR_EXPRESSION,
        3,
        { AST_EXCLUSIVE_OR_EXPRESSION, AST_CARET, AST_AND_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_EXCLUSIVE_OR_EXPRESSION,
        1,
        { AST_AND_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* and-expression: */
    {
        AST_AND_EXPRESSION,
        3,
        { AST_AND_EXPRESSION, AST_AMPERSAND, AST_EQUALITY_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_AND_EXPRESSION,
        1,
        { AST_EQUALITY_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* equality-expression: */
    {
        AST_EQUALITY_EXPRESSION,
        3,
        { AST_EQUALITY_EXPRESSION, AST_EQ, AST_RELATIONAL_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_EQUALITY_EXPRESSION,
        3,
        { AST_EQUALITY_EXPRESSION, AST_NEQ, AST_RELATIONAL_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_EQUALITY_EXPRESSION,
        1,
        { AST_RELATIONAL_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
    /* relational-expression: */
    {
        AST_RELATIONAL_EXPRESSION,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_LT, AST_SHIFT_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_GT, AST_SHIFT_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_LTEQ, AST_SHIFT_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        3,
        { AST_RELATIONAL_EXPRESSION, AST_GTEQ, AST_SHIFT_EXPRESSION, AST_INVALID, AST_INVALID }
    },
    {
        AST_RELATIONAL_EXPRESSION,
        1,
        { AST_SHIFT_EXPRESSION, AST_INVALID, AST_INVALID, AST_INVALID, AST_INVALID }
    },
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

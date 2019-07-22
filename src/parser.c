#include <stdlib.h>

#include "parser.h"

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
        if (tok_current->type == TOK_INTEGER ||
            tok_current->type == TOK_STRING)
        {
            ast_current = malloc(sizeof(struct astnode));
            ast_current->type = AST_CONSTANT;
            ast_current->constant = tok_current;
            list_append(&stack, ast_current);
        }
    }

    return result;
}

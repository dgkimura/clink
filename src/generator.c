#include <string.h>

#include "generator.h"
#include "utilities.h"

struct symbol
{
    struct token *identifier;
    enum token_t type;
};

int global_symbol_table_index = 0;
struct symbol global_symbol_table[8192];

int local_symbol_table_index = 0;
struct symbol local_symbol_table[8192];

static struct symbol *
find_symbol(const char *name)
{
    int i;
    struct symbol *symbol = NULL;

    for (i=0; i<global_symbol_table_index; i++)
    {
        if (strcmp(global_symbol_table[i].identifier->value, name) == 0)
        {
            symbol = &global_symbol_table[i];
            break;
        }
    }

    for (i=0; i<local_symbol_table_index; i++)
    {
        if (strcmp(local_symbol_table[i].identifier->value, name) == 0)
        {
            symbol = &local_symbol_table[i];
            break;
        }
    }

    return symbol;
}

struct astnode *
get_node(struct listnode *children, enum astnode_t type)
{
    struct listnode *next;

    next = children;
    for (next=children; next!=NULL; next=next->next)
    {
        if (((struct astnode *)(next->data))->type == type)
        {
            return (struct astnode *)(next->data);
        }
    }

    return NULL;
}

static void *
generate_code(struct astnode *ast)
{
    if (ast->type == AST_CONSTANT)
    {
        return ast->token;
    }

    return NULL;
}

static void
generate_symbol_table(struct astnode *ast)
{
    struct token *token;
    enum token_t type;

    if (ast->type == AST_STRUCT_OR_UNION_SPECIFIER)
    {
        token = get_node(ast->children, AST_IDENTIFIER)->token;
        type = get_node(ast->children, AST_STRUCT_OR_UNION)->token->type;

        if (token == NULL)
        {
            /* anonymous struct or union */
            return;
        }

        global_symbol_table[global_symbol_table_index].identifier = token;
        global_symbol_table[global_symbol_table_index].type = type;

        global_symbol_table_index += 1;
    }
}

void
generate(struct astnode *ast)
{
    generate_symbol_table(ast);
    generate_code(ast);
}

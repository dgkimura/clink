#include <assert.h>
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

static struct astnode *
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

static void
generate_symbol_table(struct astnode *ast)
{
    struct astnode *node;
    struct token *token;
    enum token_t type;

    if (ast == NULL)
    {
        return;
    }

    if (ast->type == AST_STRUCT_OR_UNION_SPECIFIER)
    {
        node = get_node(ast->children, AST_IDENTIFIER);
        type = get_node(ast->children, AST_STRUCT_OR_UNION)->token->type;

        if (node == NULL)
        {
            /* anonymous struct or union */
            return;
        }

        global_symbol_table[global_symbol_table_index].identifier = node->token;
        global_symbol_table[global_symbol_table_index].type = type;

        global_symbol_table_index += 1;
    }
    if (ast->type == AST_FUNCTION_DEFINITION)
    {
        /* iterate AST_PARAMETER_TYPE_LIST insert into local symbol table */
        /* iterate AST_DECLARATION_LIST insert into local symbol table */
        node = get_node(ast->children, AST_DECLARATION_LIST);
    }
}

static void
visit_translation_unit(struct astnode *ast)
{
    assert(ast->type == AST_TRANSLATION_UNIT);
}

void
generate(struct astnode *ast)
{
    visit_translation_unit(ast);
}

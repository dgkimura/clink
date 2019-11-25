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
visit_declaration(struct astnode *ast)
{
    assert(ast->type == AST_DECLARATION);

    /* add to local symbol table */
}

static void
visit_function_definition(struct astnode *ast)
{
    assert(ast->type == AST_FUNCTION_DEFINITION);

    /* add to local symbol table */
}

static void
visit_external_declaration(struct astnode *ast)
{
    /* add to global symbol table */

    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_EXTERNAL_DECLARATION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_FUNCTION_DEFINITION:
            {
                visit_function_definition(next);
                break;
            }
            case AST_DECLARATION:
            {
                visit_declaration(next);
                break;
            }
            default:
            {
                assert(1);
                break;
            }
        }
    }
}

static void
visit_translation_unit(struct astnode *ast)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_TRANSLATION_UNIT);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (((struct astnode *)list->data)->type)
        {
            case AST_TRANSLATION_UNIT:
            {
                visit_translation_unit(next);
                break;
            }
            case AST_EXTERNAL_DECLARATION:
            {
                visit_external_declaration(next);
                break;
            }
            default:
            {
                assert(1);
                break;
            }
        }
    }
}

void
generate(struct astnode *ast)
{
    visit_translation_unit(ast);
}

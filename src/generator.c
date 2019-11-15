#include <string.h>

#include "generator.h"
#include "utilities.h"

struct symbol
{
    char *name;
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
        if (strcmp(global_symbol_table[i].name, name) == 0)
        {
            symbol = &global_symbol_table[i];
            break;
        }
    }

    for (i=0; i<local_symbol_table_index; i++)
    {
        if (strcmp(local_symbol_table[i].name, name) == 0)
        {
            symbol = &local_symbol_table[i];
            break;
        }
    }

    return symbol;
}

static void *
generate_code(struct astnode *ast)
{
    if (ast->type == AST_CONSTANT)
    {
        return ast->constant;
    }

    return NULL;
}

static void
generate_symbol_table(struct astnode *ast)
{
}

void
generate(struct astnode *ast)
{
    generate_symbol_table(ast);
    generate_code(ast);
}

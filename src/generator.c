#include "generator.h"
#include "utilities.h"

struct symbol
{
    char *name;
};

struct symbol symbol_table[8192];

void *
generate(struct astnode *ast)
{
    if (ast->type == AST_CONSTANT)
    {
        return ast->constant;
    }

    return NULL;
}

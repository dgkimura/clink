#include "generator.h"
#include "utilities.h"

struct listnode *symbol_table;

void *
generate(struct astnode *ast)
{
    if (ast->type == AST_CONSTANT)
    {
        return ast->constant;
    }

    return NULL;
}

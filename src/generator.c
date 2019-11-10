#include "generator.h"
#include "utilities.h"

struct symbol
{
    char *name;
};

int symbol_table_index = 0;
struct symbol symbol_table[8192];

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

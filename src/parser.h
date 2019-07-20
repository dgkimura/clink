#include "scanner.h"

enum astnode_t
{
    AST_CONSTANT
};

struct astnode
{
    enum token_t type;
};

struct astnode *
do_parsing(struct listnode *tokens);

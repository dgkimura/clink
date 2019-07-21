#include "scanner.h"

enum astnode_t
{
    AST_CONSTANT
};

struct astnode
{
    enum astnode_t type;
    union
    {
        struct
        {
            struct token *constant;
        };
    };
};

struct astnode *
do_parsing(struct listnode *tokens);

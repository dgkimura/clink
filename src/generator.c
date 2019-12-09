#include <assert.h>
#include <string.h>

#include "generator.h"
#include "utilities.h"

struct symbol
{
    struct token *identifier;
    enum token_t type;
};

enum scope
{
    LOCAL,
    GLOBAL
};

int global_symbol_table_index = 0;
struct symbol global_symbol_table[8192];

int local_symbol_table_index = 0;
struct symbol local_symbol_table[8192];

static void visit_declaration_specifiers(struct astnode *ast);
static void visit_declarator(struct astnode *ast, enum scope scope);
static void visit_statement(struct astnode *ast);
static void visit_statement_list(struct astnode *ast);

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
visit_init_declarator_list(struct astnode *ast)
{
    assert(ast->type == AST_INIT_DECLARATOR_LIST);
}

static void
visit_declaration(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_DECLARATION);

    /* add to local symbol table */

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_DECLARATION_SPECIFIERS:
            {
                visit_declaration_specifiers(ast);
                break;
            }
            case AST_INIT_DECLARATOR_LIST:
            {
                visit_init_declarator_list(ast);
                break;
            }
            case AST_SEMICOLON:
            {
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
visit_declaration_specifiers(struct astnode *ast)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_DECLARATION_SPECIFIERS);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_STORAGE_CLASS_SPECIFIER:
            {
                /*
                 * next->next->type:
                 *     AST_AUTO|AST_REGISTER|AST_STATIC|AST_EXTERN|AST_TYPEDEF
                 */
                break;
            }
            case AST_TYPE_SPECIFIER:
            {
                /*
                 * next->next->type:
                 *     AST_CHAR|AST_SHORT|AST_INT|AST_LONG|AST_FLOAT|AST_DOUBLE|
                 *     AST_SIGNED|AST_UNSIGNED|AST_STRUCT_OR_UNION_SPECIFIER|
                 *     AST_ENUM_SPECIFIER|AST_TYPEDEF_NAME
                 */
                break;
            }
            case AST_TYPE_QUALIFIER:
            {
                /*
                 * next->next->type:
                 *     AST_CONST|AST_VOLATILE
                 */
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
visit_declarator(struct astnode *ast, enum scope scope)
{
    assert(ast->type == AST_DECLARATOR);
}

static void
visit_declaration_list(struct astnode *ast)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_DECLARATION_LIST);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_DECLARATION:
            {
                visit_declaration(next, LOCAL);
                break;
            }
            case AST_DECLARATION_LIST:
            {
                visit_declaration_list(next);
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
visit_labeled_statement(struct astnode *ast)
{
    assert(ast->type == AST_LABELED_STATEMENT);
}

static void
visit_expression_statement(struct astnode *ast)
{
    assert(ast->type == AST_EXPRESSION_STATEMENT);
}

static void
visit_compound_statement(struct astnode *ast)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_COMPOUND_STATEMENT);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_LBRACE:
            {
                break;
            }
            case AST_RBRACE:
            {
                break;
            }
            case AST_DECLARATION_LIST:
            {
                visit_declaration_list(next);
                break;
            }
            case AST_STATEMENT_LIST:
            {
                visit_statement_list(next);
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
visit_selection_statement(struct astnode *ast)
{
    assert(ast->type == AST_SELECTION_STATEMENT);
}

static void
visit_iteration_statement(struct astnode *ast)
{
    assert(ast->type == AST_ITERATION_STATEMENT);
}

static void
visit_jump_statement(struct astnode *ast)
{
    assert(ast->type == AST_JUMP_STATEMENT);
}

static void
visit_statement(struct astnode *ast)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_STATEMENT);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_LABELED_STATEMENT:
            {
                visit_labeled_statement(next);
                break;
            }
            case AST_EXPRESSION_STATEMENT:
            {
                visit_expression_statement(next);
                break;
            }
            case AST_COMPOUND_STATEMENT:
            {
                visit_compound_statement(next);
                break;
            }
            case AST_SELECTION_STATEMENT:
            {
                visit_selection_statement(next);
                break;
            }
            case AST_ITERATION_STATEMENT:
            {
                visit_iteration_statement(next);
                break;
            }
            case AST_JUMP_STATEMENT:
            {
                visit_jump_statement(next);
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
visit_statement_list(struct astnode *ast)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_STATEMENT_LIST);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_STATEMENT:
            {
                visit_statement(next);
                break;
            }
            case AST_STATEMENT_LIST:
            {
                visit_statement_list(next);
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
visit_function_definition(struct astnode *ast)
{
    /* add to local symbol table */

    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_FUNCTION_DEFINITION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_DECLARATION_SPECIFIERS:
            {
                visit_declaration_specifiers(next);
                break;
            }
            case AST_DECLARATOR:
            {
                visit_declarator(next, GLOBAL);
                break;
            }
            case AST_DECLARATION_LIST:
            {
                visit_declaration_list(next);
                break;
            }
            case AST_COMPOUND_STATEMENT:
            {
                visit_compound_statement(next);
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
                visit_declaration(next, GLOBAL);
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

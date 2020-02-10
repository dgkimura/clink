#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "generator.h"
#include "utilities.h"

#define MAX_SYMBOL_ATTRIBUTES 10

struct symbol
{
    /* String identifier of the symbol.. */
    char *identifier;

    /* Attributes of the symbol. */
    enum token_t attributes[MAX_SYMBOL_ATTRIBUTES];
};

/*
 * Temporarily holds the attributes of a symbol during AST iteration when name
 * of symbol(s) is not yet known.
 */
int index_current_symbol_attributes;
enum token_t current_symbol_attributes[MAX_SYMBOL_ATTRIBUTES];

enum scope
{
    LOCAL,
    GLOBAL
};

int global_symbol_table_index = 0;
struct symbol global_symbol_table[8192];

int local_symbol_table_index = 0;
struct symbol local_symbol_table[8192];

static void visit_declaration_specifiers(struct astnode *ast, enum scope scope);
static void visit_declarator(struct astnode *ast, enum scope scope);
static void visit_statement(struct astnode *ast);
static void visit_statement_list(struct astnode *ast);
static void visit_constant_expression(struct astnode *ast, enum scope scope);

static void
insert_symbol(char *name, enum scope scope)
{
    struct symbol *s = malloc(sizeof(struct symbol));
    memset(s, 0, sizeof(struct symbol));

    s->identifier = name;
    memcpy(s->attributes, current_symbol_attributes, MAX_SYMBOL_ATTRIBUTES);

    if (scope == LOCAL)
    {
        local_symbol_table[local_symbol_table_index++] = *s;
    }
    else if (scope == GLOBAL)
    {
        global_symbol_table[global_symbol_table_index++] = *s;
    }
}

static struct symbol *
find_symbol(const char *name)
{
    int i;
    struct symbol *symbol = NULL;

    for (i=0; i<global_symbol_table_index; i++)
    {
        if (strcmp(global_symbol_table[i].identifier, name) == 0)
        {
            symbol = &global_symbol_table[i];
            break;
        }
    }

    for (i=0; i<local_symbol_table_index; i++)
    {
        if (strcmp(local_symbol_table[i].identifier, name) == 0)
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
visit_initializer(struct astnode *ast, enum scope scope)
{
    assert(ast->type == AST_INITIALIZER);
}

static void
visit_init_declarator(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_INIT_DECLARATOR);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_DECLARATOR:
            {
                visit_declarator(next, scope);
                break;
            }
            case AST_EQUAL:
            {
                break;
            }
            case AST_INITIALIZER:
            {
                visit_initializer(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_init_declarator_list(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_INIT_DECLARATOR_LIST);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_INIT_DECLARATOR:
            {
                visit_init_declarator(next, scope);
                break;
            }
            case AST_COMMA:
            {
                break;
            }
            case AST_INIT_DECLARATOR_LIST:
            {
                visit_init_declarator_list(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_declaration(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_DECLARATION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_DECLARATION_SPECIFIERS:
            {
                visit_declaration_specifiers(next, scope);
                break;
            }
            case AST_INIT_DECLARATOR_LIST:
            {
                /*
                 * If this is a named declaration then it will parse an
                 * identifer inside the init declarator list
                 */
                visit_init_declarator_list(next, scope);
                break;
            }
            case AST_SEMICOLON:
            {
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_storage_class_specifier(struct astnode *ast, enum scope scope)
{
    assert(ast->type == AST_STORAGE_CLASS_SPECIFIER);

    switch (ast->type)
    {
        case AST_AUTO:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_AUTO;
            break;
        }
        case AST_REGISTER:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_REGISTER;
            break;
        }
        case AST_STATIC:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_STATIC;
            break;
        }
        case AST_EXTERN:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_EXTERN;
            break;
        }
        case AST_TYPEDEF:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_TYPEDEF;
            break;
        }
        default:
        {
            assert(0);
            break;
        }
    }
}

static void
visit_struct_or_union_specifier(struct astnode *ast, enum scope scope)
{
    assert(ast->type == AST_STRUCT_OR_UNION_SPECIFIER);
}

static void
visit_enumerator(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_ENUMERATOR);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_IDENTIFIER:
            {
                insert_symbol(next->token->value, scope);
                break;
            }
            case AST_EQUAL:
            {
                break;
            }
            case AST_CONSTANT_EXPRESSION:
            {
                visit_constant_expression(next, scope);
                /* TODO: find_symbol and add constant */
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_enumerator_list(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_ENUMERATOR_LIST);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_ENUMERATOR:
            {
                visit_enumerator(next, scope);
                break;
            }
            case AST_COMMA:
            {
                break;
            }
            case AST_ENUMERATOR_LIST:
            {
                visit_enumerator_list(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_enum_specifier(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_ENUM_SPECIFIER);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_ENUM:
            {
                current_symbol_attributes[index_current_symbol_attributes++] = TOK_ENUM;
                break;
            }
            case AST_IDENTIFIER:
            {
                insert_symbol(next->token->value, scope);
                break;
            }
            case AST_LBRACE:
            {
                break;
            }
            case AST_ENUMERATOR_LIST:
            {
                visit_enumerator_list(next, scope);
                break;
            }
            case AST_RBRACE:
            {
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_type_specifier(struct astnode *ast, enum scope scope)
{
    assert(ast->type == AST_TYPE_SPECIFIER);

    switch (ast->type)
    {
        case AST_CHAR:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_CHAR;
            break;
        }
        case AST_SHORT:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_SHORT;
            break;
        }
        case AST_INT:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_INT;
            break;
        }
        case AST_LONG:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_LONG;
            break;
        }
        case AST_FLOAT:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_FLOAT;
            break;
        }
        case AST_DOUBLE:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_DOUBLE;
            break;
        }
        case AST_SIGNED:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_SIGNED;
            break;
        }
        case AST_UNSIGNED:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_UNSIGNED;
            break;
        }
        case AST_STRUCT_OR_UNION_SPECIFIER:
        {
            visit_struct_or_union_specifier(ast, scope);
            break;
        }
        case AST_ENUM_SPECIFIER:
        {
            visit_enum_specifier(ast, scope);
            break;
        }
        case AST_TYPEDEF_NAME:
        {
            /* TODO: What is this? */
            break;
        }
        default:
        {
            assert(0);
            break;
        }
    }
}

static void
visit_type_qualifier(struct astnode *ast, enum scope scope)
{
    assert(ast->type == AST_TYPE_QUALIFIER);

    switch (ast->type)
    {
        case AST_CONST:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_CONST;
            break;
        }
        case AST_VOLATILE:
        {
            current_symbol_attributes[index_current_symbol_attributes++] = TOK_VOLATILE;
            break;
        }
        default:
        {
            assert(0);
            break;
        }
    }
}

static void
visit_declaration_specifiers(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_DECLARATION_SPECIFIERS);

    index_current_symbol_attributes = 0;
    memset(current_symbol_attributes, 0, sizeof(enum token_t) * MAX_SYMBOL_ATTRIBUTES);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_STORAGE_CLASS_SPECIFIER:
            {
                visit_storage_class_specifier(next, scope);
                break;
            }
            case AST_TYPE_SPECIFIER:
            {
                visit_type_specifier(next, scope);
                break;
            }
            case AST_TYPE_QUALIFIER:
            {
                visit_type_qualifier(next, scope);
                break;
            }
            case AST_DECLARATION_SPECIFIERS:
            {
                /*
                 * Recursive when declaration has multiple specifiers.
                 */
                visit_declaration_specifiers(ast, scope);
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_additive_expression(struct astnode *ast, enum scope scope)
{
    assert(ast->type == AST_ADDITIVE_EXPRESSION);
}

static void
visit_shift_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_SHIFT_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_SHIFT_EXPRESSION:
            {
                visit_shift_expression(next, scope);
                break;
            }
            case AST_SHIFTLEFT:
            case AST_SHIFTRIGHT:
            {
                break;
            }
            case AST_ADDITIVE_EXPRESSION:
            {
                visit_additive_expression(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_relational_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_RELATIONAL_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_RELATIONAL_EXPRESSION:
            {
                visit_relational_expression(next, scope);
                break;
            }
            case AST_LT:
            case AST_GT:
            case AST_LTEQ:
            case AST_GTEQ:
            {
                break;
            }
            case AST_SHIFT_EXPRESSION:
            {
                visit_shift_expression(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_equality_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_EQUALITY_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_EQUALITY_EXPRESSION:
            {
                visit_equality_expression(next, scope);
                break;
            }
            case AST_NEQ:
            case AST_EQ:
            {
                break;
            }
            case AST_RELATIONAL_EXPRESSION:
            {
                visit_relational_expression(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_and_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_AND_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_AND_EXPRESSION:
            {
                visit_and_expression(next, scope);
                break;
            }
            case AST_AMPERSAND:
            {
                break;
            }
            case AST_EQUALITY_EXPRESSION:
            {
                visit_equality_expression(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_exclusive_or_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_EXCLUSIVE_OR_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_EXCLUSIVE_OR_EXPRESSION:
            {
                visit_exclusive_or_expression(next, scope);
                break;
            }
            case AST_CARET:
            {
                break;
            }
            case AST_AND_EXPRESSION:
            {
                visit_and_expression(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_inclusive_or_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_INCLUSIVE_OR_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_INCLUSIVE_OR_EXPRESSION:
            {
                visit_inclusive_or_expression(next, scope);
                break;
            }
            case AST_VERTICALBAR:
            {
                break;
            }
            case AST_EXCLUSIVE_OR_EXPRESSION:
            {
                visit_exclusive_or_expression(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_logical_and_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_LOGICAL_AND_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_LOGICAL_AND_EXPRESSION:
            {
                visit_logical_and_expression(next, scope);
                break;
            }
            case AST_AMPERSAND_AMPERSAND:
            {
                break;
            }
            case AST_INCLUSIVE_OR_EXPRESSION:
            {
                visit_inclusive_or_expression(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_logical_or_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_LOGICAL_OR_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_LOGICAL_OR_EXPRESSION:
            {
                visit_logical_or_expression(next, scope);
                break;
            }
            case AST_VERTICALBAR_VERTICALBAR:
            {
                break;
            }
            case AST_LOGICAL_AND_EXPRESSION:
            {
                visit_logical_and_expression(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_expression(struct astnode *ast, enum scope scope)
{
    assert(ast->type == AST_EXPRESSION);
}

static void
visit_conditional_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_CONDITIONAL_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_LOGICAL_OR_EXPRESSION:
            {
                visit_logical_or_expression(next, scope);
                break;
            }
            case AST_QUESTIONMARK:
            {
                break;
            }
            case AST_EXPRESSION:
            {
                visit_expression(next, scope);
                break;
            }
            case AST_COLON:
            {
                break;
            }
            case AST_CONDITIONAL_EXPRESSION:
            {
                visit_conditional_expression(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_constant_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_CONSTANT_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_CONDITIONAL_EXPRESSION:
            {
                visit_conditional_expression(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_parameter_declaration(struct astnode *ast, enum scope scope)
{
    assert(ast->type == AST_PARAMETER_DECLARATION);
}

static void
visit_parameter_list(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_PARAMETER_LIST);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_PARAMETER_DECLARATION:
            {
                visit_parameter_declaration(next, scope);
                break;
            }
            case AST_COMMA:
            {
                break;
            }
            case AST_PARAMETER_LIST:
            {
                visit_parameter_list(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_parameter_type_list(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_PARAMETER_TYPE_LIST);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_PARAMETER_LIST:
            {
                visit_parameter_list(next, scope);
                break;
            }
            case AST_COMMA:
            {
                break;
            }
            case AST_ELLIPSIS:
            {
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_identifier_list(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_IDENTIFIER_LIST);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_IDENTIFIER_LIST:
            {
                visit_identifier_list(next, scope);
                break;
            }
            case AST_COMMA:
            {
                break;
            }
            case AST_IDENTIFIER:
            {
                insert_symbol(next->token->value, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_direct_declarator(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_DIRECT_DECLARATOR);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_IDENTIFIER:
            {
                insert_symbol(next->token->value, scope);
                break;
            }
            case AST_LPAREN:
            {
                break;
            }
            case AST_DECLARATOR:
            {
                visit_declarator(next, scope);
                break;
            }
            case AST_RPAREN:
            {
                break;
            }
            case AST_DIRECT_DECLARATOR:
            {
                visit_direct_declarator(next, scope);
                break;
            }
            case AST_LBRACKET:
            {
                break;
            }
            case AST_RBRACKET:
            {
                break;
            }
            case AST_CONSTANT_EXPRESSION:
            {
                visit_constant_expression(next, scope);
                break;
            }
            case AST_PARAMETER_TYPE_LIST:
            {
                visit_parameter_type_list(next, scope);
                break;
            }
            case AST_IDENTIFIER_LIST:
            {
                visit_identifier_list(next, scope);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
}

static void
visit_declarator(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_DECLARATOR);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_DIRECT_DECLARATOR:
            {
                visit_direct_declarator(next, LOCAL);
                break;
            }
            case AST_POINTER:
            {
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }
    }
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
                assert(0);
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
                assert(0);
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
                assert(0);
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
                assert(0);
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
                visit_declaration_specifiers(next, GLOBAL);
                break;
            }
            case AST_DECLARATOR:
            {
                /*
                 * If this is a named function then it will parse an identifer
                 * inside the declarator
                 */
                visit_declarator(next, GLOBAL);
                break;
            }
            case AST_DECLARATION_LIST:
            {
                /*
                 * If there are identifiers in the declaration list then add
                 * them to the local symbol table. Those symbols can then later
                 * be referenced in the compound statements.
                 */
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
                assert(0);
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
                assert(0);
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
                assert(0);
                break;
            }
        }
    }
}

/*
 * Given an abstract syntax tree, construct symbol tables and generate code.
 */
void
generate(struct astnode *ast)
{
    visit_translation_unit(ast);
}

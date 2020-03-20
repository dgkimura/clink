#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "generator.h"
#include "utilities.h"

#define MAX_SYMBOL_ATTRIBUTES 10

struct symbol
{
    /* String identifier of the symbol.. */
    char *identifier;

    /* Attributes of the symbol. */
    enum token_t attributes[MAX_SYMBOL_ATTRIBUTES];

    /*
     * Offset of the symbol. For parameters and local variables this will be an
     * offset from ebp.
     */
    int offset;
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
static void visit_cast_expression(struct astnode *ast, enum scope scope);

static FILE *assembly_filename;

static void
write_assembly(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(assembly_filename, format, args);
    va_end(args);
    fprintf(assembly_filename, "\n");
}

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
    int i;
    struct astnode *next;

    assert(ast->elided_type == AST_DECLARATION);

    for (i=0; i<ast->declarators_size; i++)
    {
        next = ast->declarators[i];
        /* TODO: Write the location of the declaration */

        if (ast->type_specifiers & INT)
        {
            write_assembly("_%s:", next->declarator_identifier);
            write_assembly(".long %d", next->declarator_value);
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
visit_constant(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_CONSTANT);

    switch (((struct astnode *)ast->children->data)->type)
    {
            case AST_INTEGER_CONSTANT:
            {
                write_assembly("mov $%d %rax", ast->int_value);
                break;
            }
            case AST_CHARACTER_CONSTANT:
            {
                /* TODO: */
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
visit_primary_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_PRIMARY_EXPRESSION);

    switch (((struct astnode *)ast->children->data)->type)
    {
            case AST_IDENTIFIER:
            {
                /* TODO: find_symbol() */
                break;
            }
            case AST_CONSTANT:
            {
                visit_constant((struct astnode *)ast->children->data, scope);
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
visit_postfix_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_POSTFIX_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_POSTFIX_EXPRESSION:
            {
                visit_postfix_expression(next, scope);
                break;
            }
            case AST_ARROW:
            case AST_PLUS_PLUS:
            case AST_MINUS_MINUS:
            {
                break;
            }
            case AST_IDENTIFIER:
            {
                /* TODO: find_symbol() */
                break;
            }
            case AST_PRIMARY_EXPRESSION:
            {
                visit_primary_expression(next, scope);
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
visit_unary_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_UNARY_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_PLUS_PLUS:
            case AST_MINUS_MINUS:
            {
                break;
            }
            case AST_UNARY_EXPRESSION:
            {
                visit_unary_expression(next, scope);
                break;
            }
            case AST_AMPERSAND:
            case AST_ASTERISK:
            case AST_PLUS:
            case AST_MINUS:
            {
                break;
            }
            case AST_CAST_EXPRESSION:
            {
                visit_cast_expression(next, scope);
                break;
            }
            case AST_POSTFIX_EXPRESSION:
            {
                visit_postfix_expression(next, scope);
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
visit_cast_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_CAST_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_UNARY_EXPRESSION:
            {
                visit_unary_expression(next, scope);
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
visit_multiplicative_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_MULTIPLICATIVE_EXPRESSION);

    for (list=ast->children; list!=NULL; list=list->next)
    {
        next = (struct astnode *)list->data;
        switch (next->type)
        {
            case AST_MULTIPLICATIVE_EXPRESSION:
            {
                visit_multiplicative_expression(next, scope);
                break;
            }
            case AST_ASTERISK:
            case AST_BACKSLASH:
            case AST_MOD:
            {
                break;
            }
            case AST_CAST_EXPRESSION:
            {
                visit_cast_expression(next, scope);
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
visit_additive_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->type == AST_ADDITIVE_EXPRESSION);

    switch (ast->op)
    {
        case AST_MINUS:
        case AST_PLUS:
        {
            visit_additive_expression(ast->left, scope);
            write_assembly("push %rax");
            visit_multiplicative_expression(ast->right, scope);
            write_assembly("mov %rax %rcx");
            write_assembly("pop %rax");
            if (ast->op == AST_PLUS)
            {
                write_assembly("add %rcx %rax");
            }
            else if (ast->op == AST_MINUS)
            {
                write_assembly("sub %rcx %rax");
            }
            break;
        }
        default:
        {
            visit_multiplicative_expression(next, scope);
            break;
        }
    }
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

    int i;
    struct listnode *list;
    struct astnode *declarator, *declaration, *parameter;

    assert(ast->type == AST_FUNCTION_DEFINITION);

    declarator = ast->function_declarator;
    declaration = ast->function_declarator->declarators[0];

    for (i=0; i<declaration->declarator_parameter_type_list_size; i++)
    {
        parameter = declarator->declarators[0]->declarator_parameter_type_list[i];

        /*
         * Add up the size of all parameters and push onto the stack
         */
    }
}

static void
visit_translation_unit(struct astnode *ast)
{
    int i;
    struct astnode *next;

    assert(ast->elided_type == AST_TRANSLATION_UNIT);

    for (i=0; i<ast->translation_unit_items_size; i++)
    {
        next = ast->translation_unit_items[i];
        switch (next->elided_type)
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

/*
 * Given an abstract syntax tree, construct symbol tables and generate code.
 */
void
generate(struct astnode *ast, char *outfile)
{
    assembly_filename = fopen(outfile, "w");
    visit_translation_unit(ast);
}

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
        else if (ast->type_specifiers & CHAR)
        {
            write_assembly("_%s:", next->declarator_identifier);
            write_assembly(".byte %d", next->declarator_value);
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
visit_multiplicative_expression(struct astnode *ast, enum scope scope)
{
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
visit_expression(struct astnode *ast)
{
    if (ast->type == AST_ADDITIVE_EXPRESSION)
    {
        visit_additive_expression(ast, LOCAL);
    }
}

static void
visit_function_definition(struct astnode *ast)
{
    /* add to local symbol table */

    int i;
    struct listnode *list;
    struct astnode *declarator, *declaration, *statement, *parameter;

    assert(ast->elided_type == AST_FUNCTION_DEFINITION);

    declarator = ast->function_declarator;
    declaration = ast->function_declarator->declarator_parameter_type_list;

    /*
     * Function prologue
     */
    write_assembly(".text");
    write_assembly(".global _%s:", declarator->declarator_identifier);
    write_assembly("  movq %%esp %%ebp");

    for (i=0; declaration && i<declaration->parameter_type_list_size; i++)
    {
        parameter = declaration->parameter_type_list[i];

        /*
         * Add up the size of all parameters and push onto the stack
         */
    }

    for (i=0; i<ast->statement_list_size; i++)
    {
        statement = ast->statement_list[i];

        /*
         * Iterate over the statements
         */
        visit_expression(statement);
    }

    write_assembly("  retq");
}

static void
visit_translation_unit(struct astnode *ast)
{
    int i;
    struct astnode *next;

    assert(ast->type == AST_TRANSLATION_UNIT);

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

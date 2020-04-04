#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "generator.h"
#include "utilities.h"

enum scope
{
    LOCAL,
    GLOBAL
};

static FILE *assembly_filename;

static void visit_expression(struct astnode *ast, enum scope scope);

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

    switch (ast->elided_type)
    {
            case AST_INTEGER_CONSTANT:
            {
                write_assembly("  mov $%d, %%rax", ast->int_value);
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
visit_arithmetic_expression(struct astnode *ast, enum scope scope)
{
    struct listnode *list;
    struct astnode *next;

    assert(ast->elided_type == AST_ADDITIVE_EXPRESSION ||
           ast->elided_type == AST_MULTIPLICATIVE_EXPRESSION );

    visit_expression(ast->left, scope);
    write_assembly("  push %%rax");
    visit_expression(ast->right, scope);
    write_assembly("  mov %%rax, %%rcx");
    write_assembly("  pop %%rax");

    switch (ast->op)
    {
        case AST_MINUS:
        {
            write_assembly("  sub %%rcx, %%rax");
            break;
        }
        case AST_PLUS:
        {
            write_assembly("  add %%rcx, %%rax");
            break;
        }
        case AST_ASTERISK:
        {
            write_assembly("  imul %%rcx, %%rax");
            break;
        }
        default:
        {
            break;
        }
    }
}

static void
visit_expression(struct astnode *ast, enum scope scope)
{
    switch (ast->elided_type)
    {
        case AST_ADDITIVE_EXPRESSION:
        case AST_MULTIPLICATIVE_EXPRESSION:
        {
            visit_arithmetic_expression(ast, LOCAL);
            break;
        }
        case AST_INTEGER_CONSTANT:
        {
            visit_constant(ast, LOCAL);
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
visit_function_definition(struct astnode *ast)
{
    /* add to local symbol table */

    int i;
    struct listnode *list;
    struct astnode *declarator, *declaration, *statement, *parameter;
    struct ast_compound_statement *compound;

    assert(ast->elided_type == AST_FUNCTION_DEFINITION);

    declarator = ast->function_declarator;
    declaration = ast->function_declarator->declarator_parameter_type_list;
    compound = ast->statements;

    /*
     * Function prologue
     */
    write_assembly(".text");
    write_assembly("  .global _%s", declarator->declarator_identifier);
    write_assembly("_%s:", declarator->declarator_identifier);
    write_assembly("  movq %%rsp, %%rbp");

    for (i=0; declaration && i<declaration->parameter_type_list_size; i++)
    {
        parameter = declaration->parameter_type_list[i];

        /*
         * Add up the size of all parameters and push onto the stack
         */
    }

    /*
     * TODO: Reserve stack space for local variables in this function so that
     * if this function calls another function it will not clobber this
     * functions local variables on the stack.
     */

    for (i=0; compound->statements && i<compound->statements->size; i++)
    {
        statement = compound->statements->items[i];

        /*
         * Iterate over the statements
         */
        visit_expression(statement, LOCAL);
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

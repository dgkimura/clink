#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "generator.h"
#include "parser.h"
#include "utilities.h"

enum scope
{
    LOCAL,
    GLOBAL
};

static FILE *assembly_filename;

static void visit_expression(struct astnode *ast, enum scope scope);

static char *
function_register(int argnum)
{
    char *registers[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
    assert(argnum < 6);

    return registers[argnum];
}

static int
size_of_type(int type_specifiers)
{
    int size = 0;
    if (type_specifiers | AST_INT)
    {
        size = 4;
    }
    else if (type_specifiers | AST_SHORT)
    {
        size = 2;
    }
    else if (type_specifiers | AST_CHAR)
    {
        size = 1;
    }
    else
    {
        assert(0);
    }
    return size;
}

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
visit_declaration(struct ast_declaration *ast, enum scope scope)
{
    int i;
    struct ast_declarator *next;

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
        case AST_POSTFIX_EXPRESSION:
        {
            /* TODO function call. */
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
visit_function_definition(struct ast_function *ast)
{
    /* add to local symbol table */

    int i, local_size;
    struct listnode *list;
    struct astnode *statement;
    struct ast_declarator *declarator;
    struct ast_declaration *parameter;
    struct ast_declaration *declaration;
    struct ast_compound_statement *compound;
    struct ast_parameter_type_list *parameters;

    assert(ast->elided_type == AST_FUNCTION_DEFINITION);

    declarator = ast->function_declarator;
    parameters = ast->function_declarator->declarator_parameter_type_list;
    compound = ast->statements;

    /*
     * Function prologue
     */
    write_assembly(".text");
    write_assembly("  .global _%s", declarator->declarator_identifier);
    write_assembly("_%s:", declarator->declarator_identifier);
    write_assembly("  push %%rbp");
    write_assembly("  movq %%rsp, %%rbp");

    /* Begin at 4 as old ebp push is on the stack. */
    local_size = 4;
    for (i=0; parameters && i<parameters->size; i++)
    {
        parameter = parameters->items[i];

        /*
         * Add up the size of all parameters and push onto the stack
         */
        write_assembly("  movl %%%s, -%d(%%rbp)",
                function_register(i),
                local_size);
        local_size += size_of_type(parameter->type_specifiers);
    }

    /*
     * Reserve stack space for local variables in this function so that if this
     * function calls another function it will not clobber this functions local
     * variables on the stack.
     */
    local_size = 0;
    for (i=0; compound->declarations && i<compound->declarations->size; i++)
    {
        declaration = compound->declarations->items[i];
        local_size += (declaration->declarators_size *
                      size_of_type(declaration->type_specifiers));
    }
    write_assembly("  sub $%d, %%rsp", local_size);

    for (i=0; compound->statements && i<compound->statements->size; i++)
    {
        statement = compound->statements->items[i];

        /*
         * Iterate over the statements
         */
        visit_expression(statement, LOCAL);
    }

    /*
     * Return registers and stack to state before called.
     */
    write_assembly("  add $%d, %%rsp", local_size);
    write_assembly("  popq %%rbp");
    write_assembly("  retq");
}

static void
visit_translation_unit(struct ast_translation_unit *ast)
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
                visit_function_definition((struct ast_function *)next);
                break;
            }
            case AST_DECLARATION:
            {
                visit_declaration((struct ast_declaration *)next, GLOBAL);
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
    visit_translation_unit((struct ast_translation_unit *)ast);
}

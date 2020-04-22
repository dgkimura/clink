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

static void visit_expression(struct astnode *ast,
                             struct ast_parameter_type_list *parameters,
                             struct ast_declaration_list *declarations);
static int
identifier_offset(char *identifier,
                  struct ast_parameter_type_list *parameters,
                  struct ast_declaration_list *declarations);

static char *
get_32bit_register(int argnum)
{
    char *registers[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
    assert(argnum < 6);

    return registers[argnum];
}

static char *
get_64bit_register(int argnum)
{
    char *registers[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
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

static int
align8(int size)
{
    return size + ((size % 8 == 0) ?  0 : 8 - (size % 8));
}

static int
align16(int size)
{
    return size + ((size % 16 == 0) ?  0 : 16 - (size % 16));
}

/*
 * cursor and string_literal_buffer are used as a buffer to store that will
 * later be appended to the end of the assembly file.
 */
static int cursor = 0;
static char string_literal_buffer[512];

static char *
create_string_literal(char *string)
{
    static int i = 0;
    int j;
    char *label;

    label = malloc(sizeof(char) * 16);
    snprintf(label, 16, "L.str.%d", i);
    cursor += snprintf(string_literal_buffer + cursor, 512 - cursor, "%s:\n", label);
    strcpy(string_literal_buffer + cursor, "  .asciz ");
    cursor += strlen("  .asciz ");

    string_literal_buffer[cursor++] = '"';
    for (j=0; j<strlen(string); j++)
    {
        string_literal_buffer[cursor++] = string[j];
        if (string[j] == '%')
        {
            string_literal_buffer[cursor++] = '%';
        }
    }
    string_literal_buffer[cursor++] = '"';
    string_literal_buffer[cursor++] = '\n';

    i += 1;
    return label;
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
visit_constant(struct ast_expression *ast, enum scope scope)
{
    switch (ast->elided_type)
    {
            case AST_INTEGER_CONSTANT:
            {
                write_assembly("  mov $%d, %%eax", ast->int_value);
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
visit_arithmetic_expression(struct astnode *ast,
                            struct ast_parameter_type_list *parameters,
                            struct ast_declaration_list *declarations)
{
    assert(ast->elided_type == AST_ADDITIVE_EXPRESSION ||
           ast->elided_type == AST_MULTIPLICATIVE_EXPRESSION);

    visit_expression(ast->left, parameters, declarations);
    write_assembly("  push %%rax");
    visit_expression(ast->right, parameters, declarations);
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
visit_function_call(struct ast_expression *ast,
                    struct ast_parameter_type_list *parameters,
                    struct ast_declaration_list *declarations)
{
    int i, j;
    struct ast_expression *argument;

    for (i=0; i<ast->arguments_size; i++)
    {
        argument = ast->arguments[i];

        if (ast->arguments[i]->kind == INT_VALUE)
        {
            write_assembly("  mov $%d, %%%s", ast->arguments[i]->int_value,
                get_32bit_register(i));
        }
        else if (ast->arguments[i]->kind == STRING_VALUE)
        {
            write_assembly("  leaq %s(%%rip), %%%s",
                create_string_literal(ast->arguments[i]->identifier),
                get_64bit_register(i));
        }
        else if (ast->arguments[i]->kind == IDENTIFIER_VALUE)
        {
            /*
             * FIXME: Identifier could be a function paramter passed through
             * register, not on the stack. In that case we need to copy the old
             * register in to the new callees register.
             */
            write_assembly("  mov -%d(%%rbp), %%%s",
                identifier_offset(argument->identifier, parameters, declarations),
                get_32bit_register(i));
        }
        else if (ast->arguments[i]->kind == FUNCTION_VALUE)
        {
            /*
             * Save registers that have updated. Since we are about to perform
             * another function call we may need the registers.
             */
            for (j=0; j<i; j++)
            {
                write_assembly("  push %%%s", get_64bit_register(j));
            }
            visit_function_call(ast->arguments[i], parameters, declarations);

            /*
             * Re-apply registers. Since registers are stored on the stack they
             * need to be pop'd in the opposite order that they were push'd.
             */
            for (j=i-1; j>=0; j--)
            {
                write_assembly("  pop %%%s", get_64bit_register(j));
            }

            write_assembly("  mov %%eax, %%%s", get_32bit_register(i));
        }
    }
    write_assembly("  call _%s", ast->identifier);
}

static void
visit_identifier(struct ast_expression *ast,
                 struct ast_parameter_type_list *parameters,
                 struct ast_declaration_list *declarations)
{
    int i, offset;
    struct ast_declaration *parameter, *declaration;

    for (i=0; parameters && i<parameters->size; i++)
    {
        parameter = parameters->items[i];

        if (strcmp(ast->identifier, parameter->declarators[0]->declarator_identifier) == 0)
        {
            write_assembly("  movl %%%s, %%eax", get_32bit_register(i));
            return;
        }
    }

    offset = 0;
    for (i=0; i<declarations->size; i++)
    {
        declaration = declarations->items[i];
        offset += size_of_type(declaration->type_specifiers);
        offset = align8(offset);

        if (strcmp(ast->identifier, declaration->declarators[0]->declarator_identifier) == 0)
        {
            write_assembly("  mov -%d(%%rbp), %%eax", offset);
            return;
        }
    }
}

static void
visit_selection_statement(struct ast_selection_statement *ast,
                            struct ast_parameter_type_list *parameters,
                            struct ast_declaration_list *declarations)
{
    /*
     * Use 'i' to generate and keep track of a unique label
     */
    static int i = 0;

    visit_expression(ast->expression, parameters, declarations);

    switch (ast->expression->op)
    {
        case AST_EQ:
        {
            write_assembly("  jne L_ELSE_%d", i);
            break;
        }
        case AST_VERTICALBAR_VERTICALBAR:
        {
            visit_expression(ast->expression->left, parameters, declarations);
            write_assembly("  je L_IF_%d", i);
            visit_expression(ast->expression->right, parameters, declarations);
            write_assembly("  jne L_ELSE_%d", i);
            break;
        }
        case AST_AMPERSAND_AMPERSAND:
        {
            visit_expression(ast->expression->left, parameters, declarations);
            write_assembly("  jne L_ELSE_%d", i);
            visit_expression(ast->expression->right, parameters, declarations);
            write_assembly("  jne L_ELSE_%d", i);
            break;
        }
        default:
        {
            assert(0);
            break;
        }
    }

    /*
     * if block statements
     */
    write_assembly("L_IF_%d:", i);
    visit_expression(ast->statement1, parameters, declarations);
    write_assembly("  jmp L_DONE_%d", i);

    write_assembly("L_ELSE_%d:", i);

    if (ast->statement2)
    {
        /*
         * else block statements
         */
        visit_expression(ast->statement2, parameters, declarations);
    }

    write_assembly("L_DONE_%d:", i++);
}

static void
visit_equality_expression(struct astnode *ast,
                          struct ast_parameter_type_list *parameters,
                          struct ast_declaration_list *declarations)
{
    visit_expression(ast->left, parameters, declarations);
    write_assembly("  push %%rax");
    visit_expression(ast->right, parameters, declarations);
    write_assembly("  mov %%rax, %%rcx");
    write_assembly("  pop %%rax");

    switch (ast->op)
    {
        case AST_EQ:
        {
            write_assembly("  cmpl %%ecx, %%eax");
            break;
        }
        default:
        {
            break;
        }
    }
}

static int
identifier_offset(char *identifier,
                  struct ast_parameter_type_list *parameters,
                  struct ast_declaration_list *declarations)
{
    int i, offset = 0;
    struct ast_declaration *declaration;

    offset = 0;
    for (i=0; i<declarations->size; i++)
    {
        declaration = declarations->items[i];
        offset += size_of_type(declaration->type_specifiers);
        offset = align8(offset);

        if (strcmp(identifier,
                   declaration->declarators[0]->declarator_identifier) == 0)
        {
            break;
        }
    }

    return offset;
}

static void
visit_assignment_expression(struct astnode *ast,
                            struct ast_parameter_type_list *parameters,
                            struct ast_declaration_list *declarations)
{
    int i, offset;

    /*
     * FIXME: Need to evaluate ast->left to find location. Should not
     *        assume it is a simple identifier node. It may be part of a
     *        struct..
     */
    offset = identifier_offset(((struct ast_expression *)ast->left)->identifier,
                               parameters, declarations);

    visit_expression(ast->right, parameters, declarations);

    switch (ast->op)
    {
        case AST_EQUAL:
        {
            write_assembly("  mov %%eax, -%d(%%rbp)", offset);
            break;
        }
        case AST_PLUS_EQUAL:
        {
            write_assembly("  mov %%eax, %%ecx");
            write_assembly("  mov -%d(%%rbp), %%eax", offset);
            write_assembly("  add %%ecx, %%eax");
            write_assembly("  mov %%eax, -%d(%%rbp)", offset);
            break;
        }
        case AST_MINUS_EQUAL:
        {
            write_assembly("  mov %%eax, %%ecx");
            write_assembly("  mov -%d(%%rbp), %%eax", offset);
            write_assembly("  sub %%ecx, %%eax");
            write_assembly("  mov %%eax, -%d(%%rbp)", offset);
            break;
        }
        case AST_ASTERISK_EQUAL:
        {
            write_assembly("  mov %%eax, %%ecx");
            write_assembly("  mov -%d(%%rbp), %%eax", offset);
            write_assembly("  imul %%ecx, %%eax");
            write_assembly("  mov %%eax, -%d(%%rbp)", offset);
            break;
        }
        default:
        {
            break;
        }
    }
}

static void
visit_expression(struct astnode *ast,
                 struct ast_parameter_type_list *parameters,
                 struct ast_declaration_list *declarations)
{
    int i;

    switch (ast->elided_type)
    {
        case AST_ADDITIVE_EXPRESSION:
        case AST_MULTIPLICATIVE_EXPRESSION:
        {
            visit_arithmetic_expression(ast, parameters, declarations);
            break;
        }
        case AST_INTEGER_CONSTANT:
        {
            visit_constant((struct ast_expression *)ast, LOCAL);
            break;
        }
        case AST_PRIMARY_EXPRESSION:
        case AST_POSTFIX_EXPRESSION:
        {
            if (((struct ast_expression *)ast)->kind == FUNCTION_VALUE)
            {
                visit_function_call((struct ast_expression *)ast, parameters, declarations);
            }
            else
            {
                visit_identifier((struct ast_expression *)ast, parameters, declarations);
            }
            break;
        }
        case AST_SELECTION_STATEMENT:
        {
            struct ast_selection_statement *statement = (struct ast_selection_statement *)ast;
            visit_selection_statement(statement, parameters, declarations);
            break;
        }
        case AST_LOGICAL_OR_EXPRESSION:
        case AST_LOGICAL_AND_EXPRESSION:
        case AST_EQUALITY_EXPRESSION:
        {
            visit_equality_expression(ast, parameters, declarations);
            break;
        }
        case AST_ASSIGNMENT_EXPRESSION:
        {
            visit_assignment_expression(ast, parameters, declarations);
            break;
        }
        case AST_COMPOUND_STATEMENT:
        {
            struct ast_compound_statement *compound = (struct ast_compound_statement *)ast;

            for (i=0; i<compound->statements->size; i++)
            {
                visit_expression(compound->statements->items[i], parameters, declarations);
            }
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

    /*
     * Begin local_size 4 as old ebp push is on the stack.
     *
     * NOTE: System-V AMD64 ABI specifies in section 3.2.3 that the 6 function
     * arguments are passed through registers. The remainder is pushed on the
     * stack.
     */
    local_size = 4;
    for (i=6; parameters && i<parameters->size; i++)
    {
        parameter = parameters->items[i];

        /*
         * Add up the size of all parameters and push onto the stack
         */
        write_assembly("  movl %%%s, -%d(%%rbp)",
                get_32bit_register(i),
                local_size);
        local_size += size_of_type(parameter->type_specifiers);
    }

    /*
     * Reserve stack space for local variables in this function so that if this
     * function calls another function it will not clobber this functions local
     * variables on the stack.
     *
     * NOTE: System-V AMD64 ABI mandates in section 3.2.2 that the stack frame
     * must be 16 bytes aligned.
     */
    for (i=0; compound->declarations && i<compound->declarations->size; i++)
    {
        declaration = compound->declarations->items[i];
        local_size += (declaration->declarators_size *
                      size_of_type(declaration->type_specifiers));
    }
    write_assembly("  subq $%d, %%rsp", align16(local_size));

    for (i=0; compound->statements && i<compound->statements->size; i++)
    {
        statement = compound->statements->items[i];

        /*
         * Iterate over the statements
         */
        visit_expression(statement, parameters, compound->declarations);
    }

    /*
     * Return registers and stack to state before called.
     */
    write_assembly("  addq $%d, %%rsp", align16(local_size));
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

    write_assembly(string_literal_buffer);
}

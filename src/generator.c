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
identifier_offset(struct ast_expression *ast,
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
visit_arithmetic_expression(struct ast_binary_op *ast,
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
            assert(0);
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
        else if (ast->arguments[i]->kind == PTR_VALUE)
        {
            write_assembly("  mov -%d(%%rbp), %%rax",
                           identifier_offset(ast->arguments[i],
                                             parameters, declarations));
            write_assembly("  mov (%%rax), %%%s", get_32bit_register(i));
        }
        else if (ast->arguments[i]->kind == STRING_VALUE)
        {
            write_assembly("  leaq %s(%%rip), %%%s",
                create_string_literal(ast->arguments[i]->identifier),
                get_64bit_register(i));
        }
        else if (ast->arguments[i]->kind == IDENTIFIER_VALUE)
        {
            visit_expression((struct astnode *)ast->arguments[i], parameters,
                             declarations);
            write_assembly("  mov %%eax, %%%s", get_32bit_register(i));
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
        else if (argument->elided_type == AST_ADDITIVE_EXPRESSION)
        {
            /*
             * Save registers that have updated. Since we are about to perform
             * another function call we may need the registers.
             */
            for (j=0; j<i; j++)
            {
                write_assembly("  push %%%s", get_64bit_register(j));
            }
            visit_expression((struct astnode *)argument, parameters,
                             declarations);

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
        else
        {
            assert(0);
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
    char location[25];

    memset(location, 0, sizeof(location));

    for (i=0; parameters && i<parameters->size; i++)
    {
        parameter = parameters->items[i];

        if (strcmp(ast->identifier, parameter->declarators[0]->declarator_identifier) == 0)
        {
            snprintf(location, sizeof(location), "%%%s", get_32bit_register(i));
            write_assembly("  movl %%%s, %%eax", get_32bit_register(i));
            goto done;
        }
    }

    offset = identifier_offset(ast, parameters, declarations);
    for (i=0; i<declarations->size; i++)
    {
        if (ast->kind == PTR_VALUE)
        {
            snprintf(location, sizeof(location), "-%d(%%rbp)", offset);
            write_assembly("  leaq %s, %%rax", location);
        }
        else
        {
            if (ast->extra)
            {
                visit_expression((struct astnode *)ast->extra, parameters, declarations);
                write_assembly("  mov %%rax, %%rcx");
                write_assembly("  leaq -%d(%%rbp), %%rdx", offset);
                write_assembly("  movq (%%rdx, %%rcx, %d), %%rax",
                               size_of_type(declarations->items[i]->type_specifiers));
            }
            else
            {
                snprintf(location, sizeof(location), "-%d(%%rbp)", offset);
                write_assembly("  movq %s, %%rax", location);
            }
        }
        goto done;
    }

done:
    switch (ast->inplace_op)
    {
        /*
         * Post-increment/decrement saves the original value, then increments
         * or decrements current value and saves the updated value into the
         * local location on the stack (offset of 'rbp' frame pointer), and
         * finally restores original value back into rax..
         */
        case POST_INCREMENT:
        {
            write_assembly("  push %%rax");
            write_assembly("  add $1, %%eax", location);
            write_assembly("  mov %%eax, %s", location);
            write_assembly("  pop %%rax");
            break;
        }
        case POST_DECREMENT:
        {
            write_assembly("  push %%rax");
            write_assembly("  sub $1, %%eax", location);
            write_assembly("  mov %%eax, %s", location);
            write_assembly("  pop %%rax");
            break;
        }
        case PRE_INCREMENT:
        {
            write_assembly("  add $1, %%eax", location);
            write_assembly("  mov %%eax, %s", location);
            break;
        }
        case PRE_DECREMENT:
        {
            write_assembly("  sub $1, %%eax", location);
            write_assembly("  mov %%eax, %s", location);
            break;
        }
        case NO_OP:
        {
            break;
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
    char label[25];

    visit_expression((struct astnode *)ast->expression, parameters, declarations);
    write_assembly("  cmpl $1, %%eax");
    write_assembly("  jne L_ELSE_%d", i);

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
visit_equality_expression(struct ast_binary_op *ast,
                          struct ast_parameter_type_list *parameters,
                          struct ast_declaration_list *declarations)
{
    static int i = 0;
    int ilocal = i++;

    visit_expression(ast->left, parameters, declarations);
    write_assembly("  push %%rax");
    visit_expression(ast->right, parameters, declarations);
    write_assembly("  mov %%rax, %%rcx");
    write_assembly("  pop %%rax");

    if (ast->op == AST_EQ)
    {
        write_assembly("  cmpl %%ecx, %%eax");
        write_assembly("  je L_EQ_%d", ilocal);
        write_assembly("  mov $0, %%eax");
        write_assembly("  jmp L_EQ_DONE_%d", ilocal);
        write_assembly("L_EQ_%d:", ilocal);
        write_assembly("  mov $1, %%eax");
        write_assembly("L_EQ_DONE_%d:", ilocal);
    }
    else if (ast->op == AST_LTEQ)
    {
        write_assembly("  cmpl %%ecx, %%eax");
        write_assembly("  jle L_EQ_%d", ilocal);
        write_assembly("  mov $0, %%eax");
        write_assembly("  jmp L_EQ_DONE_%d", ilocal);
        write_assembly("L_EQ_%d:", ilocal);
        write_assembly("  mov $1, %%eax");
        write_assembly("L_EQ_DONE_%d:", ilocal);
    }
    else if (ast->op == AST_LT)
    {
        write_assembly("  cmpl %%ecx, %%eax");
        write_assembly("  jl L_EQ_%d", ilocal);
        write_assembly("  mov $0, %%eax");
        write_assembly("  jmp L_EQ_DONE_%d", ilocal);
        write_assembly("L_EQ_%d:", ilocal);
        write_assembly("  mov $1, %%eax");
        write_assembly("L_EQ_DONE_%d:", ilocal);
    }
    else if (ast->op == AST_AMPERSAND_AMPERSAND)
    {
        write_assembly("  cmpl $0, %%eax");
        write_assembly("  je L_NEQ_%d", ilocal);
        write_assembly("  cmpl $0, %%ecx");
        write_assembly("  je L_NEQ_%d", ilocal);
        write_assembly("  mov $1, %%eax");
        write_assembly("  jmp L_EQ_DONE_%d", ilocal);
        write_assembly("L_NEQ_%d:", ilocal);
        write_assembly("  mov $0, %%eax");
        write_assembly("L_EQ_DONE_%d:", ilocal);
    }
    else if (ast->op == AST_VERTICALBAR_VERTICALBAR)
    {
        write_assembly("  cmpl $1, %%eax");
        write_assembly("  je L_EQ_%d", ilocal);
        write_assembly("  cmpl $1, %%ecx");
        write_assembly("  je L_EQ_%d", ilocal);
        write_assembly("  mov $0, %%eax");
        write_assembly("  jmp L_EQ_DONE_%d", ilocal);
        write_assembly("L_EQ_%d:", ilocal);
        write_assembly("  mov $1, %%eax");
        write_assembly("L_EQ_DONE_%d:", ilocal);
    }
    else
    {
        assert(0);
    }
}

static int
identifier_offset(struct ast_expression *ast,
                  struct ast_parameter_type_list *parameters,
                  struct ast_declaration_list *declarations)
{
    int i, j, local_size, offset = 0;
    struct ast_declaration *declaration;

    /*
     * Caculate start of local variables offset from block pointer.
     *
     * Local variables are offset from the frame pointer (rbp) based on
     * declaration position in the function. In the following function, 'i' is
     * declared first so it is immediately below the frame pointer in the
     * stack.
     *
     * ```
     * void f()
     * {
     *     int i;
     *     int a[3];
     * }
     * ```
     *
     * In this case 'i' would be at  4 off frame pointer and 'a' would be 16
     * off frame pointer (16 == 4 + 3 * 4).
     *
     *  rbp -->     ---------   High memory
     *             |   ret   |
     *              ---------
     *             |   i     |
     *  rbp-4  ->   ---------
     *             |         |
     *             |   a     |
     *             |         |
     *  rbp-16 ->   ---------   Low memory (top of stack)
     */
    offset = 4;
    for (i=0; i<declarations->size; i++)
    {
        declaration = declarations->items[i];

        for (j=0; j<declaration->declarators_size; j++)
        {
            offset += (declaration->declarators[j]->count *
                       size_of_type(declaration->type_specifiers));
        }

        if (strcmp(ast->identifier,
                   declaration->declarators[0]->declarator_identifier) == 0)
        {
            break;
        }
        /*
         * FIXME: Why is this necessary?
         */
        offset += 8;
    }

    return offset;
}

static void
visit_assignment_expression(struct ast_binary_op *ast,
                            struct ast_parameter_type_list *parameters,
                            struct ast_declaration_list *declarations)
{
    int offset;

    /*
     * FIXME: Need to evaluate ast->left to find location. Should not
     *        assume it is a simple identifier node. It may be part of a
     *        struct..
     */
    offset = identifier_offset((struct ast_expression *)ast->left,
                               parameters, declarations);

    visit_expression(ast->right, parameters, declarations);

    switch (ast->op)
    {
        case AST_EQUAL:
        {
            if (((struct ast_expression *)ast->left)->extra != NULL)
            {
                write_assembly("  push %%rax");
                visit_expression((struct astnode *)((struct ast_expression *)ast->left)->extra, parameters, declarations);
                write_assembly("  mov %%rax, %%rdi");
                write_assembly("  lea -%d(%%rbp), %%rdx", offset);
                write_assembly("  pop %%rax");
                write_assembly("  movl %%eax, (%%rdx, %%rdi, 4)"); /* FIXME: hardcode 4 declaration->type_specifiers */
            }
            else
            {
                write_assembly("  mov %%rax, -%d(%%rbp)", offset);
            }
            break;
        }
        case AST_PLUS_EQUAL:
        {
            if (((struct ast_expression *)ast->left)->extra != NULL)
            {
                write_assembly("  push %%rax");
                visit_expression((struct astnode *)((struct ast_expression *)ast->left)->extra, parameters, declarations);
                write_assembly("  mov %%rax, %%rdi");
                write_assembly("  lea -%d(%%rbp), %%rdx", offset);
                write_assembly("  mov (%%rdx, %%rdi, 4), %%rcx"); /* FIXME: hardcode 4 declaration->type_specifiers */
                write_assembly("  pop %%rax");
                write_assembly("  add %%ecx, %%eax");
                write_assembly("  movl %%eax, (%%rdx, %%rdi, 4)"); /* FIXME: hardcode 4 declaration->type_specifiers */
            }
            else
            {
                write_assembly("  mov %%eax, %%ecx");
                write_assembly("  mov -%d(%%rbp), %%eax", offset);
                write_assembly("  add %%ecx, %%eax");
                write_assembly("  mov %%eax, -%d(%%rbp)", offset);
            }
            break;
        }
        case AST_MINUS_EQUAL:
        {
            if (((struct ast_expression *)ast->left)->extra != NULL)
            {
                write_assembly("  push %%rax");
                visit_expression((struct astnode *)((struct ast_expression *)ast->left)->extra, parameters, declarations);
                write_assembly("  mov %%rax, %%rdi");
                write_assembly("  lea -%d(%%rbp), %%rdx", offset);
                write_assembly("  mov (%%rdx, %%rdi, 4), %%rcx"); /* FIXME: hardcode 4 declaration->type_specifiers */
                write_assembly("  pop %%rax");
                write_assembly("  sub %%eax, %%ecx");
                write_assembly("  movl %%ecx, (%%rdx, %%rdi, 4)"); /* FIXME: hardcode 4 declaration->type_specifiers */
            }
            else
            {
                write_assembly("  mov %%eax, %%ecx");
                write_assembly("  mov -%d(%%rbp), %%eax", offset);
                write_assembly("  sub %%ecx, %%eax");
                write_assembly("  mov %%eax, -%d(%%rbp)", offset);
            }
            break;
        }
        case AST_ASTERISK_EQUAL:
        {
            if (((struct ast_expression *)ast->left)->extra != NULL)
            {
                write_assembly("  push %%rax");
                visit_expression((struct astnode *)((struct ast_expression *)ast->left)->extra, parameters, declarations);
                write_assembly("  mov %%rax, %%rdi");
                write_assembly("  lea -%d(%%rbp), %%rdx", offset);
                write_assembly("  mov (%%rdx, %%rdi, 4), %%rcx"); /* FIXME: hardcode 4 declaration->type_specifiers */
                write_assembly("  pop %%rax");
                write_assembly("  imul %%eax, %%ecx");
                write_assembly("  movl %%ecx, (%%rdx, %%rdi, 4)"); /* FIXME: hardcode 4 declaration->type_specifiers */
            }
            else
            {
                write_assembly("  mov %%eax, %%ecx");
                write_assembly("  mov -%d(%%rbp), %%eax", offset);
                write_assembly("  imul %%ecx, %%eax");
                write_assembly("  mov %%eax, -%d(%%rbp)", offset);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

static void
visit_iteration_statement(struct ast_iteration_statement *ast,
                          struct ast_parameter_type_list *parameters,
                          struct ast_declaration_list *declarations)
{
    /*
     * Use 'i' to generate and keep track of a unique label
     */
    static int i = 0;
    int ilocal = i++;

    visit_expression(ast->expression1, parameters, declarations);
    write_assembly("L_FOR_BEGIN_%d:", ilocal);

    visit_expression(ast->expression2, parameters, declarations);
    write_assembly("  cmpl $1, %%eax");
    write_assembly("  jne L_FOR_END_%d", ilocal);

    visit_expression(ast->statement, parameters, declarations);
    visit_expression(ast->expression3, parameters, declarations);

    write_assembly("  jmp L_FOR_BEGIN_%d", ilocal);

    write_assembly("L_FOR_END_%d:", ilocal);
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
            visit_arithmetic_expression((struct ast_binary_op *)ast,
                                        parameters, declarations);
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
                visit_function_call((struct ast_expression *)ast, parameters,
                                    declarations);
            }
            else
            {
                visit_identifier((struct ast_expression *)ast, parameters,
                                 declarations);
            }
            break;
        }
        case AST_SELECTION_STATEMENT:
        {
            struct ast_selection_statement *statement =
                (struct ast_selection_statement *)ast;
            visit_selection_statement(statement, parameters, declarations);
            break;
        }
        case AST_LOGICAL_OR_EXPRESSION:
        case AST_LOGICAL_AND_EXPRESSION:
        case AST_EQUALITY_EXPRESSION:
        case AST_RELATIONAL_EXPRESSION:
        {
            visit_equality_expression((struct ast_binary_op *)ast, parameters,
                                      declarations);
            break;
        }
        case AST_ASSIGNMENT_EXPRESSION:
        {
            visit_assignment_expression((struct ast_binary_op *)ast, parameters,
                                        declarations);
            break;
        }
        case AST_ITERATION_STATEMENT:
        {
            visit_iteration_statement((struct ast_iteration_statement *)ast,
                                      parameters, declarations);
            break;
        }
        case AST_COMPOUND_STATEMENT:
        {
            struct ast_compound_statement *compound =
                (struct ast_compound_statement *)ast;

            for (i=0; i<compound->statements->size; i++)
            {
                visit_expression(compound->statements->items[i], parameters,
                                 declarations);
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

    int i, j;
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
    write_assembly("  subq $4, %%rsp");
    /*
     * TODO: This is not implemented yet. Revisit when functions support more
     *       than 6 arguments.
    local_size = 4;
    for (i=6; parameters && i<parameters->size; i++)
    {
        parameter = parameters->items[i];

        write_assembly("  movl %%%s, -%d(%%rbp)",
                get_32bit_register(i),
                local_size);
        local_size += size_of_type(parameter->type_specifiers);
    }
    */

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
        for (j=0; j<declaration->declarators_size; j++)
        {
            write_assembly("  subq $%d, %%rsp",
                    declaration->declarators[j]->count *
                    size_of_type(declaration->type_specifiers));
        }
    }
    write_assembly("  andq $0xFFFFFFFFFFFFFFF0, %%rsp");

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
    write_assembly("  movq %%rbp, %%rsp");
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

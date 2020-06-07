#ifndef __AST_H__
#define __AST_H__

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "parser.h"
#include "utilities.h"

/*
 * Storage Specifier flags
 */
#define AUTO        0x01
#define REGISTER    0x02
#define STATIC      0x04
#define EXTERN      0x08
#define TYPEDEF     0x10

/*
 * Type Specifier flags
 */
#define VOID                        0x001
#define CHAR                        0x002
#define SHORT                       0x004
#define INT                         0x008
#define LONG                        0x010
#define FLOAT                       0x020
#define DOUBLE                      0x040
#define SIGNED                      0x080
#define UNSIGNED                    0x100
#define STRUCT_OR_UNION_SPECIFIER   0x200
#define ENUM_SPECIFIER              0x400
#define TYPEDEF_NAME                0x800

/*
 * Type Qualifier flags
 */
#define CONST       0x1
#define VOLATILE    0x2

/*
 * Following structs are nodes in the abstract syntax tree.
 */

struct ast_expression
{
    enum astnode_t type;
    enum astnode_t elided_type;

    int int_value;
    char *identifier;

    /*
     * For indexes expressions, this holds the index value expression
     */
    struct ast_expression *extra;

    enum kind
    {
        INT_VALUE,
        IDENTIFIER_VALUE,
        FUNCTION_VALUE,
        STRING_VALUE,
        PTR_VALUE
    } kind;

    enum inplace_op
    {
        NO_OP,
        PRE_INCREMENT,
        POST_INCREMENT,
        PRE_DECREMENT,
        POST_DECREMENT
    } inplace_op;

    unsigned int arguments_size;
    struct ast_expression *arguments[0];
};

struct ast_selection_statement
{
    enum astnode_t type;
    enum astnode_t elided_type;

    struct ast_binary_op *expression;
    struct astnode *statement1;
    struct astnode *statement2;
};

struct ast_iteration_statement
{
    enum astnode_t type;
    enum astnode_t elided_type;

    struct astnode *expression1;
    struct astnode *expression2;
    struct astnode *expression3;
    struct astnode *statement;
};

struct ast_compound_statement
{
    enum astnode_t type;
    enum astnode_t elided_type;

    struct ast_declaration_list *declarations;
    struct ast_statement_list *statements;
};

struct ast_statement_list
{
    enum astnode_t type;
    enum astnode_t elided_type;

    unsigned int size;
    struct astnode *items[0];
};

struct ast_declaration_list
{
    enum astnode_t type;
    enum astnode_t elided_type;

    unsigned int size;
    struct ast_declaration *items[0];
};

struct ast_declarator
{
    enum astnode_t type;
    enum astnode_t elided_type;

    int is_pointer;

    char *declarator_identifier;

    int declarator_value;

    /*
     * Used to indicate number of objects (i.e. array can have count > 1)
     */
    struct ast_expression *count;

    struct ast_parameter_type_list *declarator_parameter_type_list;

    struct astnode *declarator_identifier_list;
};

struct ast_parameter_type_list
{
    enum astnode_t type;
    enum astnode_t elided_type;

    unsigned int size;
    struct ast_declaration *items[0];
};

struct ast_declaration
{
    enum astnode_t type;
    enum astnode_t elided_type;

    /*
     * Storage specifiers - there are 5 specifiers in ast.h:
     *     AUTO, REGISTER, STATIC, EXTERN, TYPEDEF
     */
    int storage_class_specifiers;

    /*
     * Type specifiers - there are 12 specifiers in ast.h:
     *      void, char, short, long, ... typedef name
     */
    int type_specifiers;

    /*
     * Type qualifier - there are 2 specifiers:
     *      const, volatile
     */
    int type_qualifier;

    /*
     * Declarators
     */
    int declarators_size;
    struct ast_declarator *declarators[1];
};

struct ast_function
{
    enum astnode_t type;
    enum astnode_t elided_type;

    /*
     * Contains specifiers and function args
     */
    struct ast_declarator *function_declarator;

    /*
     * List of variable declarations
     */
    struct astnode *declaration_list;

    /*
     * List of function statements
     */
    struct ast_compound_statement *statements;
};

struct ast_translation_unit
{
    enum astnode_t type;
    enum astnode_t elided_type;

    unsigned int translation_unit_items_size;

    /*
     * Variable length array of astnode with size specified by
     * translation_unit_items.
     */
    struct astnode *translation_unit_items[0];
};

struct ast_binary_op
{
    enum astnode_t type;
    enum astnode_t elided_type;

    enum astnode_t op;
    struct astnode *left;
    struct astnode *right;
};

struct astnode
{
    enum astnode_t type;

    /*
     * An elided type is removes extra types that aren't necessary during
     * generation.
     */
    enum astnode_t elided_type;
    struct token *token;
};

struct astnode *
create_translation_unit_node(struct listnode *list, struct rule *rule);

struct astnode *
create_elided_node(struct listnode *list, struct rule *rule);

struct astnode *
create_function_definition(struct listnode *list, struct rule *rule);

struct astnode *
create_declaration(struct listnode *list, struct rule *rule);

struct astnode *
create_declaration_list(struct listnode *list, struct rule *rule);

struct astnode *
create_parameter_list(struct listnode *list, struct rule *rule);

struct astnode *
create_parameter_declaration(struct listnode *list, struct rule *rule);

struct astnode *
create_expression_statement(struct listnode *list, struct rule *rule);

struct astnode *
create_compound_statement(struct listnode *list, struct rule *rule);

struct astnode *
create_statement_list(struct listnode *list, struct rule *rule);

struct astnode *
create_selection_statement(struct listnode *list, struct rule *rule);

struct astnode *
create_iteration_statement(struct listnode *list, struct rule *rule);

struct astnode *
create_jump_statement(struct listnode *list, struct rule *rule);

struct astnode *
create_assignment_expression(struct listnode *list, struct rule *rule);

struct astnode *
create_declaration_specifiers(struct listnode *list, struct rule *rule);

struct astnode *
create_init_declarator_list(struct listnode *list, struct rule *rule);

struct astnode *
create_init_declarator(struct listnode *list, struct rule *rule);

struct astnode *
create_declarator(struct listnode *list, struct rule *rule);

struct astnode *
create_direct_declarator(struct listnode *list, struct rule *rule);

struct astnode *
create_pointer(struct listnode *list, struct rule *rule);

struct astnode *
create_storage_class_specifier(struct listnode *list, struct rule *rule);

struct astnode *
create_type_specifier(struct listnode *list, struct rule *rule);

struct astnode *
create_type_qualifier(struct listnode *list, struct rule *rule);

struct astnode *
create_(struct listnode *list, struct rule *rule);

struct astnode *
create_binary_op(struct listnode *list, struct rule *rule);

struct astnode *
create_unary_expression(struct listnode *list, struct rule *rule);

struct astnode *
create_postfix_expression(struct listnode *list, struct rule *rule);

struct astnode *
create_primary_expression(struct listnode *list, struct rule *rule);

struct astnode *
create_argument_expression_list(struct listnode *list, struct rule *rule);

struct astnode *
create_constant(struct listnode *list, struct rule *rule);

#endif

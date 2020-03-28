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

struct astnode *
create_translation_unit_node(struct listnode *list, struct rule *rule);

struct astnode *
create_elided_node(struct listnode *list, struct rule *rule);

struct astnode *
create_function_definition(struct listnode *list, struct rule *rule);

struct astnode *
create_declaration(struct listnode *list, struct rule *rule);

struct astnode *
create_parameter_list(struct listnode *list, struct rule *rule);

struct astnode *
create_parameter_declaration(struct listnode *list, struct rule *rule);

struct astnode *
create_assignment_expression(struct listnode *list, struct rule *rule);

struct astnode *
create_declaration_specifiers(struct listnode *list, struct rule *rule);

struct astnode *
create_init_declarator_list(struct listnode *list, struct rule *rule);

struct astnode *
create_init_declarator(struct listnode *list, struct rule *rule);

struct astnode *
create_direct_declarator(struct listnode *list, struct rule *rule);

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

#endif

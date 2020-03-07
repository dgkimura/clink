#ifndef __AST_H__
#define __AST_H__

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "parser.h"
#include "utilities.h"

struct astnode *
create_translation_unit_node(struct listnode *list, struct rule *rule);

struct astnode *
create_elided_node(struct listnode *list, struct rule *rule);

struct astnode *
create_declaration(struct listnode *list, struct rule *rule);

struct astnode *
create_parameter_declaration(struct listnode *list, struct rule *rule);

struct astnode *
create_declaration_specifiers(struct listnode *list, struct rule *rule);

struct astnode *
create_direct_declarator(struct listnode *list, struct rule *rule);

struct astnode *
create_(struct listnode *list, struct rule *rule);

struct astnode *
create_binary_op(struct listnode *list, struct rule *rule);

#endif

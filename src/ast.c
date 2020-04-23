#include <stdarg.h>
#include <stdlib.h>

#include "ast.h"

static int
is_rule(struct rule *rule, ...)
{
    int i, is_rule = 1;
    va_list list;
    va_start(list, rule);

    for (i=0; i<rule->length_of_nodes; i++)
    {
        enum astnode_t node = va_arg(list, enum astnode_t);
        if (rule->nodes[i] != node)
        {
            is_rule = 0;
            break;
        }
    }
    return is_rule;
}

struct astnode *
create_translation_unit_node(struct listnode *list, struct rule *rule)
{
    unsigned int node_size;
    struct ast_translation_unit *node;
    struct astnode *child;

    if (is_rule(rule, AST_EXTERNAL_DECLARATION))
    {
        node_size = sizeof(struct astnode) + (sizeof(struct astnode *));
        node = malloc(node_size);
        memset(node, 0, node_size);

        /* index 1 is AST_EXTERNAL_DECLARATION astnode */
        /* index 0 is AST_EXTERNAL_DECLARATION state */
        node->translation_unit_items[0] = list_item(&list, 1);
        node->translation_unit_items_size = 1;
    }
    if (is_rule(rule, AST_TRANSLATION_UNIT, AST_EXTERNAL_DECLARATION))
    {
        /* index 3 is AST_TRANSLATION_UNIT astnode */
        /* index 2 is AST_TRANSLATION_UNIT state */
        node = list_item(&list, 3);

        node_size = sizeof(struct astnode) +
            sizeof(struct astnode *) * (node->translation_unit_items_size + 1);
        node = realloc(node, node_size);

        /* index 1 is AST_EXTERNAL_DECLARATION astnode */
        /* index 0 is AST_EXTERNAL_DECLARATION state */
        child = list_item(&list, 1);

        node->translation_unit_items[node->translation_unit_items_size] = child;
        node->translation_unit_items_size += 1;
    }
    node->type = rule->type;

    return (struct astnode *)node;
}

struct astnode *
create_elided_node(struct listnode *list, struct rule *rule)
{
    struct astnode *node;

    assert(rule->length_of_nodes == 1);

    node = list_item(&list, 1);
    node->type = rule->type;

    /*
     * Only set elided type if it has not already been set. We do not want to
     * override a lower node in the tree.
     */
    if (!node->elided_type)
    {
        node->elided_type = rule->nodes[0];
    }

    return node;
}

struct astnode *
create_function_definition(struct listnode *list, struct rule *rule)
{
    struct ast_function *node;

    node = malloc(sizeof(struct ast_function));
    memset(node, 0, sizeof(struct ast_function));

    if (is_rule(rule,
        AST_DECLARATION_SPECIFIERS, AST_DECLARATOR, AST_COMPOUND_STATEMENT))
    {
        /* index 5 is AST_DECLARATION_SPECIFIERS astnode */
        /* index 3 is AST_DECLARATOR state */
        /* index 1 is AST_COMPOUND_STATEMENT state */
        node->function_declarator = list_item(&list, 3);
        node->statements = list_item(&list, 1);
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_declaration(struct listnode *list, struct rule *rule)
{
    struct ast_declaration *node, *child;

    if (is_rule(rule, AST_DECLARATION_SPECIFIERS, AST_SEMICOLON))
    {
        /* index 3 is AST_DECLARATION_SPECIFIERS astnode */
        /* index 1 is AST_SEMICOLON state */
        node = list_item(&list, 3);
    }
    else if (is_rule(rule,
             AST_DECLARATION_SPECIFIERS, AST_INIT_DECLARATOR_LIST, AST_SEMICOLON))
    {
        /* index 5 is AST_DECLARATION_SPECIFIERS astnode */
        /* index 3 is AST_INIT_DECLARATOR_LIST state */
        /* index 1 is AST_SEMICOLON state */
        node = list_item(&list, 5);
        child = list_item(&list, 3);

        node->declarators_size = child->declarators_size;
        memcpy(node->declarators, child->declarators, sizeof(struct astnode *) * child->declarators_size);
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_declaration_list(struct listnode *list, struct rule *rule)
{
    unsigned int node_size;
    struct ast_declaration_list *node;
    struct ast_declaration *child;

    if (is_rule(rule, AST_DECLARATION_LIST, AST_DECLARATION))
    {
        /* index 3 is AST_DECLARATION_LIST astnode */
        /* index 1 is AST_DECLARATION astnode */
        node = list_item(&list, 3);

        node_size = sizeof(struct ast_declaration_list) +
                    sizeof(struct ast_declaration *) * (node->size + 1);
        node = realloc(node, node_size);
        child = list_item(&list, 1);

        node->items[node->size] = child;
        node->size += 1;
    }
    else if (is_rule(rule, AST_DECLARATION))
    {
        /* index 1 is AST_DECLARATION astnode */
        node_size = sizeof(struct astnode) + sizeof(struct astnode *);
        node = malloc(node_size);

        node->items[0] = list_item(&list, 1);
        node->size = 1;
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_parameter_list(struct listnode *list, struct rule *rule)
{
    unsigned int node_size;
    struct ast_parameter_type_list *node;
    struct ast_declaration *child;

    if (is_rule(rule, AST_PARAMETER_LIST, AST_COMMA, AST_PARAMETER_DECLARATION))
    {
        /* index 5 is AST_PARAMETER_LIST astnode */
        /* index 1 is AST_PARAMETER_DECLARATION astnode */
        node = list_item(&list, 5);

        node_size = sizeof(struct astnode) + sizeof(struct astnode *) *
            (node->size + 1);
        node = realloc(node, node_size);
        child = list_item(&list, 1);

        node->items[node->size] = child;
        node->size += 1;
    }
    else if (is_rule(rule, AST_PARAMETER_DECLARATION))
    {
        /* index 1 is AST_PARAMETER_DECLARATION astnode */
        child = list_item(&list, 1);

        node_size = sizeof(struct astnode) + sizeof(struct astnode *);
        node = malloc(node_size);

        node->items[0] = child;
        node->size = 1;
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_parameter_declaration(struct listnode *list, struct rule *rule)
{
    struct ast_declaration *node;
    struct ast_declarator *child;

    if (is_rule(rule, AST_DECLARATION_SPECIFIERS))
    {
        /* index 1 is AST_DECLARATION_SPECIFIERS astnode */
        node = list_item(&list, 1);
        node->type = rule->type;
        return (struct astnode *)node;
    }

    assert(rule->length_of_nodes == 2);

    /* index 3 is AST_DECLARATION_SPECIFIERS astnode */
    node = list_item(&list, 3);

    /* index 1 is [ AST_DECLARATOR | AST_ABSTRACT_DECLARATOR ] astnode */
    child = list_item(&list, 1);

    node->declarators[0] = child;
    node->declarators_size = 1;

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_expression_statement(struct listnode *list, struct rule *rule)
{
    struct astnode *node;

    if (is_rule(rule, AST_EXPRESSION, AST_SEMICOLON))
    {
        node = list_item(&list, 3);
    }

    node->type = rule->type;
    return node;
}

struct astnode *
create_compound_statement(struct listnode *list, struct rule *rule)
{
    struct ast_compound_statement *node;

    node = malloc(sizeof(struct ast_compound_statement));
    memset(node, 0, sizeof(struct ast_compound_statement));

    if (is_rule(rule, AST_LBRACE, AST_STATEMENT_LIST, AST_RBRACE))
    {
        node->statements = list_item(&list, 3);
    }
    else if (is_rule(rule,
             AST_LBRACE, AST_DECLARATION_LIST, AST_STATEMENT_LIST, AST_RBRACE))
    {
        node->statements = list_item(&list, 3);
        node->declarations = list_item(&list, 5);
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_statement_list(struct listnode *list, struct rule *rule)
{
    unsigned int node_size;
    struct ast_statement_list *node, *child;


    if (is_rule(rule, AST_STATEMENT))
    {
        node_size = sizeof(struct ast_statement_list) + (sizeof(struct astnode *));
        node = malloc(node_size);
        memset(node, 0, node_size);

        /* index 1 is AST_STATEMENT astnode */
        node->items[0] = list_item(&list, 1);
        node->size = 1;
    }
    else if (is_rule(rule, AST_STATEMENT_LIST, AST_STATEMENT))
    {
        /* index 3 is AST_STATEMENT_LIST astnode */
        /* index 1 is AST_STATEMENT astnode */
        child = list_item(&list, 3);

        node_size = sizeof(struct ast_statement_list) +
            (sizeof(struct astnode *) * (child->size + 1));
        node = realloc(child, node_size);

        node->items[node->size] = list_item(&list, 1);
        node->size += 1;
    }
    node->type = rule->type;

    return (struct astnode *)node;
}

struct astnode *
create_selection_statement(struct listnode *list, struct rule *rule)
{

    struct astnode *expression;
    struct astnode *statement1;

    struct ast_selection_statement *node;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    if (is_rule(rule,
        AST_IF, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT))
    {
        node->expression = list_item(&list, 5);
        node->statement1 = list_item(&list, 1);
    }
    else if (is_rule(rule,
        AST_IF, AST_LPAREN, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT,
        AST_ELSE, AST_STATEMENT))
    {
        node->expression = list_item(&list, 9);
        node->statement1 = list_item(&list, 5);
        node->statement2 = list_item(&list, 1);
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_iteration_statement(struct listnode *list, struct rule *rule)
{
    struct astnode *expression1;
    struct astnode *expression2;
    struct astnode *expression3;
    struct astnode *statement;

    struct ast_iteration_statement *node;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    if (is_rule(rule,
        AST_FOR, AST_LPAREN, AST_EXPRESSION, AST_SEMICOLON, AST_EXPRESSION,
        AST_SEMICOLON, AST_EXPRESSION, AST_RPAREN, AST_STATEMENT))
    {
        node->expression1 = list_item(&list, 13);
        node->expression2 = list_item(&list, 9);
        node->expression3 = list_item(&list, 5);
        node->statement = list_item(&list, 1);
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_jump_statement(struct listnode *list, struct rule *rule)
{
    struct astnode *node;

    if (is_rule(rule, AST_RETURN, AST_EXPRESSION, AST_SEMICOLON))
    {
        node = list_item(&list, 3);
    }

    node->type = rule->type;
    return node;
}

struct astnode *
create_assignment_expression(struct listnode *list, struct rule *rule)
{
    /*
     * TODO: This is identical to create_binary_op(). Is a duplicate function
     * necessary?
     */

    struct astnode *node;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    /* index 1 is right astnode */
    /* index 3 is operator astnode */
    /* index 5 is right astnode */
    node->left = list_item(&list, 5);
    node->op = ((struct astnode *)list_item(&list, 3))->type;
    node->right = list_item(&list, 1);

    node->elided_type = rule->type;
    node->type = rule->type;
    return node;
}

struct astnode *
create_declaration_specifiers(struct listnode *list, struct rule *rule)
{
    struct ast_declaration *node, *child;

    if (rule->length_of_nodes == 1)
    {
        node = malloc(sizeof(struct astnode));
        memset(node, 0, sizeof(struct astnode));
        child = list_item(&list, 1);
    }
    else if (rule->length_of_nodes == 2)
    {
        /* index 1 is AST_DECLARATION_SPECIFIERS astnode */
        /* index 0 is AST_DECLARATION_SPECIFIERS state */
        node = list_item(&list, 1);

        child = list_item(&list, 3);
    }

    switch (child->type)
    {
        case AST_STORAGE_CLASS_SPECIFIER:
        {
            node->storage_class_specifiers |= child->storage_class_specifiers;
            break;
        }
        case AST_TYPE_SPECIFIER:
        {
            /* FIXME: How to handle struct, union, or enum specifier? */
            node->type_specifiers |= child->type_specifiers;
            break;
        }
        case AST_TYPE_QUALIFIER:
        {
            node->type_qualifier |= child->type_qualifier;
            break;
        }
        default:
        {
            assert(0);
            break;
        }

    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_init_declarator_list(struct listnode *list, struct rule *rule)
{
    struct ast_declaration *node, *init_declarator_list;
    struct ast_declarator *init_declarator;
    size_t node_size;

    assert(rule->length_of_nodes == 1 || rule->length_of_nodes == 3);

    if (is_rule(rule, AST_INIT_DECLARATOR))
    {
        /* index 1 is AST_INIT_DECLARATOR astnode */
        init_declarator = list_item(&list, 1);

        node_size = sizeof(struct ast_declaration);

        node = malloc(node_size);
        memset(node, 0, node_size);

        node->declarators_size = 1;
        node->declarators[0] = init_declarator;
    }
    else if (is_rule(rule, AST_INIT_DECLARATOR_LIST, AST_COMMA, AST_INIT_DECLARATOR))
    {
        /* index 5 is AST_INIT_DECLARATOR_LIST astnode */
        /* index 3 is AST_COMMA astnode */
        /* index 1 is AST_INIT_DECLARATOR astnode */
        init_declarator_list = list_item(&list, 5);
        init_declarator = list_item(&list, 1);

        node_size = sizeof(struct ast_declaration) +
                    sizeof(struct ast_declarator *) *
                    (init_declarator_list->declarators_size + 1);

        node = malloc(node_size);
        memset(node, 0, node_size);

        node->declarators_size = init_declarator_list->declarators_size + 1;
        memcpy(node->declarators, init_declarator_list->declarators,
            sizeof(struct astnode *) * init_declarator_list->declarators_size);
        node->declarators[node->declarators_size - 1] = init_declarator;
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_init_declarator(struct listnode *list, struct rule *rule)
{
    struct astnode *node;
    assert(rule->length_of_nodes == 3);


    if (is_rule(rule, AST_DECLARATOR, AST_EQUAL, AST_INITIALIZER))
    {
        /* index 5 is AST_DECLARATOR astnode */
        /* index 3 is AST_EQUAL astnode */
        /* index 1 is AST_INITIALIZER astnode */
        node = list_item(&list, 5);
    }
    return node;
}

struct astnode *
create_direct_declarator(struct listnode *list, struct rule *rule)
{
    struct ast_declarator *node;
    struct ast_declaration *child;

    if (is_rule(rule, AST_IDENTIFIER))
    {
        /* index 1 is AST_IDENTIFIER astnode */
        child = list_item(&list, 1);

        node = malloc(sizeof(struct ast_declarator));
        memset(node, 0, sizeof(struct ast_declarator));

        node->declarator_identifier = child->token->value;
    }
    else if (rule->length_of_nodes == 3)
    {
        node = list_item(&list, 5);
        if (node->type == AST_DIRECT_DECLARATOR)
        {
            /* { AST_DIRECT_DECLARATOR, AST_LBRACKET, AST_RBRACKET } */
            /* { AST_DIRECT_DECLARATOR,  AST_LPAREN,  AST_RPAREN } */
        }
        else
        {
            /* { AST_LPAREN, AST_DECLARATOR, AST_RPAREN } */
            node = list_item(&list, 3);
        }
    }
    else if (rule->length_of_nodes == 4)
    {
        /* index 7 is AST_DIRECT_DECLARATOR astnode */
        /* index 3 is AST_PARAMETER_TYPE_LIST | AST_IDENTIFIER_LIST astnode */
        node = list_item(&list, 7);
        child = list_item(&list, 3);

        switch (child->type)
        {
            case AST_CONSTANT_EXPRESSION:
            {
                /*TODO:*/
                break;
            }
            case AST_PARAMETER_TYPE_LIST:
            {
                node->declarator_parameter_type_list = (struct ast_parameter_type_list *)child;
                break;
            }
            case AST_IDENTIFIER_LIST:
            {
                node->declarator_identifier_list = (struct astnode *)child;
                break;
            }
            default:
            {
                assert(0);
                break;
            }

        }
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_storage_class_specifier(struct listnode *list, struct rule *rule)
{
    struct ast_declaration *node;
    struct astnode *child;
    node = malloc(sizeof(struct ast_declaration));
    memset(node, 0, sizeof(struct ast_declaration));

    assert(rule->length_of_nodes == 1);

    child = list_item(&list, 1);
    switch (child->type)
    {
        case AST_AUTO:
        {
            node->storage_class_specifiers = AUTO;
            break;
        }
        case AST_REGISTER:
        {
            node->storage_class_specifiers = REGISTER;
            break;
        }
        case AST_STATIC:
        {
            node->storage_class_specifiers = STATIC;
            break;
        }
        case AST_EXTERN:
        {
            node->storage_class_specifiers = EXTERN;
            break;
        }
        case AST_TYPEDEF:
        {
            node->storage_class_specifiers = TYPEDEF;
            break;
        }
        default:
        {
            assert(0);
            break;
        }
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_type_specifier(struct listnode *list, struct rule *rule)
{
    struct ast_declaration *node;
    struct astnode *child;
    node = malloc(sizeof(struct ast_declaration));
    memset(node, 0, sizeof(struct ast_declaration));

    assert(rule->length_of_nodes == 1);

    child = list_item(&list, 1);
    switch (child->type)
    {
        case AST_VOID:
        {
            node->type_specifiers = VOID;
            break;
        }
        case AST_CHAR:
        {
            node->type_specifiers = CHAR;
            break;
        }
        case AST_SHORT:
        {
            node->type_specifiers = SHORT;
            break;
        }
        case AST_INT:
        {
            node->type_specifiers = INT;
            break;
        }
        case AST_LONG:
        {
            node->type_specifiers = LONG;
            break;
        }
        case AST_FLOAT:
        {
            node->type_specifiers = FLOAT;
            break;
        }
        case AST_DOUBLE:
        {
            node->type_specifiers = DOUBLE;
            break;
        }
        case AST_SIGNED:
        {
            node->type_specifiers = SIGNED;
            break;
        }
        case AST_UNSIGNED:
        {
            node->type_specifiers = UNSIGNED;
            break;
        }
        case AST_STRUCT_OR_UNION_SPECIFIER:
        {
            node->type_specifiers = STRUCT_OR_UNION_SPECIFIER;
            break;
        }
        case AST_ENUM_SPECIFIER:
        {
            node->type_specifiers = ENUM_SPECIFIER;
            break;
        }
        case AST_TYPEDEF_NAME:
        {
            node->type_specifiers = TYPEDEF_NAME;
            break;
        }
        default:
        {
            assert(0);
            break;
        }
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_type_qualifier(struct listnode *list, struct rule *rule)
{
    struct ast_declaration *node;
    struct astnode *child;
    node = malloc(sizeof(struct ast_declaration));
    memset(node, 0, sizeof(struct ast_declaration));

    assert(rule->length_of_nodes == 1);

    child = list_item(&list, 1);
    switch (child->type)
    {
        case AST_CONST:
        {
            node->type_qualifier = VOID;
            break;
        }
        case AST_VOLATILE:
        {
            node->type_qualifier = VOLATILE;
            break;
        }
        default:
        {
            assert(0);
            break;
        }
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_(struct listnode *list, struct rule *rule)
{
    struct astnode *node;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    node->type = rule->type;
    return node;
}

struct astnode *
create_binary_op(struct listnode *list, struct rule *rule)
{
    struct astnode *node;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    /* index 1 is right astnode */
    /* index 3 is operator astnode */
    /* index 5 is right astnode */
    node->left = list_item(&list, 5);
    node->op = ((struct astnode *)list_item(&list, 3))->type;
    node->right = list_item(&list, 1);

    node->elided_type = rule->type;
    node->type = rule->type;
    return node;
}

struct astnode *
create_unary_expression(struct listnode *list, struct rule *rule)
{
    struct ast_expression *node;

    if (is_rule(rule, AST_PLUS_PLUS, AST_UNARY_EXPRESSION))
    {
        node = list_item(&list, 1);
        node->inplace_op = PRE_INCREMENT;
    }
    else if (is_rule(rule, AST_MINUS_MINUS, AST_UNARY_EXPRESSION))
    {
        node = list_item(&list, 1);
        node->inplace_op = PRE_DECREMENT;
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_postfix_expression(struct listnode *list, struct rule *rule)
{
    struct ast_expression *node, *child;

    if (is_rule(rule, AST_POSTFIX_EXPRESSION, AST_LPAREN, AST_RPAREN))
    {
        node = list_item(&list, 5);
        node->kind = FUNCTION_VALUE;
    }
    else if (is_rule(rule,
        AST_POSTFIX_EXPRESSION, AST_LPAREN, AST_ARGUMENT_EXPRESSION_LIST, AST_RPAREN))
    {
        /* index 7 is AST_POSTFIX_EXPRESSION astnode */
        /* index 3 is AST_ARGUMENT_EXPRESSION_LIST astnode */
        child = list_item(&list, 7);
        node = list_item(&list, 3);

        node->identifier = child->identifier;
        node->kind = FUNCTION_VALUE;
    }
    else if (is_rule(rule, AST_POSTFIX_EXPRESSION, AST_PLUS_PLUS))
    {
        node = list_item(&list, 3);
        node->inplace_op = POST_INCREMENT;
    }
    else if (is_rule(rule, AST_POSTFIX_EXPRESSION, AST_MINUS_MINUS))
    {
        node = list_item(&list, 3);
        node->inplace_op = POST_DECREMENT;
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_primary_expression(struct listnode *list, struct rule *rule)
{
    struct ast_expression *node;
    struct astnode *child;

    if (is_rule(rule, AST_IDENTIFIER))
    {
        node = malloc(sizeof(struct astnode));
        memset(node, 0, sizeof(struct astnode));

        child = list_item(&list, 1);
        node->identifier = child->token->value;
        node->kind = IDENTIFIER_VALUE;
    }
    else if (is_rule(rule, AST_STRING_CONSTANT))
    {
        node = malloc(sizeof(struct astnode));
        memset(node, 0, sizeof(struct astnode));

        child = list_item(&list, 1);
        node->identifier = child->token->value;
        node->kind = STRING_VALUE;
    }
    else if (is_rule(rule, AST_LPAREN, AST_EXPRESSION, AST_RPAREN))
    {
        node = list_item(&list, 3);
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_argument_expression_list(struct listnode *list, struct rule *rule)
{
    unsigned int node_size;
    struct ast_expression *node;

    if (rule->length_of_nodes == 1)
    {
        node_size = sizeof(struct ast_expression) + (sizeof(struct ast_expression *));
        node = malloc(node_size);
        memset(node, 0, node_size);

        node->arguments[0] = list_item(&list, 1);
        node->arguments_size = 1;
    }
    else
    {
        node = list_item(&list, 5);

        node_size = sizeof(struct ast_expression) +
            (sizeof(struct ast_expression *) * node->arguments_size + 1);
        node = realloc(node, node_size);

        node->arguments[node->arguments_size] = list_item(&list, 1);
        node->arguments_size += 1;
    }

    node->type = rule->type;
    return (struct astnode *)node;
}

struct astnode *
create_constant(struct listnode *list, struct rule *rule)
{
    struct ast_expression *node;
    struct astnode *child;
    node = malloc(sizeof(struct ast_expression));
    memset(node, 0, sizeof(struct ast_expression));

    child = list_item(&list, 1);

    node->int_value = atoi(child->token->value);
    node->type = rule->type;
    node->elided_type = rule->nodes[0];
    return (struct astnode *)node;
}

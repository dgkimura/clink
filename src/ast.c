#include "ast.h"

struct astnode *
create_translation_unit_node(struct listnode *list, struct rule *rule)
{
    unsigned int i, node_size;
    struct astnode *node;
    struct astnode *child;


    if (rule->length_of_nodes == 1)
    {
        node_size = sizeof(struct astnode) + (sizeof(struct astnode *));
        node = malloc(node_size);
        memset(node, 0, node_size);

        node->translation_unit_items[0] = list_item(&list, 0);
        node->translation_unit_items_size = 1;
        node->type = rule->type;
    }
    else if (rule->length_of_nodes == 2)
    {
        /* index 3 is AST_TRANSLATION_UNIT astnode */
        /* index 2 is AST_TRANSLATION_UNIT state */
        child = list_item(&list, 3);

        node_size = sizeof(struct astnode) +
            (sizeof(struct astnode *) * child->translation_unit_items_size);
        node = realloc(child, node_size);

        /* index 1 is AST_EXTERNAL_DECLARATION astnode */
        /* index 0 is AST_EXTERNAL_DECLARATION state */
        child = list_item(&list, 1);

        node->translation_unit_items_size += 1;
        node->translation_unit_items[node->translation_unit_items_size] = child;
    }

    return node;
}

struct astnode *
create_elided_node(struct listnode *list, struct rule *rule)
{
    struct astnode *node;

    node = list_item(&list, 1);
    node->type = rule->type;

    return node;
}

struct astnode *
create_declaration(struct listnode *list, struct rule *rule)
{
    struct astnode *node, *child;

    assert(rule->length_of_nodes == 3);

    /* index 5 is AST_DECLARATION_SPECIFIERS astnode */
    /* index 3 is AST_INIT_DECLARATOR_LIST state */
    /* index 1 is AST_SEMICOLON state */
    node = list_item(&list, 5);
    child = list_item(&list, 3);

    node->declarator_identifier = child->declarator_identifier;
    node->declarators_size = child->declarators_size;
    memcpy(node->declarators, child->declarators, sizeof(struct astnode *) * child->declarators_size);

    node->type = rule->type;
    return node;
}

struct astnode *
create_parameter_declaration(struct listnode *list, struct rule *rule)
{
    struct astnode *node, *child;

    if (rule->length_of_nodes == 1)
    {
        /* index 1 is AST_DECLARATION_SPECIFIERS astnode */
        node = list_item(&list, 1);
        node->type = rule->type;
        return node;
    }

    assert(rule->length_of_nodes == 2);

    /* index 3 is AST_DECLARATION_SPECIFIERS astnode */
    node = list_item(&list, 3);

    /* index 1 is [ AST_DECLARATOR | AST_ABSTRACT_DECLARATOR ] astnode */
    child = list_item(&list, 1);

    node->declarator_identifier = child->declarator_identifier;
    node->declarators = child;
    node->declarators_size = 1;

    node->type = rule->type;
    return node;
}

struct astnode *
create_declaration_specifiers(struct listnode *list, struct rule *rule)
{
    struct astnode *node, *child;

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
            node->storage_class_specifiers[node->storage_class_specifiers_size++] = child->type;
            break;
        }
        case AST_TYPE_SPECIFIER:
        {
            /* FIXME: How to handle struct, union, or enum specifier? */
            node->type_specifiers[node->type_specifiers_size++] = child->type;
            break;
        }
        case AST_TYPE_QUALIFIER:
        {
            node->type_qualifier[node->type_qualifier_size++] = child->type;
            break;
        }
        default:
        {
            assert(0);
            break;
        }

    }

    node->type = rule->type;
    return node;
}

struct astnode *
create_direct_declarator(struct listnode *list, struct rule *rule)
{
    int i;
    struct astnode *node, *child;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    if (rule->length_of_nodes == 1)
    {
        /* index 1 is AST_IDENTIFIER astnode */
        child = list_item(&list, 1);

        node->declarator_identifier = child->token->value;
    }
    else if (rule->length_of_nodes == 3)
    {
        child = list_item(&list, 5);
        if (child->type == AST_DIRECT_DECLARATOR)
        {
            /* { AST_DIRECT_DECLARATOR, AST_LBRACKET, AST_RBRACKET } */
            /* { AST_DIRECT_DECLARATOR,  AST_LPAREN,  AST_RPAREN } */

            /* index 3 is AST_DECLARATOR astnode */
            node = child;
        }
        else
        {
            /* { AST_LPAREN, AST_DECLARATOR, AST_RPAREN } */

            /* index 3 is AST_DECLARATOR astnode */
            node = list_item(&list, 3);
        }
    }
    else if (rule->length_of_nodes == 4)
    {
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
                for (i=0; i<child->declarator_parameter_type_list_size; i++)
                {
                    node->declarator_parameter_type_list[i] =
                        child->declarator_parameter_type_list[i];
                }
                break;
            }
            case AST_IDENTIFIER_LIST:
            {
                for (i=0; i<child->declarator_identifier_list_size; i++)
                {
                    node->declarator_identifier_list[i] =
                        child->declarator_identifier_list[i];
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

    node->type = rule->type;
    return node;
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

    node->left = list->data;
    /*
     * next = token
     * next->next = next state
     * next->next->next = next token
     */
    node->right = list->next->next->next->next->data;

    node->type = rule->type;
    return node;
}

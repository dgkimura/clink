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

        /* index 1 is AST_EXTERNAL_DECLARATION astnode */
        /* index 0 is AST_EXTERNAL_DECLARATION state */
        node->translation_unit_items[0] = list_item(&list, 1);
        node->translation_unit_items_size = 1;
        node->elided_type = rule->type;
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
    node->elided_type = rule->nodes[0];

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
    node->declarators[0] = child;
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
    return node;
}

struct astnode *
create_init_declarator_list(struct listnode *list, struct rule *rule)
{
    struct astnode *node, *init_declarator, *init_declarator_list;
    size_t node_size;

    assert(rule->length_of_nodes == 1 || rule->length_of_nodes == 3);

    if (rule->length_of_nodes == 1)
    {
        /* index 1 is AST_INIT_DECLARATOR astnode */
        init_declarator = list_item(&list, 1);

        node = malloc(sizeof(struct astnode) + sizeof(struct astnode *));
        memset(node, 0, sizeof(struct astnode));

        node->declarators_size = 1;
        node->declarators[0] = init_declarator;
    }
    else if (rule->length_of_nodes == 3)
    {
        /* index 5 is AST_INIT_DECLARATOR_LIST astnode */
        /* index 3 is AST_COMMA astnode */
        /* index 1 is AST_INIT_DECLARATOR astnode */
        init_declarator_list = list_item(&list, 5);
        init_declarator = list_item(&list, 1);

        node_size = sizeof(struct astnode) + sizeof(struct astnode *) *
            init_declarator_list->declarators_size;

        node = malloc(node_size);
        memset(node, 0, node_size);

        node->declarators_size = init_declarator_list->declarators_size + 1;
        memcpy(node->declarators, init_declarator_list->declarators,
            sizeof(struct astnode *) * init_declarator_list->declarators_size);
        node->declarators[node->declarators_size - 1] = init_declarator;
    }

    node->type = rule->type;
    return node;
}

struct astnode *
create_init_declarator(struct listnode *list, struct rule *rule)
{
    struct astnode *node;
    assert(rule->length_of_nodes == 3);

    /* index 5 is AST_DECLARATOR astnode */
    /* index 3 is AST_EQUAL astnode */
    /* index 1 is AST_INITIALIZER astnode */

    node = list_item(&list, 5);
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
create_storage_class_specifier(struct listnode *list, struct rule *rule)
{
    struct astnode *node, *child;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    assert(rule->length_of_nodes == 1);

    child = list_item(&list, 1);
    switch (child->type)
    {
        case AST_AUTO:
        {
            child->storage_class_specifiers = AUTO;
            break;
        }
        case AST_REGISTER:
        {
            child->storage_class_specifiers = REGISTER;
            break;
        }
        case AST_STATIC:
        {
            child->storage_class_specifiers = STATIC;
            break;
        }
        case AST_EXTERN:
        {
            child->storage_class_specifiers = EXTERN;
            break;
        }
        case AST_TYPEDEF:
        {
            child->storage_class_specifiers = TYPEDEF;
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
create_type_specifier(struct listnode *list, struct rule *rule)
{
    struct astnode *node, *child;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

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
    return node;
}

struct astnode *
create_type_qualifier(struct listnode *list, struct rule *rule)
{
    struct astnode *node, *child;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    assert(rule->length_of_nodes == 1);

    child = list_item(&list, 1);
    switch (child->type)
    {
        case AST_CONST:
        {
            child->type_qualifier = VOID;
            break;
        }
        case AST_VOLATILE:
        {
            child->type_qualifier = VOLATILE;
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

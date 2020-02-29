#ifndef __PARSER_H__
#define __PARSER_H__

#include "scanner.h"

enum astnode_t
{
    /*
     * following are terminal values in grammar
     */
    AST_CHARACTER_CONSTANT,
    AST_INTEGER_CONSTANT,
    AST_IDENTIFIER,
    AST_ARROW,
    AST_PLUS,
    AST_PLUS_PLUS,
    AST_PLUS_EQUAL,
    AST_MINUS,
    AST_MINUS_MINUS,
    AST_MINUS_EQUAL,
    AST_AMPERSAND,
    AST_AMPERSAND_AMPERSAND,
    AST_ASTERISK,
    AST_ASTERISK_EQUAL,
    AST_BACKSLASH,
    AST_BACKSLASH_EQUAL,
    AST_CARET,
    AST_COMMA,
    AST_DOT,
    AST_ELLIPSIS,
    AST_MOD,
    AST_MOD_EQUAL,
    AST_QUESTIONMARK,
    AST_COLON,
    AST_SEMICOLON,
    AST_LPAREN,
    AST_RPAREN,
    AST_LBRACKET,
    AST_RBRACKET,
    AST_LBRACE,
    AST_RBRACE,
    AST_VERTICALBAR,
    AST_VERTICALBAR_VERTICALBAR,
    AST_SHIFTLEFT,
    AST_SHIFTRIGHT,
    AST_LT,
    AST_GT,
    AST_LTEQ,
    AST_GTEQ,
    AST_EQ,
    AST_NEQ,
    AST_EQUAL,

    /* reserved words */
    AST_VOID,
    AST_CHAR,
    AST_SHORT,
    AST_INT,
    AST_LONG,
    AST_FLOAT,
    AST_DOUBLE,
    AST_SIGNED,
    AST_UNSIGNED,
    AST_GOTO,
    AST_CONTINUE,
    AST_BREAK,
    AST_RETURN,
    AST_FOR,
    AST_DO,
    AST_WHILE,
    AST_IF,
    AST_ELSE,
    AST_SWITCH,
    AST_CASE,
    AST_DEFAULT,
    AST_ENUM,
    AST_STRUCT,
    AST_UNION,
    AST_CONST,
    AST_VOLATILE,
    AST_AUTO,
    AST_REGISTER,
    AST_STATIC,
    AST_EXTERN,
    AST_TYPEDEF,
    AST_TYPEDEF_NAME,

    /*
     * invalid symbol that separates termainal and non-terminal values
     */
    AST_INVALID,

    /*
     * following variable values in grammar
     */
    AST_CONSTANT,
    AST_PRIMARY_EXPRESSION,
    AST_POSTFIX_EXPRESSION,
    AST_UNARY_EXPRESSION,
    AST_CAST_EXPRESSION,
    AST_MULTIPLICATIVE_EXPRESSION,
    AST_ADDITIVE_EXPRESSION,
    AST_SHIFT_EXPRESSION,
    AST_RELATIONAL_EXPRESSION,
    AST_EQUALITY_EXPRESSION,
    AST_AND_EXPRESSION,
    AST_EXCLUSIVE_OR_EXPRESSION,
    AST_INCLUSIVE_OR_EXPRESSION,
    AST_LOGICAL_AND_EXPRESSION,
    AST_LOGICAL_OR_EXPRESSION,
    AST_CONSTANT_EXPRESSION,
    AST_CONDITIONAL_EXPRESSION,
    AST_ASSIGNMENT_EXPRESSION,
    AST_EXPRESSION,
    AST_JUMP_STATEMENT,
    AST_ITERATION_STATEMENT,
    AST_SELECTION_STATEMENT,
    AST_STATEMENT_LIST,
    AST_COMPOUND_STATEMENT,
    AST_EXPRESSION_STATEMENT,
    AST_LABELED_STATEMENT,
    AST_STATEMENT,
    AST_DIRECT_ABSTRACT_DECLARATOR,
    AST_ABSTRACT_DECLARATOR,
    AST_TYPE_NAME,
    AST_INITIALIZER_LIST,
    AST_INITIALIZER,
    AST_IDENTIFIER_LIST,
    AST_PARAMETER_DECLARATION,
    AST_PARAMETER_LIST,
    AST_PARAMETER_TYPE_LIST,
    AST_TYPE_QUALIFIER,
    AST_TYPE_QUALIFIER_LIST,
    AST_POINTER,
    AST_DIRECT_DECLARATOR,
    AST_DECLARATOR,
    AST_ENUMERATOR,
    AST_ENUMERATOR_LIST,
    AST_ENUM_SPECIFIER,
    AST_STRUCT_DECLARATOR,
    AST_STRUCT_DECLARATOR_LIST,
    AST_STRUCT_QUALIFIER_LIST,
    AST_STRUCT_DECLARATION,
    AST_SPECIFIER_QUALIFIER_LIST,
    AST_INIT_DECLARATOR,
    AST_INIT_DECLARATOR_LIST,
    AST_STRUCT_DECLARATION_LIST,
    AST_STRUCT_OR_UNION,
    AST_STRUCT_OR_UNION_SPECIFIER,
    AST_TYPE_SPECIFIER,
    AST_STORAGE_CLASS_SPECIFIER,
    AST_DECLARATION_SPECIFIERS,
    AST_DECLARATION_LIST,
    AST_DECLARATION,
    AST_FUNCTION_DEFINITION,
    AST_EXTERNAL_DECLARATION,
    AST_TRANSLATION_UNIT
};


#define MAX_ASTNODES 9

struct rule
{
    enum astnode_t type;
    struct astnode *(*create)(struct listnode *list, struct rule *rule);
    int length_of_nodes;
    enum astnode_t nodes[MAX_ASTNODES];
};

#define NUM_RULES 199

struct astnode
{
    enum astnode_t type;
    struct token *token;

    struct listnode *children;

    union
    {
        int int_value;

        struct
        {
            unsigned int translation_unit_items_size;

            /*
             * Variable length array of astnode with size specified by
             * translation_unit_items.
             */
            struct astnode *translation_unit_items[0];
        };

        struct
        {
            enum astnode_t op;
            struct astnode *left;
            struct astnode *right;
        };

        /*
         * Declarator
         */
        struct
        {
            /*
             * Storage specifiers - there are 5 specifiers:
             *     auto, register, static, extern, typedef
             */
            enum astnode_t storage_class_specifiers[5];

            /*
             * Type specifiers - there are 12 specifiers:
             *      void, char, short, long, ... typedef name
             */
            enum astnode_t type_specifiers[12];

            /*
             * Type qualifier - there are 2 specifiers:
             *      const, volatile
             */
            enum astnode_t type_qualifier[2];

            /*
             * Name of the declaration
             */
            char *declaration_name;

            /*
             * If array, size of array
             * If function declarator, number of args
             */
            unsigned int declaration_size;

            /*
             * Function arguments. Array of declarators where size is defined by
             * declaration_size.
             */
            struct astnode *args;
        };

        /*
         * Function definition
         */
        struct
        {
            /*
             * Contains specifiers and function args
             */
            struct astnode *function_declarator;

            /*
             * List of variable declarations
             */
            struct astnode *declaration_list;

            /*
             * List of function statements
             */
            struct astnode *statement_list;
        };
    };
};

/*
 * item is a rule with a cursor position to indicate how many symbols have been
 * consumed.
 */
struct item
{
    /*
     * rule that is being used.
     */
    struct rule *rewrite_rule;

    /*
     * position into the rule.
     */
    int cursor_position;

    /*
     * list of lookahead symbols.
     */
    struct listnode *lookahead;
};

#define NUM_TERMINALS (AST_INVALID - AST_CHARACTER_CONSTANT+ 1)
#define NUM_SYMBOLS (AST_TRANSLATION_UNIT - AST_CHARACTER_CONSTANT + 1)
#define INDEX(s) ((s) - AST_CHARACTER_CONSTANT)

/*
 * state contains information of a set of items.
 */
struct state
{
    /*
     * unique identifier of the state.
     */
    int identifier;

    /*
     * list of items in the state.
     */
    struct listnode *items;

    /*
     * links maps symbol transitions to other states.
     */
    struct state *links[NUM_SYMBOLS];
};

#define MAX_STATES 16384

/*
 * item inside a parse table row.
 */
struct parsetable_item
{
    /*
     * rule to reduce, if this is a reduce operation.
     */
    struct rule *rule;

    /*
     * shift indicates whether this is a shift operation. It cannot be both a
     * shift and reduce operation.
     */
    int shift;

    /*
     * reduce indicates whether this is a shift operation. It cannot be both a
     * shift and reduce operation.
     */
    int reduce;

    /*
     * state indicates the next state to shift to.
     */
    int state;
};

struct rule *
get_grammar(void);

void
head_terminal_values(enum astnode_t node, struct listnode **checked_nodes,
                     struct listnode **terminals);

void
generate_items(enum astnode_t node, struct listnode *lookahead, struct listnode **items);

void
generate_transitions(struct state *state);

struct state *
generate_states(void);

int
state_contains_item(struct state *state, struct item *item);

int
compare_states(struct state *a, struct state *b);

int
index_of_state(struct state *state);

void
init_parsetable(void);

struct astnode *
token_to_astnode(struct token * token);

struct astnode *
parse(struct listnode *tokens);

struct astnode *
create_translation_unit_node(struct listnode *list, struct rule *rule);

struct astnode *
create_(struct listnode *list, struct rule *rule);

struct astnode *
create_binary_op(struct listnode *list, struct rule *rule);

#endif

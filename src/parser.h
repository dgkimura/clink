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

struct astnode
{
    enum astnode_t type;
    union
    {
        struct
        {
            struct token *constant;
        };
    };
    struct listnode *children;
};

#define MAX_ASTNODES 9

struct rule
{
    enum astnode_t type;
    int length_of_nodes;
    enum astnode_t nodes[MAX_ASTNODES];
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

#define MAX_STATES 1024

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
    unsigned int shift:1;

    /*
     * reduce indicates whether this is a shift operation. It cannot be both a
     * shift and reduce operation.
     */
    unsigned int reduce:1;

    /*
     * state indicates the next state to shift to.
     */
    unsigned int state:30;
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

struct parsetable_item *
generate_parsetable(void);

struct astnode *
shift(struct token * token);

struct astnode *
parse(struct listnode *tokens, struct parsetable_item *parsetable);

#endif

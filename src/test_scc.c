#include <stdlib.h>

#include <check.h>

#include "list.h"
#include "scanner.h"
#include "parser.h"

static void
push_node_type_onto_stack(enum astnode_t type, struct listnode **stack)
{
    struct astnode *node;

    node = malloc(sizeof(struct astnode));
    node->type = type;

    list_prepend(stack, node);
}

START_TEST(test_list_append)
{
    struct listnode *a_list;
    char *e1 = "123";
    char *e2 = "456";

    list_init(&a_list);
    list_append(&a_list, e1);
    list_append(&a_list, e2);

    ck_assert_str_eq("123", a_list->data);
}
END_TEST

START_TEST(test_scanner_can_parse_integer_token)
{
    char *content = "1234";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_INTEGER, ((struct token *)tokens->data)->type);
    ck_assert_str_eq("1234", ((struct token *)tokens->data)->value);
}
END_TEST

START_TEST(test_scanner_can_parse_string_token)
{
    char *content = "abcd";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_IDENTIFIER, ((struct token *)tokens->data)->type);
    ck_assert_str_eq("abcd", ((struct token *)tokens->data)->value);
}
END_TEST

START_TEST(test_scanner_can_parse_string_token_with_integers)
{
    char *content = "abc123";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_IDENTIFIER, ((struct token *)tokens->data)->type);
    ck_assert_str_eq("abc123", ((struct token *)tokens->data)->value);
}
END_TEST

START_TEST(test_scanner_can_parse_paren)
{
    char *content = "()";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_LPAREN, ((struct token *)tokens->data)->type);
    ck_assert_int_eq(TOK_RPAREN, ((struct token *)tokens->next->data)->type);
}
END_TEST

START_TEST(test_scanner_can_parse_two_braces)
{
    char *content = "{}";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_LBRACE, ((struct token *)tokens->data)->type);
    ck_assert_int_eq(TOK_RBRACE, ((struct token *)tokens->next->data)->type);
}
END_TEST

START_TEST(test_scanner_can_parse_two_brackets)
{
    char *content = "[]";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_LBRACKET, ((struct token *)tokens->data)->type);
    ck_assert_int_eq(TOK_RBRACKET, ((struct token *)tokens->next->data)->type);
}
END_TEST

START_TEST(test_scanner_can_parse_special_characters)
{
    char *content = ";=+*&'\"/%<>^|?:";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_SEMICOLON, ((struct token *)tokens->data)->type);
    ck_assert_int_eq(TOK_EQUAL, ((struct token *)tokens->next->data)->type);
    ck_assert_int_eq(TOK_PLUS, ((struct token *)tokens->next->next->data)->type);
    ck_assert_int_eq(TOK_ASTERISK, ((struct token *)tokens->next->next->next->data)->type);
    ck_assert_int_eq(TOK_AMPERSAND, ((struct token *)tokens->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_SINGLEQUOTE, ((struct token *)tokens->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_DOUBLEQUOTE, ((struct token *)tokens->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_BACKSLASH, ((struct token *)tokens->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_MOD, ((struct token *)tokens->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_LESSTHAN, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_GREATERTHAN, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_CARET, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_VERTICALBAR, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_QUESTIONMARK, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_COLON, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_scanner_can_parse_combination_tokens)
{
    char *content = "+++=---=->>><<<=>===!=&&||";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_PLUS_PLUS, ((struct token *)tokens->data)->type);
    ck_assert_int_eq(TOK_PLUS_EQUAL, ((struct token *)tokens->next->data)->type);
    ck_assert_int_eq(TOK_MINUS_MINUS, ((struct token *)tokens->next->next->data)->type);
    ck_assert_int_eq(TOK_MINUS_EQUAL, ((struct token *)tokens->next->next->next->data)->type);
    ck_assert_int_eq(TOK_ARROW, ((struct token *)tokens->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_SHIFTRIGHT, ((struct token *)tokens->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_SHIFTLEFT, ((struct token *)tokens->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_LESSTHANEQUAL, ((struct token *)tokens->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_GREATERTHANEQUAL, ((struct token *)tokens->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_EQ, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_NEQ, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_AMPERSAND_AMPERSAND, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_VERTICALBAR_VERTICALBAR, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_scanner_ignores_comment_contents)
{
    char *content = "123/*456*/789";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_INTEGER, ((struct token *)tokens->data)->type);
    ck_assert_str_eq("123", ((struct token *)tokens->data)->value);
    ck_assert_str_eq("789", ((struct token *)tokens->next->data)->value);
}
END_TEST

START_TEST(test_scanner_ignores_comment_contents_around_strings)
{
    char *content = "abc/*def*/ghi";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_IDENTIFIER, ((struct token *)tokens->data)->type);
    ck_assert_str_eq("abc", ((struct token *)tokens->data)->value);
    ck_assert_str_eq("ghi", ((struct token *)tokens->next->data)->value);
}
END_TEST

START_TEST(test_scanner_can_parse_reserved_words)
{
    char *content = "int char goto  continue break  return if else switch case default";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_INT, ((struct token *)tokens->data)->type);
    ck_assert_int_eq(TOK_CHAR, ((struct token *)tokens->next->data)->type);
    ck_assert_int_eq(TOK_GOTO, ((struct token *)tokens->next->next->data)->type);
    ck_assert_int_eq(TOK_CONTINUE, ((struct token *)tokens->next->next->next->data)->type);
    ck_assert_int_eq(TOK_BREAK, ((struct token *)tokens->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_RETURN, ((struct token *)tokens->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_IF, ((struct token *)tokens->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_ELSE, ((struct token *)tokens->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_SWITCH, ((struct token *)tokens->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_CASE, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_DEFAULT, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_constant_reduces_into_primary_expression)
{
    char *content = "32";
    struct listnode *tokens, *stack;
    struct astnode *node;

    list_init(&tokens);
    list_init(&stack);

    /* tokenize input string */
    do_tokenizing(content, strlen(content), &tokens);

    /* construct astnode from tokens  */
    node = shift(tokens->data);
    list_append(&stack, node);

    /* perform single reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_PRIMARY_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_primary_expression_reduces_into_postfix_expression)
{
    struct astnode *node;
    struct listnode *stack;

    node = malloc(sizeof(struct astnode));
    node->type = AST_PRIMARY_EXPRESSION;

    list_init(&stack);
    list_append(&stack, node);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_POSTFIX_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_postfix_expression_and_plus_plus_reduces_into_postfix_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_POSTFIX_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_PLUS_PLUS, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_POSTFIX_EXPRESSION, node->type);

    ck_assert_int_eq(AST_POSTFIX_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_PLUS_PLUS, ((struct astnode *)node->children->next->data)->type);
}
END_TEST

START_TEST(test_parser_postfix_expression_reduces_into_unary_expression)
{
    struct astnode *node;
    struct listnode *stack;

    node = malloc(sizeof(struct astnode));
    node->type = AST_POSTFIX_EXPRESSION;

    list_init(&stack);
    list_append(&stack, node);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_UNARY_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_unary_expression_reduces_into_cast_expression)
{
    struct astnode *node;
    struct listnode *stack;

    node = malloc(sizeof(struct astnode));
    node->type = AST_UNARY_EXPRESSION;

    list_init(&stack);
    list_append(&stack, node);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_CAST_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_cast_expression_reduces_into_multiplicative_expression)
{
    struct astnode *node;
    struct listnode *stack;

    node = malloc(sizeof(struct astnode));
    node->type = AST_CAST_EXPRESSION;

    list_init(&stack);
    list_append(&stack, node);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_MULTIPLICATIVE_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_multiplicative_expression_asterisk_cast_expression_reduces_into_multiplicative_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_MULTIPLICATIVE_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_ASTERISK, &stack);
    push_node_type_onto_stack(AST_CAST_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_MULTIPLICATIVE_EXPRESSION, node->type);

    ck_assert_int_eq(AST_MULTIPLICATIVE_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_ASTERISK, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_CAST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_multiplicative_expression_backslash_cast_expression_reduces_into_multiplicative_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_MULTIPLICATIVE_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_BACKSLASH, &stack);
    push_node_type_onto_stack(AST_CAST_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_MULTIPLICATIVE_EXPRESSION, node->type);

    ck_assert_int_eq(AST_MULTIPLICATIVE_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_BACKSLASH, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_CAST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_multiplicative_expression_mod_cast_expression_reduces_into_multiplicative_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_MULTIPLICATIVE_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_MOD, &stack);
    push_node_type_onto_stack(AST_CAST_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_MULTIPLICATIVE_EXPRESSION, node->type);

    ck_assert_int_eq(AST_MULTIPLICATIVE_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_MOD, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_CAST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_multiplicative_expression_reduces_into_additive_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_MULTIPLICATIVE_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ADDITIVE_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_additive_expression_plus_multiplicative_expression_reduces_into_additive_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_ADDITIVE_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_PLUS, &stack);
    push_node_type_onto_stack(AST_MULTIPLICATIVE_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ADDITIVE_EXPRESSION, node->type);

    ck_assert_int_eq(AST_ADDITIVE_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_PLUS, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_MULTIPLICATIVE_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_additive_expression_minus_multiplicative_expression_reduces_into_additive_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_ADDITIVE_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_MINUS, &stack);
    push_node_type_onto_stack(AST_MULTIPLICATIVE_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ADDITIVE_EXPRESSION, node->type);

    ck_assert_int_eq(AST_ADDITIVE_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_MINUS, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_MULTIPLICATIVE_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_additive_expression_reduces_into_shift_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_ADDITIVE_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_SHIFT_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_shift_expression_shiftleft_additive_expression_reduces_into_shift_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_SHIFT_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SHIFTLEFT, &stack);
    push_node_type_onto_stack(AST_ADDITIVE_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_SHIFT_EXPRESSION, node->type);

    ck_assert_int_eq(AST_SHIFT_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_SHIFTLEFT, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_ADDITIVE_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_shift_expression_shiftright_additive_expression_reduces_into_shift_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_SHIFT_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SHIFTRIGHT, &stack);
    push_node_type_onto_stack(AST_ADDITIVE_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_SHIFT_EXPRESSION, node->type);

    ck_assert_int_eq(AST_SHIFT_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_SHIFTRIGHT, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_ADDITIVE_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_shift_expression_reduces_into_relational_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_SHIFT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_relational_expression_lessthan_shift_expression_reduces_into_relational_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_RELATIONAL_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_LT, &stack);
    push_node_type_onto_stack(AST_SHIFT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, node->type);

    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LT, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_SHIFT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_relational_expression_greaterthan_shift_expression_reduces_into_relational_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_RELATIONAL_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_GT, &stack);
    push_node_type_onto_stack(AST_SHIFT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, node->type);

    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_GT, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_SHIFT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_relational_expression_lessthanequal_shift_expression_reduces_into_relational_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_RELATIONAL_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_LTEQ, &stack);
    push_node_type_onto_stack(AST_SHIFT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, node->type);

    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LTEQ, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_SHIFT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_relational_expression_greaterthanequal_shift_expression_reduces_into_relational_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_RELATIONAL_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_GTEQ, &stack);
    push_node_type_onto_stack(AST_SHIFT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, node->type);

    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_GTEQ, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_SHIFT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_relational_expression_reduces_into_equality_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_RELATIONAL_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_EQUALITY_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_equality_expression_isequal_relational_expression_reduces_into_equality_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_EQUALITY_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_EQ, &stack);
    push_node_type_onto_stack(AST_RELATIONAL_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_EQUALITY_EXPRESSION, node->type);

    ck_assert_int_eq(AST_EQUALITY_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_EQ, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_equality_expression_isnotequal_relational_expression_reduces_into_equality_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_EQUALITY_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_NEQ, &stack);
    push_node_type_onto_stack(AST_RELATIONAL_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_EQUALITY_EXPRESSION, node->type);

    ck_assert_int_eq(AST_EQUALITY_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_NEQ, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_RELATIONAL_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_equality_expression_reduces_into_and_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_EQUALITY_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_AND_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_and_expression_ampersand_equality_expression_reduces_into_and_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_AND_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_AMPERSAND, &stack);
    push_node_type_onto_stack(AST_EQUALITY_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_AND_EXPRESSION, node->type);

    ck_assert_int_eq(AST_AND_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_AMPERSAND, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EQUALITY_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_and_expression_reduces_into_exclusive_or_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_AND_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_EXCLUSIVE_OR_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_exclusive_or_expression_caret_and_expression_reduces_into_and_exclusive_or_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_EXCLUSIVE_OR_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_CARET, &stack);
    push_node_type_onto_stack(AST_AND_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_EXCLUSIVE_OR_EXPRESSION, node->type);

    ck_assert_int_eq(AST_EXCLUSIVE_OR_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_CARET, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_AND_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_exclusive_or_expression_reduces_into_inclusive_or_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_EXCLUSIVE_OR_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_INCLUSIVE_OR_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_inclusive_or_expression_verticalbar_exclusive_or_expression_reduces_into_inclusive_or_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_INCLUSIVE_OR_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_VERTICALBAR, &stack);
    push_node_type_onto_stack(AST_EXCLUSIVE_OR_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_INCLUSIVE_OR_EXPRESSION, node->type);

    ck_assert_int_eq(AST_INCLUSIVE_OR_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_VERTICALBAR, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EXCLUSIVE_OR_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_inclusive_or_expression_reduces_into_logical_and_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_INCLUSIVE_OR_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_LOGICAL_AND_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_logical_and_expression_ampersandampersand_inclusive_or_expression_reduces_into_logical_and_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_LOGICAL_AND_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_AMPERSAND_AMPERSAND, &stack);
    push_node_type_onto_stack(AST_INCLUSIVE_OR_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_LOGICAL_AND_EXPRESSION, node->type);

    ck_assert_int_eq(AST_LOGICAL_AND_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_AMPERSAND_AMPERSAND, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_INCLUSIVE_OR_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_logical_and_expression_reduces_into_logical_or_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_LOGICAL_AND_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_LOGICAL_OR_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_logical_or_expression_verticalbarverticalbar_logical_and_expression_reduces_into_logical_or_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_LOGICAL_OR_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_VERTICALBAR_VERTICALBAR, &stack);
    push_node_type_onto_stack(AST_LOGICAL_AND_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_LOGICAL_OR_EXPRESSION, node->type);

    ck_assert_int_eq(AST_LOGICAL_OR_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_VERTICALBAR_VERTICALBAR, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_LOGICAL_AND_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_logical_or_expression_reduces_into_conditional_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_LOGICAL_OR_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_CONDITIONAL_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_logical_or_expression_questionmark_expression_colon_conditional_expression_reduces_into_conditional_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_LOGICAL_OR_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_QUESTIONMARK, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_COLON, &stack);
    push_node_type_onto_stack(AST_CONDITIONAL_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_CONDITIONAL_EXPRESSION, node->type);

    ck_assert_int_eq(AST_LOGICAL_OR_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_QUESTIONMARK, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_COLON, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_CONDITIONAL_EXPRESSION, ((struct astnode *)node->children->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_conditional_expression_reduces_into_assignment_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_CONDITIONAL_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_unary_expression_equal_assignment_expression_reduces_into_assignment_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_UNARY_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_EQUAL, &stack);
    push_node_type_onto_stack(AST_ASSIGNMENT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, node->type);

    ck_assert_int_eq(AST_UNARY_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_EQUAL, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_unary_expression_asteriskequal_assignment_expression_reduces_into_assignment_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_UNARY_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_ASTERISK_EQUAL, &stack);
    push_node_type_onto_stack(AST_ASSIGNMENT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, node->type);

    ck_assert_int_eq(AST_UNARY_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_ASTERISK_EQUAL, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_unary_expression_backslashequal_assignment_expression_reduces_into_assignment_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_UNARY_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_BACKSLASH_EQUAL, &stack);
    push_node_type_onto_stack(AST_ASSIGNMENT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, node->type);

    ck_assert_int_eq(AST_UNARY_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_BACKSLASH_EQUAL, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_unary_expression_modequal_assignment_expression_reduces_into_assignment_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_UNARY_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_MOD_EQUAL, &stack);
    push_node_type_onto_stack(AST_ASSIGNMENT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, node->type);

    ck_assert_int_eq(AST_UNARY_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_MOD_EQUAL, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_unary_expression_plusequal_assignment_expression_reduces_into_assignment_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_UNARY_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_PLUS_EQUAL, &stack);
    push_node_type_onto_stack(AST_ASSIGNMENT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, node->type);

    ck_assert_int_eq(AST_UNARY_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_PLUS_EQUAL, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_unary_expression_minusequal_assignment_expression_reduces_into_assignment_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_UNARY_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_MINUS_EQUAL, &stack);
    push_node_type_onto_stack(AST_ASSIGNMENT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, node->type);

    ck_assert_int_eq(AST_UNARY_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_MINUS_EQUAL, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_assignment_expression_reduces_into_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_ASSIGNMENT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_EXPRESSION, node->type);
}
END_TEST

START_TEST(test_parser_expression_comma_assignment_expression_reduces_into_expression)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_COMMA, &stack);
    push_node_type_onto_stack(AST_ASSIGNMENT_EXPRESSION, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_EXPRESSION, node->type);

    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_COMMA, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_ASSIGNMENT_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_goto_identifier_semicomma_reduces_into_jump_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_GOTO, &stack);
    push_node_type_onto_stack(AST_IDENTIFIER, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_JUMP_STATEMENT, node->type);

    ck_assert_int_eq(AST_GOTO, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_IDENTIFIER, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_continue_semicomma_reduces_into_jump_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_CONTINUE, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_JUMP_STATEMENT, node->type);

    ck_assert_int_eq(AST_CONTINUE, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->data)->type);
}
END_TEST

START_TEST(test_parser_break_semicomma_reduces_into_jump_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_BREAK, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_JUMP_STATEMENT, node->type);

    ck_assert_int_eq(AST_BREAK, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->data)->type);
}
END_TEST

START_TEST(test_parser_return_semicomma_reduces_into_jump_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_RETURN, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_JUMP_STATEMENT, node->type);

    ck_assert_int_eq(AST_RETURN, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->data)->type);
}
END_TEST

START_TEST(test_parser_return_expression_semicomma_reduces_into_jump_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_RETURN, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_JUMP_STATEMENT, node->type);

    ck_assert_int_eq(AST_RETURN, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_while_lparen_expression_rparen_statement_reduces_into_iteration_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_WHILE, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, node->type);

    ck_assert_int_eq(AST_WHILE, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_do_statement_while_lparen_expression_rparen_semicolon_reduces_into_iteration_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_DO, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);
    push_node_type_onto_stack(AST_WHILE, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, node->type);

    ck_assert_int_eq(AST_DO, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_WHILE, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_for_lparen_semicolon_semicolon_rparen_statement_reduces_into_iteration_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_FOR, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, node->type);

    ck_assert_int_eq(AST_FOR, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_for_lparen_expression_semicolon_semicolon_rparen_statement_reduces_into_iteration_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_FOR, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, node->type);

    ck_assert_int_eq(AST_FOR, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_for_lparen_semicolon_expression_semicolon_rparen_statement_reduces_into_iteration_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_FOR, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, node->type);

    ck_assert_int_eq(AST_FOR, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_for_lparen_semicolon_semicolon_expression_rparen_statement_reduces_into_iteration_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_FOR, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, node->type);

    ck_assert_int_eq(AST_FOR, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_for_lparen_expression_semicolon_expression_semicolon_rparen_statement_reduces_into_iteration_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_FOR, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, node->type);

    ck_assert_int_eq(AST_FOR, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_for_lparen_expression_semicolon_semicolon_expression_rparen_statement_reduces_into_iteration_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_FOR, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, node->type);

    ck_assert_int_eq(AST_FOR, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_for_lparen_semicolon_expression_semicolon_expression_rparen_statement_reduces_into_iteration_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_FOR, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, node->type);

    ck_assert_int_eq(AST_FOR, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_for_lparen_expression_semicolon_expression_semicolon_expression_rparen_statement_reduces_into_iteration_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_FOR, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, node->type);

    ck_assert_int_eq(AST_FOR, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_if_lparen_expression_rparen_statement_reduces_into_selection_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_IF, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_SELECTION_STATEMENT, node->type);

    ck_assert_int_eq(AST_IF, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_switch_lparen_expression_rparen_statement_reduces_into_selection_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_IF, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_SELECTION_STATEMENT, node->type);

    ck_assert_int_eq(AST_IF, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_if_lparen_expression_rparen_statement_else_statement_reduces_into_selection_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_IF, &stack);
    push_node_type_onto_stack(AST_LPAREN, &stack);
    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_RPAREN, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);
    push_node_type_onto_stack(AST_ELSE, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_SELECTION_STATEMENT, node->type);

    ck_assert_int_eq(AST_IF, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_LPAREN, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_RPAREN, ((struct astnode *)node->children->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_ELSE, ((struct astnode *)node->children->next->next->next->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_statement_reduces_into_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_STATEMENT_LIST, node->type);

    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->data)->type);
}
END_TEST

START_TEST(test_parser_statement_list_statement_reduces_into_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_STATEMENT_LIST, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_STATEMENT_LIST, node->type);

    ck_assert_int_eq(AST_STATEMENT_LIST, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->data)->type);
}
END_TEST

START_TEST(test_parser_lbrace_rbrace_reduces_into_compound_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_LBRACE, &stack);
    push_node_type_onto_stack(AST_RBRACE, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_COMPOUND_STATEMENT, node->type);

    ck_assert_int_eq(AST_LBRACE, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_RBRACE, ((struct astnode *)node->children->next->data)->type);
}
END_TEST

START_TEST(test_parser_lbrace_declaration_list_rbrace_reduces_into_compound_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_LBRACE, &stack);
    push_node_type_onto_stack(AST_DECLARATION_LIST, &stack);
    push_node_type_onto_stack(AST_RBRACE, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_COMPOUND_STATEMENT, node->type);

    ck_assert_int_eq(AST_LBRACE, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_DECLARATION_LIST, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_RBRACE, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_lbrace_statement_list_rbrace_reduces_into_compound_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_LBRACE, &stack);
    push_node_type_onto_stack(AST_STATEMENT_LIST, &stack);
    push_node_type_onto_stack(AST_RBRACE, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_COMPOUND_STATEMENT, node->type);

    ck_assert_int_eq(AST_LBRACE, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_STATEMENT_LIST, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_RBRACE, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_lbrace_declaration_list_statement_list_rbrace_reduces_into_compound_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_LBRACE, &stack);
    push_node_type_onto_stack(AST_DECLARATION_LIST, &stack);
    push_node_type_onto_stack(AST_STATEMENT_LIST, &stack);
    push_node_type_onto_stack(AST_RBRACE, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_COMPOUND_STATEMENT, node->type);

    ck_assert_int_eq(AST_LBRACE, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_DECLARATION_LIST, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT_LIST, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_RBRACE, ((struct astnode *)node->children->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_semicolon_reduces_into_expression_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_SEMICOLON, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_EXPRESSION_STATEMENT, node->type);

    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->data)->type);
}
END_TEST

START_TEST(test_parser_expression_semicolon_reduces_into_expression_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_SEMICOLON, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_EXPRESSION_STATEMENT, node->type);

    ck_assert_int_eq(AST_EXPRESSION, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_SEMICOLON, ((struct astnode *)node->children->next->data)->type);
}
END_TEST

START_TEST(test_parser_identifier_colon_statement_reduces_into_labeled_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_IDENTIFIER, &stack);
    push_node_type_onto_stack(AST_COLON, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_LABELED_STATEMENT, node->type);

    ck_assert_int_eq(AST_IDENTIFIER, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_COLON, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_case_constant_expression_colon_statement_reduces_into_labeled_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_CASE, &stack);
    push_node_type_onto_stack(AST_CONDITIONAL_EXPRESSION, &stack);
    push_node_type_onto_stack(AST_COLON, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_LABELED_STATEMENT, node->type);

    ck_assert_int_eq(AST_CASE, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_CONDITIONAL_EXPRESSION, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_COLON, ((struct astnode *)node->children->next->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_default_colon_statement_reduces_into_labeled_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_DEFAULT, &stack);
    push_node_type_onto_stack(AST_COLON, &stack);
    push_node_type_onto_stack(AST_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_LABELED_STATEMENT, node->type);

    ck_assert_int_eq(AST_DEFAULT, ((struct astnode *)node->children->data)->type);
    ck_assert_int_eq(AST_COLON, ((struct astnode *)node->children->next->data)->type);
    ck_assert_int_eq(AST_STATEMENT, ((struct astnode *)node->children->next->next->data)->type);
}
END_TEST

START_TEST(test_parser_labeled_statement_reduces_into_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_LABELED_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_STATEMENT, node->type);

    ck_assert_int_eq(AST_LABELED_STATEMENT, ((struct astnode *)node->children->data)->type);
}
END_TEST

START_TEST(test_parser_expression_statement_reduces_into_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_EXPRESSION_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_STATEMENT, node->type);

    ck_assert_int_eq(AST_EXPRESSION_STATEMENT, ((struct astnode *)node->children->data)->type);
}
END_TEST

START_TEST(test_parser_compound_statement_reduces_into_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_COMPOUND_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_STATEMENT, node->type);

    ck_assert_int_eq(AST_COMPOUND_STATEMENT, ((struct astnode *)node->children->data)->type);
}
END_TEST

START_TEST(test_parser_selection_statement_reduces_into_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_SELECTION_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_STATEMENT, node->type);

    ck_assert_int_eq(AST_SELECTION_STATEMENT, ((struct astnode *)node->children->data)->type);
}
END_TEST

START_TEST(test_parser_iteration_statement_reduces_into_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_ITERATION_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_STATEMENT, node->type);

    ck_assert_int_eq(AST_ITERATION_STATEMENT, ((struct astnode *)node->children->data)->type);
}
END_TEST

START_TEST(test_parser_jump_statement_reduces_into_statement)
{
    struct astnode *node;
    struct listnode *stack;

    list_init(&stack);

    push_node_type_onto_stack(AST_JUMP_STATEMENT, &stack);

    /* perform next reduction on astnode */
    node = reduce(&stack);

    ck_assert_int_eq(AST_STATEMENT, node->type);

    ck_assert_int_eq(AST_JUMP_STATEMENT, ((struct astnode *)node->children->data)->type);
}
END_TEST

int
main(void)
{
    Suite *suite = suite_create("scc");
    TCase *testcase = tcase_create("scc");
    SRunner *runner = srunner_create(suite);

    suite_add_tcase(suite, testcase);
    tcase_add_test(testcase, test_list_append);
    tcase_add_test(testcase, test_scanner_can_parse_integer_token);
    tcase_add_test(testcase, test_scanner_can_parse_string_token);
    tcase_add_test(testcase, test_scanner_can_parse_string_token_with_integers);
    tcase_add_test(testcase, test_scanner_can_parse_paren);
    tcase_add_test(testcase, test_scanner_can_parse_two_braces);
    tcase_add_test(testcase, test_scanner_can_parse_two_brackets);
    tcase_add_test(testcase, test_scanner_can_parse_special_characters);
    tcase_add_test(testcase, test_scanner_can_parse_combination_tokens);
    tcase_add_test(testcase, test_scanner_ignores_comment_contents);
    tcase_add_test(testcase, test_scanner_ignores_comment_contents_around_strings);
    tcase_add_test(testcase, test_scanner_can_parse_reserved_words);
    tcase_add_test(testcase, test_parser_constant_reduces_into_primary_expression);
    tcase_add_test(testcase, test_parser_primary_expression_reduces_into_postfix_expression);
    tcase_add_test(testcase, test_parser_postfix_expression_reduces_into_unary_expression);
    tcase_add_test(testcase, test_parser_postfix_expression_and_plus_plus_reduces_into_postfix_expression);
    tcase_add_test(testcase, test_parser_unary_expression_reduces_into_cast_expression);
    tcase_add_test(testcase, test_parser_cast_expression_reduces_into_multiplicative_expression);
    tcase_add_test(testcase, test_parser_multiplicative_expression_asterisk_cast_expression_reduces_into_multiplicative_expression);
    tcase_add_test(testcase, test_parser_multiplicative_expression_backslash_cast_expression_reduces_into_multiplicative_expression);
    tcase_add_test(testcase, test_parser_multiplicative_expression_mod_cast_expression_reduces_into_multiplicative_expression);
    tcase_add_test(testcase, test_parser_multiplicative_expression_reduces_into_additive_expression);
    tcase_add_test(testcase, test_parser_additive_expression_plus_multiplicative_expression_reduces_into_additive_expression);
    tcase_add_test(testcase, test_parser_additive_expression_minus_multiplicative_expression_reduces_into_additive_expression);
    tcase_add_test(testcase, test_parser_additive_expression_reduces_into_shift_expression);
    tcase_add_test(testcase, test_parser_shift_expression_shiftleft_additive_expression_reduces_into_shift_expression);
    tcase_add_test(testcase, test_parser_shift_expression_shiftright_additive_expression_reduces_into_shift_expression);
    tcase_add_test(testcase, test_parser_shift_expression_reduces_into_relational_expression);
    tcase_add_test(testcase, test_parser_relational_expression_lessthan_shift_expression_reduces_into_relational_expression);
    tcase_add_test(testcase, test_parser_relational_expression_greaterthan_shift_expression_reduces_into_relational_expression);
    tcase_add_test(testcase, test_parser_relational_expression_lessthanequal_shift_expression_reduces_into_relational_expression);
    tcase_add_test(testcase, test_parser_relational_expression_greaterthanequal_shift_expression_reduces_into_relational_expression);
    tcase_add_test(testcase, test_parser_relational_expression_reduces_into_equality_expression);
    tcase_add_test(testcase, test_parser_equality_expression_isequal_relational_expression_reduces_into_equality_expression);
    tcase_add_test(testcase, test_parser_equality_expression_isnotequal_relational_expression_reduces_into_equality_expression);
    tcase_add_test(testcase, test_parser_equality_expression_reduces_into_and_expression);
    tcase_add_test(testcase, test_parser_and_expression_ampersand_equality_expression_reduces_into_and_expression);
    tcase_add_test(testcase, test_parser_and_expression_reduces_into_exclusive_or_expression);
    tcase_add_test(testcase, test_parser_exclusive_or_expression_caret_and_expression_reduces_into_and_exclusive_or_expression);
    tcase_add_test(testcase, test_parser_exclusive_or_expression_reduces_into_inclusive_or_expression);
    tcase_add_test(testcase, test_parser_inclusive_or_expression_verticalbar_exclusive_or_expression_reduces_into_inclusive_or_expression);
    tcase_add_test(testcase, test_parser_inclusive_or_expression_reduces_into_logical_and_expression);
    tcase_add_test(testcase, test_parser_logical_and_expression_ampersandampersand_inclusive_or_expression_reduces_into_logical_and_expression);
    tcase_add_test(testcase, test_parser_logical_and_expression_reduces_into_logical_or_expression);
    tcase_add_test(testcase, test_parser_logical_or_expression_verticalbarverticalbar_logical_and_expression_reduces_into_logical_or_expression);
    tcase_add_test(testcase, test_parser_logical_or_expression_reduces_into_conditional_expression);
    tcase_add_test(testcase, test_parser_logical_or_expression_questionmark_expression_colon_conditional_expression_reduces_into_conditional_expression);
    tcase_add_test(testcase, test_parser_conditional_expression_reduces_into_assignment_expression);
    tcase_add_test(testcase, test_parser_unary_expression_equal_assignment_expression_reduces_into_assignment_expression);
    tcase_add_test(testcase, test_parser_unary_expression_asteriskequal_assignment_expression_reduces_into_assignment_expression);
    tcase_add_test(testcase, test_parser_unary_expression_backslashequal_assignment_expression_reduces_into_assignment_expression);
    tcase_add_test(testcase, test_parser_unary_expression_modequal_assignment_expression_reduces_into_assignment_expression);
    tcase_add_test(testcase, test_parser_unary_expression_plusequal_assignment_expression_reduces_into_assignment_expression);
    tcase_add_test(testcase, test_parser_unary_expression_minusequal_assignment_expression_reduces_into_assignment_expression);
    tcase_add_test(testcase, test_parser_assignment_expression_reduces_into_expression);
    tcase_add_test(testcase, test_parser_expression_comma_assignment_expression_reduces_into_expression);
    tcase_add_test(testcase, test_parser_goto_identifier_semicomma_reduces_into_jump_statement);
    tcase_add_test(testcase, test_parser_continue_semicomma_reduces_into_jump_statement);
    tcase_add_test(testcase, test_parser_break_semicomma_reduces_into_jump_statement);
    tcase_add_test(testcase, test_parser_return_semicomma_reduces_into_jump_statement);
    tcase_add_test(testcase, test_parser_return_expression_semicomma_reduces_into_jump_statement);
    tcase_add_test(testcase, test_parser_while_lparen_expression_rparen_statement_reduces_into_iteration_statement);
    tcase_add_test(testcase, test_parser_do_statement_while_lparen_expression_rparen_semicolon_reduces_into_iteration_statement);
    tcase_add_test(testcase, test_parser_for_lparen_semicolon_semicolon_rparen_statement_reduces_into_iteration_statement);
    tcase_add_test(testcase, test_parser_for_lparen_expression_semicolon_semicolon_rparen_statement_reduces_into_iteration_statement);
    tcase_add_test(testcase, test_parser_for_lparen_semicolon_expression_semicolon_rparen_statement_reduces_into_iteration_statement);
    tcase_add_test(testcase, test_parser_for_lparen_semicolon_semicolon_expression_rparen_statement_reduces_into_iteration_statement);
    tcase_add_test(testcase, test_parser_for_lparen_expression_semicolon_expression_semicolon_rparen_statement_reduces_into_iteration_statement);
    tcase_add_test(testcase, test_parser_for_lparen_expression_semicolon_semicolon_expression_rparen_statement_reduces_into_iteration_statement);
    tcase_add_test(testcase, test_parser_for_lparen_semicolon_expression_semicolon_expression_rparen_statement_reduces_into_iteration_statement);
    tcase_add_test(testcase, test_parser_for_lparen_expression_semicolon_expression_semicolon_expression_rparen_statement_reduces_into_iteration_statement);
    tcase_add_test(testcase, test_parser_if_lparen_expression_rparen_statement_reduces_into_selection_statement);
    tcase_add_test(testcase, test_parser_switch_lparen_expression_rparen_statement_reduces_into_selection_statement);
    tcase_add_test(testcase, test_parser_if_lparen_expression_rparen_statement_else_statement_reduces_into_selection_statement);
    tcase_add_test(testcase, test_parser_statement_reduces_into_statement);
    tcase_add_test(testcase, test_parser_statement_list_statement_reduces_into_statement);
    tcase_add_test(testcase, test_parser_lbrace_rbrace_reduces_into_compound_statement);
    tcase_add_test(testcase, test_parser_lbrace_declaration_list_rbrace_reduces_into_compound_statement);
    tcase_add_test(testcase, test_parser_lbrace_statement_list_rbrace_reduces_into_compound_statement);
    tcase_add_test(testcase, test_parser_lbrace_declaration_list_statement_list_rbrace_reduces_into_compound_statement);
    tcase_add_test(testcase, test_parser_semicolon_reduces_into_expression_statement);
    tcase_add_test(testcase, test_parser_expression_semicolon_reduces_into_expression_statement);
    tcase_add_test(testcase, test_parser_identifier_colon_statement_reduces_into_labeled_statement);
    tcase_add_test(testcase, test_parser_case_constant_expression_colon_statement_reduces_into_labeled_statement);
    tcase_add_test(testcase, test_parser_default_colon_statement_reduces_into_labeled_statement);
    tcase_add_test(testcase, test_parser_labeled_statement_reduces_into_statement);
    tcase_add_test(testcase, test_parser_expression_statement_reduces_into_statement);
    tcase_add_test(testcase, test_parser_compound_statement_reduces_into_statement);
    tcase_add_test(testcase, test_parser_selection_statement_reduces_into_statement);
    tcase_add_test(testcase, test_parser_iteration_statement_reduces_into_statement);
    tcase_add_test(testcase, test_parser_jump_statement_reduces_into_statement);

    srunner_run_all(runner, CK_ENV);
    return 0;
}

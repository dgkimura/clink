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

START_TEST(test_head_terminal_values_on_constant)
{
    struct listnode *terminals, *checked_nodes;

    list_init(&terminals);
    list_init(&checked_nodes);

    head_terminal_values(AST_CONSTANT, &checked_nodes, &terminals);

    ck_assert_int_eq(AST_INTEGER_CONSTANT, (int)terminals->data);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, (int)terminals->next->data);
}
END_TEST

START_TEST(test_head_terminal_values_on_primary_expression)
{
    struct listnode *terminals, *checked_nodes;

    list_init(&terminals);
    list_init(&checked_nodes);

    head_terminal_values(AST_PRIMARY_EXPRESSION, &checked_nodes, &terminals);

    ck_assert_int_eq(AST_IDENTIFIER, (int)terminals->data);
    ck_assert_int_eq(AST_INTEGER_CONSTANT, (int)terminals->next->data);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, (int)terminals->next->next->data);
}
END_TEST

START_TEST(test_head_terminal_values_on_postfix_expression)
{
    struct listnode *terminals, *checked_nodes;

    list_init(&terminals);
    list_init(&checked_nodes);

    head_terminal_values(AST_POSTFIX_EXPRESSION, &checked_nodes, &terminals);

    ck_assert_int_eq(AST_IDENTIFIER, (int)terminals->data);
    ck_assert_int_eq(AST_INTEGER_CONSTANT, (int)terminals->next->data);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, (int)terminals->next->next->data);
}
END_TEST

START_TEST(test_head_terminal_values_on_unary_expression)
{
    struct listnode *terminals, *checked_nodes;

    list_init(&terminals);
    list_init(&checked_nodes);

    head_terminal_values(AST_UNARY_EXPRESSION, &checked_nodes, &terminals);

    ck_assert_int_eq(AST_PLUS_PLUS, (int)terminals->data);
    ck_assert_int_eq(AST_MINUS_MINUS, (int)terminals->next->data);
    ck_assert_int_eq(AST_AMPERSAND, (int)terminals->next->next->data);
    ck_assert_int_eq(AST_ASTERISK, (int)terminals->next->next->next->data);
    ck_assert_int_eq(AST_PLUS, (int)terminals->next->next->next->next->data);
    ck_assert_int_eq(AST_MINUS, (int)terminals->next->next->next->next->next->data);
    ck_assert_int_eq(AST_IDENTIFIER, (int)terminals->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_INTEGER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->next->data);
}
END_TEST

START_TEST(test_head_terminal_values_on_cast_expression)
{
    struct listnode *terminals, *checked_nodes;

    list_init(&terminals);
    list_init(&checked_nodes);

    head_terminal_values(AST_CAST_EXPRESSION, &checked_nodes, &terminals);

    ck_assert_int_eq(AST_PLUS_PLUS, (int)terminals->data);
    ck_assert_int_eq(AST_MINUS_MINUS, (int)terminals->next->data);
    ck_assert_int_eq(AST_AMPERSAND, (int)terminals->next->next->data);
    ck_assert_int_eq(AST_ASTERISK, (int)terminals->next->next->next->data);
    ck_assert_int_eq(AST_PLUS, (int)terminals->next->next->next->next->data);
    ck_assert_int_eq(AST_MINUS, (int)terminals->next->next->next->next->next->data);
    ck_assert_int_eq(AST_IDENTIFIER, (int)terminals->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_INTEGER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->next->data);
}
END_TEST

START_TEST(test_head_terminal_values_on_multiplicative_expression)
{
    struct listnode *terminals, *checked_nodes;

    list_init(&terminals);
    list_init(&checked_nodes);

    head_terminal_values(AST_MULTIPLICATIVE_EXPRESSION, &checked_nodes, &terminals);

    ck_assert_int_eq(AST_PLUS_PLUS, (int)terminals->data);
    ck_assert_int_eq(AST_MINUS_MINUS, (int)terminals->next->data);
    ck_assert_int_eq(AST_AMPERSAND, (int)terminals->next->next->data);
    ck_assert_int_eq(AST_ASTERISK, (int)terminals->next->next->next->data);
    ck_assert_int_eq(AST_PLUS, (int)terminals->next->next->next->next->data);
    ck_assert_int_eq(AST_MINUS, (int)terminals->next->next->next->next->next->data);
    ck_assert_int_eq(AST_IDENTIFIER, (int)terminals->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_INTEGER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->next->data);
}
END_TEST

START_TEST(test_head_terminal_values_on_additive_expression)
{
    struct listnode *terminals, *checked_nodes;

    list_init(&terminals);
    list_init(&checked_nodes);

    head_terminal_values(AST_ADDITIVE_EXPRESSION, &checked_nodes, &terminals);

    ck_assert_int_eq(AST_PLUS_PLUS, (int)terminals->data);
    ck_assert_int_eq(AST_MINUS_MINUS, (int)terminals->next->data);
    ck_assert_int_eq(AST_AMPERSAND, (int)terminals->next->next->data);
    ck_assert_int_eq(AST_ASTERISK, (int)terminals->next->next->next->data);
    ck_assert_int_eq(AST_PLUS, (int)terminals->next->next->next->next->data);
    ck_assert_int_eq(AST_MINUS, (int)terminals->next->next->next->next->next->data);
    ck_assert_int_eq(AST_IDENTIFIER, (int)terminals->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_INTEGER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->next->data);
}
END_TEST

START_TEST(test_head_terminal_values_on_shift_expression)
{
    struct listnode *terminals, *checked_nodes;

    list_init(&terminals);
    list_init(&checked_nodes);

    head_terminal_values(AST_SHIFT_EXPRESSION, &checked_nodes, &terminals);

    ck_assert_int_eq(AST_PLUS_PLUS, (int)terminals->data);
    ck_assert_int_eq(AST_MINUS_MINUS, (int)terminals->next->data);
    ck_assert_int_eq(AST_AMPERSAND, (int)terminals->next->next->data);
    ck_assert_int_eq(AST_ASTERISK, (int)terminals->next->next->next->data);
    ck_assert_int_eq(AST_PLUS, (int)terminals->next->next->next->next->data);
    ck_assert_int_eq(AST_MINUS, (int)terminals->next->next->next->next->next->data);
    ck_assert_int_eq(AST_IDENTIFIER, (int)terminals->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_INTEGER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->next->data);
}
END_TEST

START_TEST(test_head_terminal_values_on_relational_expression)
{
    struct listnode *terminals, *checked_nodes;

    list_init(&terminals);
    list_init(&checked_nodes);

    head_terminal_values(AST_RELATIONAL_EXPRESSION, &checked_nodes, &terminals);

    ck_assert_int_eq(AST_PLUS_PLUS, (int)terminals->data);
    ck_assert_int_eq(AST_MINUS_MINUS, (int)terminals->next->data);
    ck_assert_int_eq(AST_AMPERSAND, (int)terminals->next->next->data);
    ck_assert_int_eq(AST_ASTERISK, (int)terminals->next->next->next->data);
    ck_assert_int_eq(AST_PLUS, (int)terminals->next->next->next->next->data);
    ck_assert_int_eq(AST_MINUS, (int)terminals->next->next->next->next->next->data);
    ck_assert_int_eq(AST_IDENTIFIER, (int)terminals->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_INTEGER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, (int)terminals->next->next->next->next->next->next->next->next->data);
}
END_TEST

START_TEST(test_head_terminal_values_on_specifier_qualifier_list)
{
    struct listnode *terminals, *checked_nodes;

    list_init(&terminals);
    list_init(&checked_nodes);

    head_terminal_values(AST_SPECIFIER_QUALIFIER_LIST, &checked_nodes, &terminals);

    ck_assert_int_eq(AST_VOID, (int)terminals->data);
    ck_assert_int_eq(AST_CHAR, (int)terminals->next->data);
    ck_assert_int_eq(AST_SHORT, (int)terminals->next->next->data);
    ck_assert_int_eq(AST_INT, (int)terminals->next->next->next->data);
    ck_assert_int_eq(AST_LONG, (int)terminals->next->next->next->next->data);
    ck_assert_int_eq(AST_FLOAT, (int)terminals->next->next->next->next->next->data);
    ck_assert_int_eq(AST_DOUBLE, (int)terminals->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_SIGNED, (int)terminals->next->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_UNSIGNED, (int)terminals->next->next->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_STRUCT, (int)terminals->next->next->next->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_UNION, (int)terminals->next->next->next->next->next->next->next->next->next->next->data);
    ck_assert_int_eq(AST_ENUM, (int)terminals->next->next->next->next->next->next->next->next->next->next->next->data);

    // FIXME: update this test...
    //ck_assert_int_eq(AST_CONST, (int)terminals->next->next->next->next->next->next->next->next->next->next->next->next->data);
    //ck_assert_int_eq(AST_VOLATILE, (int)terminals->next->next->next->next->next->next->next->next->next->next->next->next->next->data);
}
END_TEST

START_TEST(test_generate_items_on_constant)
{
    struct listnode *items;
    list_init(&items);

    generate_items(AST_CONSTANT, NULL, &items);

    ck_assert_int_eq(AST_INTEGER_CONSTANT, ((struct item *)items->data)->rewrite_rule->nodes[0]);
    ck_assert_int_eq(0, ((struct item *)items->data)->cursor_position);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, ((struct item *)items->next->data)->rewrite_rule->nodes[0]);
    ck_assert_int_eq(0, ((struct item *)items->next->data)->cursor_position);
}
END_TEST

START_TEST(test_generate_items_on_primary_expression)
{
    struct listnode *items;
    list_init(&items);

    generate_items(AST_PRIMARY_EXPRESSION, NULL, &items);

    ck_assert_int_eq(AST_IDENTIFIER, ((struct item *)items->data)->rewrite_rule->nodes[0]);
    ck_assert_int_eq(0, ((struct item *)items->data)->cursor_position);
    ck_assert_int_eq(AST_CONSTANT, ((struct item *)items->next->data)->rewrite_rule->nodes[0]);
    ck_assert_int_eq(0, ((struct item *)items->next->data)->cursor_position);
    ck_assert_int_eq(AST_INTEGER_CONSTANT, ((struct item *)items->next->next->data)->rewrite_rule->nodes[0]);
    ck_assert_int_eq(0, ((struct item *)items->next->next->data)->cursor_position);
    ck_assert_int_eq(AST_CHARACTER_CONSTANT, ((struct item *)items->next->next->next->data)->rewrite_rule->nodes[0]);
    ck_assert_int_eq(0, ((struct item *)items->next->next->next->data)->cursor_position);
}
END_TEST

static void
assert_rules_equal(struct rule r1, struct rule r2)
{
    int i;

    ck_assert_int_eq(r1.type, r2.type);
    ck_assert_int_eq(r1.length_of_nodes, r2.length_of_nodes);

    for (i=0; i<r1.length_of_nodes; i++)
    {
        ck_assert_int_eq(r1.nodes[i], r2.nodes[i]);
    }
}

START_TEST(test_generate_items_on_postfix_expression)
{
    struct listnode *items;
    list_init(&items);

    generate_items(AST_POSTFIX_EXPRESSION, NULL, &items);

    assert_rules_equal(
        (struct rule) { AST_POSTFIX_EXPRESSION, 3, { AST_POSTFIX_EXPRESSION, AST_ARROW, AST_IDENTIFIER } },
        *((struct item *)items->data)->rewrite_rule);

    assert_rules_equal(
        (struct rule) { AST_POSTFIX_EXPRESSION, 3, { AST_POSTFIX_EXPRESSION, AST_ARROW, AST_IDENTIFIER } },
        *((struct item *)items->next->data)->rewrite_rule);
    ck_assert_int_eq(AST_ARROW, (int)((struct item *)items->next->data)->lookahead->data);
}
END_TEST

START_TEST(test_generate_items_on_unary_expression)
{
    struct listnode *items;
    list_init(&items);

    generate_items(AST_UNARY_EXPRESSION, NULL, &items);

    assert_rules_equal(
        (struct rule){ AST_UNARY_EXPRESSION, 2, { AST_PLUS_PLUS, AST_UNARY_EXPRESSION } },
        *((struct item *)items->data)->rewrite_rule);
    assert_rules_equal(
        (struct rule){ AST_UNARY_EXPRESSION, 2, { AST_MINUS_MINUS, AST_UNARY_EXPRESSION } },
        *((struct item *)items->next->data)->rewrite_rule);
    assert_rules_equal(
        (struct rule){ AST_UNARY_EXPRESSION, 2, { AST_AMPERSAND, AST_CAST_EXPRESSION } },
        *((struct item *)items->next->next->data)->rewrite_rule);
    assert_rules_equal(
        (struct rule){ AST_UNARY_EXPRESSION, 2, { AST_ASTERISK, AST_CAST_EXPRESSION } },
        *((struct item *)items->next->next->next->data)->rewrite_rule);
}
END_TEST

START_TEST(test_generate_transitions_increments_cursor_position)
{
    struct state *state;
    struct item *item;

    state = malloc(sizeof(struct state));;
    memset(state, 0, sizeof(struct state));

    item = malloc(sizeof(struct item));;
    item->rewrite_rule = &(struct rule) { AST_CONSTANT, 1, { AST_INTEGER_CONSTANT} };
    item->cursor_position = 0;

    list_append(&state->items, item);

    generate_transitions(state);

    /* next state should increment the cursor position */
    ck_assert_int_eq(1, ((struct item *)(state->links[INDEX(AST_INTEGER_CONSTANT)]->items->data))->cursor_position);
}
END_TEST

START_TEST(test_parser_state_contains_item)
{
    struct state *state;
    struct item item;

    item.rewrite_rule = NULL;
    item.cursor_position = 42;
    item.lookahead = NULL;

    state = generate_states();

    ck_assert_int_eq(0, state_contains_item(state, &item));
    ck_assert_int_eq(1, state_contains_item(state, (struct item *)state->items->data));
}
END_TEST

START_TEST(test_parser_generate_states)
{
    struct state *state;

    state = generate_states();

    ck_assert_int_eq(0, state->identifier);
    ck_assert_int_eq(0, index_of_state(state));
    ck_assert_int_eq(1, index_of_state(state->links[2]));
    ck_assert_int_eq(2, index_of_state(state->links[12]));
}
END_TEST

START_TEST(test_parser_can_parse_simple_declaration)
{
    struct astnode *ast;
    struct listnode *tokens;
    char *content;
    list_init(&tokens);

    init_parsetable();

    /*
     * parse global variable declaration
     */
    content = "int identifier;";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);

    list_init(&tokens);

    /*
     * parse global variable declaration with multiple specifiers
     */
    content = "static int identifier;";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);
}
END_TEST

START_TEST(test_parser_can_parse_multiple_simple_declarations)
{
    struct astnode *ast;
    struct listnode *tokens;
    char *content;
    list_init(&tokens);

    init_parsetable();

    /*
     * parse global variable declaration
     */
    content = "int identifier;"
              "long identifier;";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);
}
END_TEST

START_TEST(test_parser_can_parse_function)
{
    struct astnode *ast;
    struct listnode *tokens;
    char *content;
    list_init(&tokens);

    /*
     * parse empty function
     */
    content = "char function()"
              "{"
              "}";
    do_tokenizing(content, strlen(content), &tokens);

    init_parsetable();

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);

    /*
     * parse function with variable declarations and for loop
     */
    list_init(&tokens);
    content = "char function()"
              "{"
              "    int identifier;"
              "    long identifier;"
              "    for (;;)"
              "    {"
              "    }"
              "}";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);

    /*
     * parse function for loop with parameters
     */
    list_init(&tokens);
    content = "char function(int i)"
              "{"
              "    for (i=1;i<5;i++)"
              "    {"
              "    }"
              "}";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);

    /*
     * parse function while loop
     */
    list_init(&tokens);
    content = "char function(char a, char b)"
              "{"
              "    while (a == b)"
              "    {"
              "    }"
              "}";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);
}
END_TEST

START_TEST(test_parser_can_parse_struct)
{
    struct astnode *ast;
    struct listnode *tokens;
    char *content;
    list_init(&tokens);

    /*
     * parse empty struct
     */
    content = "struct identifier;";
    do_tokenizing(content, strlen(content), &tokens);

    init_parsetable();

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);

    /*
     * parse simple struct
     */
    list_init(&tokens);
    content = "struct identifier"
              "{"
              "    int identifier;"
              "    char identifier;"
              "};";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);
}
END_TEST

START_TEST(test_parser_can_parse_arithmatic_statements)
{
    struct astnode *ast;
    struct listnode *tokens;
    char *content;
    list_init(&tokens);

    init_parsetable();

    /*
     * parse expressions
     */
    content = "char function()"
              "{"
              "    int a;"
              "    a = b + c * d;"
              "}";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);
}
END_TEST

START_TEST(test_parser_can_parse_conditional_statements)
{
    struct astnode *ast;
    struct listnode *tokens;
    char *content;
    list_init(&tokens);

    init_parsetable();

    /*
     * parse expressions
     */
    content = "char function()"
              "{"
              "    /* solitary if statement */"
              "    if (a == b)"
              "    {"
              "        a = 1;"
              "    }"
              "    /* nested if statement */"
              "    if (a == b)"
              "    {"
              "        a = 2;"
              "    }"
              "    else if (a != b)"
              "    {"
              "        a = 3;"
              "    }"
              "    else"
              "    {"
              "        a = 4;"
              "    }"
              "    /* switch case statement */"
              "    switch (a)"
              "    {"
              "        case 1:"
              "        case 2:"
              "        {"
              "            break;"
              "        }"
              "    }"
              "}";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);

    /*
     * switch case statements
     */
    list_init(&tokens);
    content = "char function()"
              "{"
              "    /* switch case statement */"
              "    switch (a)"
              "    {"
              "        case 1:"
              "        case 2:"
              "        {"
              "            break;"
              "        }"
              "    }"
              "}";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);
}
END_TEST


START_TEST(test_parser_can_parse_assigment_operations)
{
    struct astnode *ast;
    struct listnode *tokens;
    char *content;
    list_init(&tokens);

    init_parsetable();

    /*
     * assigment operations
     */
    content = "char function()"
              "{"
              "    /* assignment operations */"
              "    a += b;"
              "    a *= b;"
              "    a -= b;"
              "    a /= b;"
              "    a %= b;"
              "}";
    do_tokenizing(content, strlen(content), &tokens);

    ast = parse(tokens);
    ck_assert_int_eq(AST_TRANSLATION_UNIT, ast->type);
}
END_TEST

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
    char *content = ";=+*&'\"/%<>^|?:.";
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
    ck_assert_int_eq(TOK_DOT, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
}
END_TEST

START_TEST(test_scanner_can_parse_combination_tokens)
{
    char *content = "+++=---=->>><<<=>===!=&&||...";
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
    ck_assert_int_eq(TOK_ELLIPSIS, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
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
    char *content = "int char goto  continue break  return if else switch case default enum struct union const volatile void short long float double signed unsigned";
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
    ck_assert_int_eq(TOK_ENUM, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_STRUCT, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_UNION, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_CONST, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_VOLATILE, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_VOID, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_SHORT, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_LONG, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_FLOAT, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_DOUBLE, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_SIGNED, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_UNSIGNED, ((struct token *)tokens->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->data)->type);
}
END_TEST


int
main(void)
{
    Suite *suite = suite_create("scc");
    TCase *testcase = tcase_create("scc");
    SRunner *runner = srunner_create(suite);

    suite_add_tcase(suite, testcase);
    tcase_add_test(testcase, test_head_terminal_values_on_constant);
    tcase_add_test(testcase, test_head_terminal_values_on_primary_expression);
    tcase_add_test(testcase, test_head_terminal_values_on_postfix_expression);
    tcase_add_test(testcase, test_head_terminal_values_on_unary_expression);
    tcase_add_test(testcase, test_head_terminal_values_on_cast_expression);
    tcase_add_test(testcase, test_head_terminal_values_on_multiplicative_expression);
    tcase_add_test(testcase, test_head_terminal_values_on_additive_expression);
    tcase_add_test(testcase, test_head_terminal_values_on_shift_expression);
    tcase_add_test(testcase, test_head_terminal_values_on_relational_expression);
    tcase_add_test(testcase, test_head_terminal_values_on_specifier_qualifier_list);
    tcase_add_test(testcase, test_generate_items_on_constant);
    tcase_add_test(testcase, test_generate_items_on_primary_expression);
    tcase_add_test(testcase, test_generate_items_on_postfix_expression);
    tcase_add_test(testcase, test_generate_items_on_unary_expression);
    tcase_add_test(testcase, test_generate_transitions_increments_cursor_position);
    tcase_add_test(testcase, test_parser_state_contains_item);
    tcase_add_test(testcase, test_parser_generate_states);
    tcase_add_test(testcase, test_parser_can_parse_simple_declaration);
    tcase_add_test(testcase, test_parser_can_parse_multiple_simple_declarations);
    tcase_add_test(testcase, test_parser_can_parse_function);
    tcase_add_test(testcase, test_parser_can_parse_struct);
    tcase_add_test(testcase, test_parser_can_parse_arithmatic_statements);
    tcase_add_test(testcase, test_parser_can_parse_conditional_statements);
    tcase_add_test(testcase, test_parser_can_parse_assigment_operations);
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

    srunner_run_all(runner, CK_ENV);
    return 0;
}

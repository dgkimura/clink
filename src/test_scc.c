#include <check.h>

#include "list.h"
#include "scanner.h"

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

    ck_assert_int_eq(TOK_STRING, ((struct token *)tokens->data)->type);
    ck_assert_str_eq("abcd", ((struct token *)tokens->data)->value);
}
END_TEST

START_TEST(test_scanner_can_parse_string_token_with_integers)
{
    char *content = "abc123";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_STRING, ((struct token *)tokens->data)->type);
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
    char *content = ";=+*'\"";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_SEMICOLON, ((struct token *)tokens->data)->type);
    ck_assert_int_eq(TOK_EQUAL, ((struct token *)tokens->next->data)->type);
    ck_assert_int_eq(TOK_PLUS, ((struct token *)tokens->next->next->data)->type);
    ck_assert_int_eq(TOK_ASTERICKS, ((struct token *)tokens->next->next->next->data)->type);
    ck_assert_int_eq(TOK_SINGLEQUOTE, ((struct token *)tokens->next->next->next->next->data)->type);
    ck_assert_int_eq(TOK_DOUBLEQUOTE, ((struct token *)tokens->next->next->next->next->next->data)->type);
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

    ck_assert_int_eq(TOK_STRING, ((struct token *)tokens->data)->type);
    ck_assert_str_eq("abc", ((struct token *)tokens->data)->value);
    ck_assert_str_eq("ghi", ((struct token *)tokens->next->data)->value);
}
END_TEST

START_TEST(test_scanner_can_parse_reserved_words)
{
    char *content = "int char";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_INT, ((struct token *)tokens->data)->type);
    ck_assert_int_eq(TOK_CHAR, ((struct token *)tokens->next->data)->type);
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
    tcase_add_test(testcase, test_scanner_ignores_comment_contents);
    tcase_add_test(testcase, test_scanner_ignores_comment_contents_around_strings);
    tcase_add_test(testcase, test_scanner_can_parse_reserved_words);

    srunner_run_all(runner, CK_ENV);
    return 0;
}

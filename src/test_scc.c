#include <check.h>

#include "list.h"
#include "scanner.h"

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

START_TEST(test_scanner_can_parse_lparen)
{
    char *content = "(";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_LPAREN, ((struct token *)tokens->data)->type);
}
END_TEST

START_TEST(test_scanner_can_parse_rparen)
{
    char *content = ")";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_RPAREN, ((struct token *)tokens->data)->type);
}
END_TEST

START_TEST(test_scanner_can_parse_two_parens)
{
    char *content = "{}";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_LPAREN, ((struct token *)tokens->next->data)->type);
    ck_assert_int_eq(TOK_RPAREN, ((struct token *)tokens->data)->type);
}
END_TEST

int
main(void)
{
    Suite *suite = suite_create("scc");
    TCase *testcase = tcase_create("scc");
    SRunner *runner = srunner_create(suite);

    suite_add_tcase(suite, testcase);
    tcase_add_test(testcase, test_scanner_can_parse_integer_token);
    tcase_add_test(testcase, test_scanner_can_parse_string_token);
    tcase_add_test(testcase, test_scanner_can_parse_string_token_with_integers);
    tcase_add_test(testcase, test_scanner_can_parse_lparen);
    tcase_add_test(testcase, test_scanner_can_parse_rparen);
    tcase_add_test(testcase, test_scanner_can_parse_two_parens);

    srunner_run_all(runner, CK_ENV);
    return 0;
}

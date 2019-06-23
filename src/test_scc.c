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
}
END_TEST

START_TEST(test_scanner_can_parse_string_token)
{
    char *content = "abcd";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_STRING, ((struct token *)tokens->data)->type);
}
END_TEST

START_TEST(test_scanner_can_parse_string_token_with_integers)
{
    char *content = "abc123";
    struct listnode *tokens;
    list_init(&tokens);

    do_tokenizing(content, strlen(content), &tokens);

    ck_assert_int_eq(TOK_STRING, ((struct token *)tokens->data)->type);
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

    srunner_run_all(runner, CK_ENV);
    return 0;
}

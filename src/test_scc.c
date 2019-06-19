#include <check.h>

#include "list.h"
#include "scanner.h"

START_TEST(test_scanner_can_parse_token)
{
    char *content = "abcd";
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
    tcase_add_test(testcase, test_scanner_can_parse_token);

    srunner_run_all(runner, CK_ENV);
    return 0;
}

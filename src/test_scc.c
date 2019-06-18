#include <check.h>

START_TEST(x)
{
    ck_assert_int_eq(1, 1);
}
END_TEST

int
main(void)
{
    Suite *suite = suite_create("scc");
    TCase *testcase = tcase_create("scc");
    SRunner *runner = srunner_create(suite);

    suite_add_tcase(suite, testcase);
    tcase_add_test(testcase, x);

    srunner_run_all(runner, CK_ENV);
    return 0;
}

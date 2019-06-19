#include "scanner.h"

int
main(int argc, char *agv[])
{
    int total_tokens;
    struct listnode *tokens;

    do_preprocessing("test.c", "_test.c");
    do_tokenizing("_test.c", 0, &tokens);

    return 0;
}

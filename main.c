#include "scanner.h"

int
main(int argc, char *agv[])
{
    int total_tokens;
    struct token *tokens;

    do_preprocessing("test.c", "_test.c");
    tokens = do_tokenizing("_test.c", &total_tokens);

    return 0;
}

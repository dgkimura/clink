#include "scanner.h"
#include "parser.h"

int
main(int argc, char *agv[])
{
    int total_tokens;
    struct listnode *tokens;

    preprocess("test.c", "_test.c");
    scan("_test.c", 0, &tokens);
    parse(tokens);

    return 0;
}

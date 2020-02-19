#include "scanner.h"
#include "parser.h"
#include "generator.h"

int
main(int argc, char *agv[])
{
    int total_tokens;
    struct listnode *tokens;
    struct astnode *ast;

    preprocess("test.c", "_test.c");
    scan("_test.c", 0, &tokens);
    ast = parse(tokens);
    generate(ast, "test.s");

    return 0;
}

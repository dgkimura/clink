#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "parser.h"
#include "generator.h"

static char *
read_file(const char *filename, long *filelength)
{
    FILE *f;
    long length;
    char *buffer;

    f = fopen(filename, "r");
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = malloc(length);
    if (buffer)
    {
        fread(buffer, 1, length, f);
    }
    fclose(f);
    *filelength = length;
    return buffer;
}

int
main(int argc, char *argv[])
{
    int total_tokens;
    struct listnode *tokens;
    struct astnode *ast;

    char *buffer;
    long filelength;

    if (argc < 2)
    {
        printf("Not enough args. Must provide a file to compile.");
        return 1;
    }

    buffer = read_file(argv[1], &filelength);
    //preprocess("test.c", "_test.c");
    scan(buffer, filelength, &tokens);
    ast = parse(tokens);
    generate(ast, "test.s");

    return 0;
}

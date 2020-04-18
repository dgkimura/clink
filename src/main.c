#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *
assembly_filename(char *filename)
{
    filename[strlen(filename) - 1] = 's';
    return filename;
}

int
main(int argc, char *argv[])
{
    int total_tokens;
    struct listnode *tokens = NULL;
    struct astnode *ast;

    char filename[25];
    char *buffer;
    long filelength;

    if (argc < 2)
    {
        printf("Not enough args. Must provide a file to compile.");
        return 1;
    }
    strncpy(filename, argv[1], sizeof(filename));

    buffer = read_file(filename, &filelength);
    //preprocess("test.c", "_test.c");
    scan(buffer, filelength, &tokens);
    ast = parse(tokens);

    generate(ast, assembly_filename(filename));

    return 0;
}

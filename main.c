#include "scanner.h"

int
main(int argc, char *agv[])
{
    int total_tokens;
    scan_file("test.c", &total_tokens);

    return 0;
}

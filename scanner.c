#include <stdio.h>

#include "list.h"

struct transform
{
    char *input;
    char *output;
};

void
do_preprocessing(char *infile, char *outfile)
{
    char *in_content;
    char *out_content;
    int in_index;
    int out_index;
    struct listnode *transforms;

    list_init(&transforms);

    /*
     * Find macros and includes inside infile in order to construct outfile
     * replacing  macros and includes
     */
    for (;;)
    {
    }
}

struct token *
do_tokenizing(char *file, int *total_tokens)
{
    return NULL;
}

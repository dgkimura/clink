#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "scanner.h"

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

void
do_tokenizing(char *content, size_t content_len, struct listnode **tokens)
{
    struct token *tok;
    size_t i, tok_start, tok_end;

    for (i=0; i<content_len; i++)
    {
        if (isalpha(content[i]))
        {
            tok_start = i;

            /* consume characters */
            while (isalpha(content[i]) || isdigit(content[i]))
            {
                i += 1;
            }
            tok_end = i;

            tok = (struct token *)malloc(sizeof(struct token));

            tok->type = TOK_STRING;
            strncpy(tok->value, content + tok_start, tok_end - tok_start);

            list_prepend(tokens, tok);
        }
        else if (isdigit(content[i]))
        {
            tok_start = i;

            /* consume digits */
            while (isdigit(content[i]))
            {
                i += 1;
            }
            tok_end = i;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_INTEGER;
            strncpy(tok->value, content + tok_start, tok_end - tok_start);

            list_prepend(tokens, tok);
        }
    }
}

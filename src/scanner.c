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
    size_t i, tok_start, tok_end, tok_size;

    for (i=0; i<content_len;)
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
            tok_size = tok_end - tok_start + 1;

            tok = (struct token *)malloc(sizeof(struct token));

            tok->type = TOK_STRING;
            tok->value = (char *)malloc(sizeof(char) * tok_size);
            strncpy(tok->value, content + tok_start, tok_size);
            tok->value[tok_size] = '\0';

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
            tok_size = tok_end - tok_start + 1;

            tok = (struct token *)malloc(sizeof(struct token));

            tok->type = TOK_INTEGER;
            tok->value = (char *)malloc(sizeof(char) * tok_size);
            strncpy(tok->value, content + tok_start, tok_size);
            tok->value[tok_size] = '\0';

            list_prepend(tokens, tok);
        }
        else if (content[i] == '(')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_LPAREN;

            list_prepend(tokens, tok);
        }
        else if (content[i] == ')')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_RPAREN;

            list_prepend(tokens, tok);
        }
        else if (content[i] == '[')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_LBRACKET;

            list_prepend(tokens, tok);
        }
        else if (content[i] == ']')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_RBRACKET;

            list_prepend(tokens, tok);
        }
        else if (content[i] == '{')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_LBRACE;

            list_prepend(tokens, tok);
        }
        else if (content[i] == '}')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_RBRACE;

            list_prepend(tokens, tok);
        }
        else if (content[i] == ';')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_SEMICOLON;

            list_prepend(tokens, tok);
        }
    }
}

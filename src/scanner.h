#include <ctype.h>

#include "list.h"

enum token_t
{
    TOK_INTEGER,
    TOK_STRING,
    TOK_LPAREN,
    TOK_RPAREN
};

struct token
{
    enum token_t type;
    char *value;
};

void do_preprocessing(char *infile, char *outfile);
void do_tokenizing(char *content, size_t content_len, struct listnode **tokens);

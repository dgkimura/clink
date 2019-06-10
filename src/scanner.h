#include "list.h"

enum token_t
{
    TOK_INTEGER,
    TOK_STRING
};

#define MAX_TOKEN_SIZE 256

struct token
{
    enum token_t type;
    char value[MAX_TOKEN_SIZE];
};

void do_preprocessing(char *infile, char *outfile);
void do_tokenizing(char *file, struct listnode **tokens);

#include <ctype.h>

#include "list.h"

enum token_t
{
    TOK_INVALID = -1,
    TOK_INTEGER,
    TOK_STRING,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_SEMICOLON,
    TOK_INT,
    TOK_CHAR
};

struct token
{
    enum token_t type;
    char *value;
};

void do_preprocessing(char *infile, char *outfile);
void do_tokenizing(char *content, size_t content_len, struct listnode **tokens);

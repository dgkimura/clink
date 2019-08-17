#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <ctype.h>

#include "list.h"

enum token_t
{
    TOK_INVALID = -1,
    TOK_INTEGER,
    TOK_IDENTIFIER,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_SEMICOLON,
    TOK_EQUAL,
    TOK_BACKSLASH,
    TOK_MOD,
    TOK_BANG,
    TOK_PLUS,
    TOK_PLUS_PLUS,
    TOK_PLUS_EQUAL,
    TOK_MINUS,
    TOK_MINUS_MINUS,
    TOK_MINUS_EQUAL,
    TOK_ARROW,
    TOK_ASTERISK,
    TOK_AMPERSAND,
    TOK_AMPERSAND_AMPERSAND,
    TOK_CARET,
    TOK_VERTICALBAR,
    TOK_SINGLEQUOTE,
    TOK_DOUBLEQUOTE,
    TOK_INT,
    TOK_CHAR,
    TOK_SHIFTLEFT,
    TOK_SHIFTRIGHT,
    TOK_LESSTHAN,
    TOK_GREATERTHAN,
    TOK_LESSTHANEQUAL,
    TOK_GREATERTHANEQUAL,
    TOK_EQ,
    TOK_NEQ
};

struct token
{
    enum token_t type;
    char *value;
};

void do_preprocessing(char *infile, char *outfile);

/*
 * Given a string of code, constructs a list of tokens
 */
void do_tokenizing(char *content, size_t content_len, struct listnode **tokens);

#endif

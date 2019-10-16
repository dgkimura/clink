#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stddef.h>
#include <ctype.h>

#include "list.h"

enum token_t
{
    TOK_EOF = -1,
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
    TOK_BACKSLASH_EQUAL,
    TOK_MOD,
    TOK_MOD_EQUAL,
    TOK_BANG,
    TOK_PLUS,
    TOK_PLUS_PLUS,
    TOK_PLUS_EQUAL,
    TOK_MINUS,
    TOK_MINUS_MINUS,
    TOK_MINUS_EQUAL,
    TOK_ARROW,
    TOK_ASTERISK,
    TOK_ASTERISK_EQUAL,
    TOK_AMPERSAND,
    TOK_AMPERSAND_AMPERSAND,
    TOK_CARET,
    TOK_COMMA,
    TOK_DOT,
    TOK_ELLIPSIS,
    TOK_QUESTIONMARK,
    TOK_COLON,
    TOK_VERTICALBAR,
    TOK_VERTICALBAR_VERTICALBAR,
    TOK_SINGLEQUOTE,
    TOK_DOUBLEQUOTE,
    TOK_SHIFTLEFT,
    TOK_SHIFTRIGHT,
    TOK_LESSTHAN,
    TOK_GREATERTHAN,
    TOK_LESSTHANEQUAL,
    TOK_GREATERTHANEQUAL,
    TOK_EQ,
    TOK_NEQ,

    /* reserved words */
    TOK_VOID,
    TOK_CHAR,
    TOK_SHORT,
    TOK_INT,
    TOK_LONG,
    TOK_FLOAT,
    TOK_DOUBLE,
    TOK_SIGNED,
    TOK_UNSIGNED,
    TOK_GOTO,
    TOK_CONTINUE,
    TOK_BREAK,
    TOK_RETURN,
    TOK_FOR,
    TOK_DO,
    TOK_WHILE,
    TOK_IF,
    TOK_ELSE,
    TOK_SWITCH,
    TOK_CASE,
    TOK_DEFAULT,
    TOK_ENUM,
    TOK_STRUCT,
    TOK_UNION,
    TOK_CONST,
    TOK_VOLATILE,
    TOK_AUTO,
    TOK_REGISTER,
    TOK_STATIC,
    TOK_EXTERN,
    TOK_TYPEDEF,
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

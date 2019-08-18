#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "scanner.h"

static enum token_t reserved_word_token(char *str, size_t len);

struct token reserved_map[] =
{
    { TOK_INT, "int" },
    { TOK_CHAR, "char" },
    { TOK_GOTO, "goto" },
    { TOK_CONTINUE, "continue" },
    { TOK_BREAK, "break" },
    { TOK_RETURN, "return" },
    { TOK_INVALID, "" } /* TOK_INVALID must be last entry */
};

static enum token_t
reserved_word_token(char *str, size_t len)
{
    enum token_t t = TOK_INVALID;
    int i;

    for (i = 0; reserved_map[i].type != TOK_INVALID; i++)
    {
        if (len != strlen(reserved_map[i].value))
        {
            continue;
        }
        if (strncmp(str, reserved_map[i].value, len) == 0)
        {
            t = reserved_map[i].type;
            break;
        }
    }

    return t;
}

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
            while (i < content_len && (isalpha(content[i]) || isdigit(content[i])))
            {
                i += 1;
            }
            tok_end = i;
            tok_size = tok_end - tok_start;

            tok = (struct token *)malloc(sizeof(struct token));

            /*
             * Check if this token is a reserved word. If not then consider it
             * a label.
             */
            tok->type = reserved_word_token(&content[tok_start], tok_size);
            if (tok->type == TOK_INVALID)
            {
                tok->type = TOK_IDENTIFIER;
                tok->value = (char *)malloc(sizeof(char) * tok_size);
                strncpy(tok->value, content + tok_start, tok_size);
                tok->value[tok_size] = '\0';
            }

            list_append(tokens, tok);
        }
        else if (isdigit(content[i]))
        {
            tok_start = i;

            /* consume digits */
            while (i < content_len && isdigit(content[i]))
            {
                i += 1;
            }
            tok_end = i;
            tok_size = tok_end - tok_start;

            tok = (struct token *)malloc(sizeof(struct token));

            tok->type = TOK_INTEGER;
            tok->value = (char *)malloc(sizeof(char) * tok_size);
            strncpy(tok->value, content + tok_start, tok_size);
            tok->value[tok_size] = '\0';

            list_append(tokens, tok);
        }
        else if (content[i] == '(')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_LPAREN;

            list_append(tokens, tok);
        }
        else if (content[i] == ')')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_RPAREN;

            list_append(tokens, tok);
        }
        else if (content[i] == '[')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_LBRACKET;

            list_append(tokens, tok);
        }
        else if (content[i] == ']')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_RBRACKET;

            list_append(tokens, tok);
        }
        else if (content[i] == '{')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_LBRACE;

            list_append(tokens, tok);
        }
        else if (content[i] == '}')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_RBRACE;

            list_append(tokens, tok);
        }
        else if (content[i] == ';')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_SEMICOLON;

            list_append(tokens, tok);
        }
        else if (content[i] == '=')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_EQUAL;

            if (i < content_len && content[i] == '=')
            {
                i += 1;
                tok->type = TOK_EQ;
            }

            list_append(tokens, tok);
        }
        else if (content[i] == '!')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_BANG;

            if (i < content_len && content[i] == '=')
            {
                i += 1;
                tok->type = TOK_NEQ;
            }

            list_append(tokens, tok);
        }
        else if (content[i] == '+')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_PLUS;

            if (i < content_len && content[i] == '+')
            {
                i += 1;
                tok->type = TOK_PLUS_PLUS;
            }
            else if (i < content_len && content[i] == '=')
            {
                i += 1;
                tok->type = TOK_PLUS_EQUAL;
            }

            list_append(tokens, tok);
        }
        else if (content[i] == '-')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_MINUS;

            if (i < content_len && content[i] == '-')
            {
                i += 1;
                tok->type = TOK_MINUS_MINUS;
            }
            else if (i < content_len && content[i] == '=')
            {
                i += 1;
                tok->type = TOK_MINUS_EQUAL;
            }
            else if (i < content_len && content[i] == '>')
            {
                i += 1;
                tok->type = TOK_ARROW;
            }

            list_append(tokens, tok);
        }
        else if (content[i] == '*')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_ASTERISK;

            if (i < content_len && content[i] == '=')
            {
                i += 1;
                tok->type = TOK_ASTERISK_EQUAL;
            }

            list_append(tokens, tok);
        }
        else if (content[i] == '&')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_AMPERSAND;

            if (content[i] == '&')
            {
                i += 1;
                tok->type = TOK_AMPERSAND_AMPERSAND;
            }

            list_append(tokens, tok);
        }
        else if (content[i] == '\'')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_SINGLEQUOTE;

            list_append(tokens, tok);
        }
        else if (content[i] == '"')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_DOUBLEQUOTE;

            list_append(tokens, tok);
        }
        else if (content[i] == '/')
        {
            i += 1;

            if (i < content_len && content[i] == '=')
            {
                i += 1;

                tok = (struct token *)malloc(sizeof(struct token));
                tok->type = TOK_BACKSLASH_EQUAL;

                list_append(tokens, tok);
            }
            else if (i < content_len && content[i] == '*')
            {
                i += 1;

                /* skip over comment contents */
                while (i + 1 < content_len &&
                       content[i] != '*' && content[i+1] != '/')
                {
                    i += 1;
                }
                /* consume the ending '*' and '/' characters */
                if (i < content_len)
                {
                    i += 2;
                }
            }
            else
            {
                tok = (struct token *)malloc(sizeof(struct token));
                tok->type = TOK_BACKSLASH;

                list_append(tokens, tok);
            }
        }
        else if (content[i] == '%')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_MOD;

            if (i < content_len && content[i] == '=')
            {
                i += 1;
                tok->type = TOK_MOD_EQUAL;
            }

            list_append(tokens, tok);
        }
        else if (content[i] == '>')
        {
            i += 1;

            if (i < content_len && content[i] == '>')
            {
                i += 1;

                tok = (struct token *)malloc(sizeof(struct token));
                tok->type = TOK_SHIFTRIGHT;

                list_append(tokens, tok);
            }
            else if (i < content_len && content[i] == '=')
            {
                i += 1;

                tok = (struct token *)malloc(sizeof(struct token));
                tok->type = TOK_GREATERTHANEQUAL;

                list_append(tokens, tok);
            }
            else
            {
                tok = (struct token *)malloc(sizeof(struct token));
                tok->type = TOK_GREATERTHAN;

                list_append(tokens, tok);
            }
        }
        else if (content[i] == '<')
        {
            i += 1;

            if (i < content_len && content[i] == '<')
            {
                i += 1;

                tok = (struct token *)malloc(sizeof(struct token));
                tok->type = TOK_SHIFTLEFT;

                list_append(tokens, tok);
            }
            else if (i < content_len && content[i] == '=')
            {
                i += 1;

                tok = (struct token *)malloc(sizeof(struct token));
                tok->type = TOK_LESSTHANEQUAL;

                list_append(tokens, tok);
            }
            else
            {
                tok = (struct token *)malloc(sizeof(struct token));
                tok->type = TOK_LESSTHAN;

                list_append(tokens, tok);
            }
        }
        else if (content[i] == '^')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_CARET;

            list_append(tokens, tok);
        }
        else if (content[i] == ',')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_COMMA;

            list_append(tokens, tok);
        }
        else if (content[i] == '?')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_QUESTIONMARK;

            list_append(tokens, tok);
        }
        else if (content[i] == ':')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_COLON;

            list_append(tokens, tok);
        }
        else if (content[i] == '|')
        {
            i += 1;

            tok = (struct token *)malloc(sizeof(struct token));
            tok->type = TOK_VERTICALBAR;

            if (i < content_len && content[i] == '|')
            {
                i += 1;
                tok->type = TOK_VERTICALBAR_VERTICALBAR;
            }

            list_append(tokens, tok);
        }
        else if (isspace(content[i]))
        {
            i += 1;
        }
    }
}

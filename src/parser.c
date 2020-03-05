/*
 * Provides a bottom-up CLR(1) parser. This involves 3 main steps:
 *
 * 1) Given a grammar, construct a state machine. This performed in
 *    `generate_states()`.
 * 2) Given a state machine, construct a parse table. This is performed in
 *    `init_parsetable()`.
 * 3) Given a parse table, construct the AST (abstract syntax tree). This is
 *    performed in `parse()`.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammar.h"
#include "parser.h"
#include "utilities.h"

#ifdef GENPT
static struct parsetable_item *parsetable = NULL;
#else
#include "parsetable.h"
#endif

/*
 * state_identifier indicates the total number of states in the grammar.
 */
static int state_identifier = 0;
static int tmp_state_identifier = 0;

/*
 * states is the list of states in the grammar.
 */
static struct state states[MAX_STATES];

static struct state temp_states[MAX_STATES];

/*
 * Print a state in the parser state machine.
 */
void
print_state(struct state *s)
{
    struct listnode *items, *lookaheads;
    struct item *item;
    int i;

    printf("state %d\n", s->identifier);
    for (items=s->items; items!=NULL; items=items->next)
    {
        item = (struct item *)items->data;
        printf("  (%d,%d)", (int)(item->rewrite_rule-grammar), item->cursor_position);

        for (lookaheads=item->lookahead; lookaheads!=NULL; lookaheads=lookaheads->next)
        {
            printf(" %d", (int)(lookaheads->data));
        }
        printf("\n");
    }
}

static int
checked_nodes_contains(struct listnode **items, enum astnode_t node)
{
    int contains = 0;
    struct listnode *c;

    c = *items;
    while (c != NULL)
    {
        if ((enum astnode_t)c->data == node)
        {
            contains = 1;
            break;
        }

        c = c->next;
    }
    return contains;
}

void
head_terminal_values(enum astnode_t node, struct listnode **checked_nodes,
                     struct listnode **terminals)
{
    int i;

    if (checked_nodes_contains(checked_nodes, node))
    {
        /*
         * If another iteration is already checking node, then there is nothing
         * more to do here.
         */
        return;
    }

    if (node < AST_INVALID)
    {
        /*
         * If node is a terminal symbol then add it and return.
         */
        list_append(terminals, (void *)node);
        return;
    }

    /*
     * Update checked_nodes to avoid trying to repeated work and infinite
     * recursion.
     */
    list_append(checked_nodes, (void *)node);

    for (i=0; i<NUM_RULES; i++)
    {
        if (grammar[i].type == node)
        {
            if (grammar[i].nodes[0] < AST_INVALID &&
                !checked_nodes_contains(terminals, grammar[i].nodes[0]))
            {
                /*
                 * If the symbol is a terminal value and we have not already
                 * added it to terminals then add it
                 */
                list_append(terminals, (void *)grammar[i].nodes[0]);
            }
            else if (grammar[i].nodes[0] != node)
            {
                /*
                 * If the symbol is a non-terminal value then recurse and find
                 * the head terminal
                 */
                head_terminal_values(grammar[i].nodes[0], checked_nodes,
                                     terminals);
            }
        }
    }
}

static int
items_contains(struct listnode **items, struct rule *r, int position, struct listnode *lookahead)
{
    int contains = 0;
    struct listnode *c;
    struct item *i;

    c = *items;
    while (c != NULL)
    {
        i = (struct item *)c->data;
        if (i->rewrite_rule == r && i->cursor_position == position &&
            list_equal(i->lookahead, lookahead))
        {
            contains = 1;
            break;
        }

        c = c->next;
    }
    return contains;
}

/*
 * Generate the items for a given production node.
 */
void
generate_items(enum astnode_t node, struct listnode *lookahead, struct listnode **items)
{
    int i;
    struct item *item;
    struct listnode *checked_nodes, *next_lookahead;

    for (i=0; i<NUM_RULES; i++)
    {
        if (grammar[i].type == node && !items_contains(items, &grammar[i], 0, lookahead))
        {
            item = malloc(sizeof(struct item));
            item->rewrite_rule = &grammar[i];
            item->cursor_position = 0;
            item->lookahead = lookahead;

            list_append(items, item);

            /*
             * Recurse if the derivation begins with variable.
             */
            if (grammar[i].nodes[0] > AST_INVALID)
            {
                if (grammar[i].length_of_nodes > 1)
                {
                    list_init(&checked_nodes);
                    list_init(&next_lookahead);

                    head_terminal_values(
                        grammar[i].nodes[1],
                        &checked_nodes,
                        &next_lookahead);
                    generate_items(grammar[i].nodes[0], next_lookahead, items);
                }
                else
                {
                    generate_items(grammar[i].nodes[0], lookahead, items);
                }
            }
        }
    }
}

/*
 * Generate a state with items, recursively construct connecting states and
 * transitions.
 */
void
generate_transitions(struct state *s)
{
    struct listnode *items;
    struct item *i, *j;
    struct rule *r;
    int index, new_index;


    for (items=s->items; items!=NULL; items=items->next)
    {
        i = (struct item *)items->data;
        r = i->rewrite_rule;

        /*
         * Check if the item contains another consumable value.
         */
        if (i->cursor_position < r->length_of_nodes)
        {
            j = malloc(sizeof(struct item));
            j->rewrite_rule = i->rewrite_rule;

            j->lookahead = i->lookahead;
            j->cursor_position = i->cursor_position + 1;

            index = INDEX(i->rewrite_rule->nodes[i->cursor_position]);
            if (s->links[index] == NULL)
            {
                assert(tmp_state_identifier < MAX_STATES);

                s->links[index] = &temp_states[tmp_state_identifier++];
                memset(s->links[index], 0, sizeof(struct state));
            }

            if (items_contains(&s->links[index]->items, j->rewrite_rule,
                                j->cursor_position, j->lookahead))
            {
                /*
                 * If state already contains item then continue.
                 */
                free(j);
                continue;
            }

            list_append(&s->links[index]->items, j);

            if (j->cursor_position < j->rewrite_rule->length_of_nodes &&
                j->rewrite_rule->nodes[j->cursor_position] > AST_INVALID)
            {
                /*
                 * Append new states due to subsequent non-terminal.
                 */
                struct listnode *lookahead;
                list_init(&lookahead);

                if (j->cursor_position + 1 < j->rewrite_rule->length_of_nodes)
                {
                    /*
                     * If a follow symbol exists then find terminal values of
                     * the follow symbol. Then generate items derived from the
                     * current symbol and add them to state with the new
                     * lookahead.
                     */
                    struct listnode *checked_nodes;
                    list_init(&checked_nodes);

                    head_terminal_values(
                        j->rewrite_rule->nodes[j->cursor_position + 1],
                        &checked_nodes,
                        &lookahead);

                    generate_items(
                        j->rewrite_rule->nodes[j->cursor_position],
                        lookahead, &s->links[index]->items);
                }
                else
                {
                    /*
                     * If there is no follow symbol then simply generate the
                     * items derived fromthe current symbol and pass along the
                     * current lookahead from the previous state.
                     */
                    generate_items(
                        j->rewrite_rule->nodes[j->cursor_position],
                        j->lookahead, &s->links[index]->items);
                }

            }
        }
    }

    for (index=0; index<NUM_SYMBOLS; index++)
    {
        new_index = index_of_state(s->links[index]);

        if (s->links[index] != NULL && new_index == -1)
        {
            /*
             * If the state does not exist in global states then update the
             * links and recursively generate next transitions. We waited until
             * now to set the link because we finally know all items have been
             * added and can avoid duplicate states.
             */
            new_index = state_identifier++;
            assert(state_identifier < MAX_STATES);

            s->links[index]->identifier = new_index;
            states[new_index] = *s->links[index];

            generate_transitions(&states[new_index]);
        }
        else if (s->links[index] != NULL)
        {
            /*
             * It may be the case that state was recursively generated. In
             * which case we do not want to recurse again, but we still need to
             * set the identifier.
             */
            s->links[index]->identifier = new_index;
        }
    }
}

/*
 * Generates all state for grammar and returns the root state.
 */
struct state *
generate_states(void)
{
    struct state *s;

    memset(states, 0, sizeof(struct state) * MAX_STATES);

    s = &states[0];
    s->identifier = state_identifier++;

    generate_items(AST_TRANSLATION_UNIT, NULL, &s->items);
    generate_transitions(s);

    return s;
}

/*
 * Returns whether the given state contains the given item.
 */
int
state_contains_item(struct state *state, struct item *item)
{
    int contains = 0;
    struct listnode *l;
    struct item *i;

    for (l=state->items; l!=NULL; l=l->next)
    {
        i = (struct item *)l->data;

        if (item->rewrite_rule == i->rewrite_rule &&
            item->cursor_position == i->cursor_position &&
            list_equal(item->lookahead, i->lookahead))
        {
            contains = 1;
            break;
        }
    }
    return contains;
}

int
compare_states(struct state *a, struct state *b)
{
    int index, i, compare;
    struct listnode *l;

    compare = 0;

    if ((a == NULL && b != NULL) || (a != NULL && b == NULL))
    {
        return -1;
    }

    /*
     * Check that indexed state contains everything in state.
     */
    for (l=a->items; l!=NULL; l=l->next)
    {
        if (!state_contains_item(b, (struct item *)l->data))
        {
            compare = -1;
            break;
        }
    }

    /*
     * Inverse check that state contains everything in indexed state.
     */
    for (l=b->items; l!=NULL; l=l->next)
    {
        if (!state_contains_item(a, (struct item *)l->data))
        {
            compare = 1;
            break;
        }
    }

    return compare;
}

/*
 * Returns the index of a state in global states that has identical items or -1
 * if does not exist.
 */
int
index_of_state(struct state *state)
{
    int index, i, match;
    struct listnode *l;

    index = -1;
    for (i=0; i<state_identifier; i++)
    {
        if (compare_states(state, &states[i]) == 0)
        {
            index = i;
            break;
        }
    }

    return index;
}

#ifdef GENPT
void
init_parsetable(void)
{
    int i, j;
    struct parsetable_item *row, *cell;
    struct state *state;
    struct listnode *node, *inner_node;
    struct item *item;
    int lookahead;
    FILE *fp;

    if (parsetable != NULL)
    {
        return;
    }

    generate_states();

    parsetable = malloc(sizeof(struct parsetable_item) * (NUM_SYMBOLS) * (state_identifier + 1));
    memset(parsetable, 0, sizeof(struct parsetable_item) * NUM_SYMBOLS * (state_identifier + 1));

    for (i=0; i<state_identifier; i++)
    {
        state = &states[i];
        row = &parsetable[state->identifier * NUM_SYMBOLS];

        for (j=0; j<NUM_SYMBOLS; j++)
        {
            cell = row + j;

            if (state->links[j] != NULL && j > AST_INVALID)
            {
                /*
                 * non-terminal value
                 */
                cell->state = state->links[j]->identifier;
            }
            else if (state->links[j] != NULL && j < AST_INVALID)
            {
                /*
                 * terminal value
                 */
                cell->shift = 1;
                cell->state = state->links[j]->identifier;
            }
        }

        /*
         * Iterate over all items and for each completed rules add to parse
         * table based on lookahead.
         */
        foreach(node, state->items)
        {
            item = ((struct item *)node->data);
            if (item->cursor_position == item->rewrite_rule->length_of_nodes)
            {
                foreach(inner_node, item->lookahead)
                {
                    lookahead = (int)inner_node->data;
                    cell = row + lookahead;

                    cell->reduce = 1;
                    cell->rule = item->rewrite_rule;
                }

                if (item->lookahead == NULL)
                {
                    /*
                     * NULL lookahead means end of input (e.g. $). Since
                     * nothing should match AST_INVALID, let's use that column
                     * for NULL lookahead items.
                     */
                    cell = row + AST_INVALID;

                    cell->reduce = 1;
                    cell->rule = item->rewrite_rule;
                }
            }
        }
    }

    fp = fopen("parsetable.h", "w");
    fprintf(fp, "/*\n");
    fprintf(fp, " * Generated parse table file:\n");
    fprintf(fp, " */\n");
    fprintf(fp, " #include \"grammar.h\"\n");
    fprintf(fp, "struct parsetable_item parsetable[%d] =\n", state_identifier * NUM_SYMBOLS);
    fprintf(fp, "{\n");

    for (i=0; i<state_identifier; i++)
    {
        state = &states[i];
        row = &parsetable[state->identifier * NUM_SYMBOLS];

        for (j=0; j<NUM_SYMBOLS; j++)
        {
            cell = row + j;

            fprintf(fp, "{ &grammar[%ld], %d, %d, %d },",
                    cell->reduce ? cell->rule-grammar : 0,
                    cell->shift, cell->reduce, cell->state);
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "};\n");
    fclose(fp);
}
#endif

struct astnode *
token_to_astnode(struct token *token)
{
    struct astnode *node;
    if (token->type == TOK_INTEGER)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_INTEGER_CONSTANT;
        node->token = token;
    }
    else if (token->type == TOK_IDENTIFIER)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_IDENTIFIER;
        node->token = token;
    }
    else if (token->type == TOK_PLUS)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_PLUS;
        node->token = token;
    }
    else if (token->type == TOK_PLUS_PLUS)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_PLUS_PLUS;
        node->token = token;
    }
    else if (token->type == TOK_PLUS_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_PLUS_EQUAL;
        node->token = token;
    }
    else if (token->type == TOK_MINUS)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MINUS;
        node->token = token;
    }
    else if (token->type == TOK_MINUS_MINUS)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MINUS_MINUS;
        node->token = token;
    }
    else if (token->type == TOK_MINUS_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MINUS_EQUAL;
        node->token = token;
    }
    else if (token->type == TOK_AMPERSAND)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_AMPERSAND;
        node->token = token;
    }
    else if (token->type == TOK_AMPERSAND_AMPERSAND)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_AMPERSAND_AMPERSAND;
        node->token = token;
    }
    else if (token->type == TOK_ASTERISK)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_ASTERISK;
        node->token = token;
    }
    else if (token->type == TOK_ASTERISK_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_ASTERISK_EQUAL;
        node->token = token;
    }
    else if (token->type == TOK_BACKSLASH)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_BACKSLASH;
        node->token = token;
    }
    else if (token->type == TOK_BACKSLASH_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_BACKSLASH_EQUAL;
        node->token = token;
    }
    else if (token->type == TOK_CARET)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_CARET;
        node->token = token;
    }
    else if (token->type == TOK_COMMA)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_COMMA;
        node->token = token;
    }
    else if (token->type == TOK_ELLIPSIS)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_ELLIPSIS;
        node->token = token;
    }
    else if (token->type == TOK_MOD)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MOD;
        node->token = token;
    }
    else if (token->type == TOK_MOD_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_MOD_EQUAL;
        node->token = token;
    }
    else if (token->type == TOK_QUESTIONMARK)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_QUESTIONMARK;
        node->token = token;
    }
    else if (token->type == TOK_COLON)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_COLON;
        node->token = token;
    }
    else if (token->type == TOK_SEMICOLON)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_SEMICOLON;
        node->token = token;
    }
    else if (token->type == TOK_LPAREN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LPAREN;
        node->token = token;
    }
    else if (token->type == TOK_RPAREN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_RPAREN;
        node->token = token;
    }
    else if (token->type == TOK_LBRACKET)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LBRACKET;
        node->token = token;
    }
    else if (token->type == TOK_RBRACKET)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_RBRACKET;
        node->token = token;
    }
    else if (token->type == TOK_LBRACE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LBRACE;
        node->token = token;
    }
    else if (token->type == TOK_RBRACE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_RBRACE;
        node->token = token;
    }
    else if (token->type == TOK_VERTICALBAR)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_VERTICALBAR;
        node->token = token;
    }
    else if (token->type == TOK_VERTICALBAR_VERTICALBAR)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_VERTICALBAR_VERTICALBAR;
        node->token = token;
    }
    else if (token->type == TOK_SHIFTLEFT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_SHIFTLEFT;
        node->token = token;
    }
    else if (token->type == TOK_SHIFTRIGHT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_SHIFTRIGHT;
        node->token = token;
    }
    else if (token->type == TOK_LESSTHAN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LT;
        node->token = token;
    }
    else if (token->type == TOK_GREATERTHAN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_GT;
        node->token = token;
    }
    else if (token->type == TOK_LESSTHANEQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LTEQ;
        node->token = token;
    }
    else if (token->type == TOK_GREATERTHANEQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_GTEQ;
        node->token = token;
    }
    else if (token->type == TOK_EQ)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_EQ;
        node->token = token;
    }
    else if (token->type == TOK_NEQ)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_NEQ;
        node->token = token;
    }
    else if (token->type == TOK_EQUAL)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_EQUAL;
        node->token = token;
    }
    else if (token->type == TOK_VOID)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_VOID;
        node->token = token;
    }
    else if (token->type == TOK_SHORT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_SHORT;
        node->token = token;
    }
    else if (token->type == TOK_INT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_INT;
        node->token = token;
    }
    else if (token->type == TOK_CHAR)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_CHAR;
        node->token = token;
    }
    else if (token->type == TOK_LONG)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_LONG;
        node->token = token;
    }
    else if (token->type == TOK_FLOAT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_FLOAT;
        node->token = token;
    }
    else if (token->type == TOK_DOUBLE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_DOUBLE;
        node->token = token;
    }
    else if (token->type == TOK_SIGNED)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_SIGNED;
        node->token = token;
    }
    else if (token->type == TOK_UNSIGNED)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_UNSIGNED;
        node->token = token;
    }
    else if (token->type == TOK_AUTO)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_AUTO;
        node->token = token;
    }
    else if (token->type == TOK_REGISTER)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_REGISTER;
        node->token = token;
    }
    else if (token->type == TOK_STATIC)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_STATIC;
        node->token = token;
    }
    else if (token->type == TOK_EXTERN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_EXTERN;
        node->token = token;
    }
    else if (token->type == TOK_TYPEDEF)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_TYPEDEF;
        node->token = token;
    }
    else if (token->type == TOK_GOTO)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_GOTO;
        node->token = token;
    }
    else if (token->type == TOK_CONTINUE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_CONTINUE;
        node->token = token;
    }
    else if (token->type == TOK_BREAK)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_BREAK;
        node->token = token;
    }
    else if (token->type == TOK_RETURN)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_RETURN;
        node->token = token;
    }
    else if (token->type == TOK_FOR)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_FOR;
        node->token = token;
    }
    else if (token->type == TOK_DO)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_DO;
        node->token = token;
    }
    else if (token->type == TOK_WHILE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_WHILE;
        node->token = token;
    }
    else if (token->type == TOK_IF)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_IF;
        node->token = token;
    }
    else if (token->type == TOK_ELSE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_ELSE;
        node->token = token;
    }
    else if (token->type == TOK_SWITCH)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_SWITCH;
        node->token = token;
    }
    else if (token->type == TOK_CASE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_CASE;
        node->token = token;
    }
    else if (token->type == TOK_DEFAULT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_DEFAULT;
        node->token = token;
    }
    else if (token->type == TOK_ENUM)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_ENUM;
        node->token = token;
    }
    else if (token->type == TOK_STRUCT)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_STRUCT;
        node->token = token;
    }
    else if (token->type == TOK_UNION)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_UNION;
        node->token = token;
    }
    else if (token->type == TOK_CONST)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_CONST;
        node->token = token;
    }
    else if (token->type == TOK_VOLATILE)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_VOLATILE;
        node->token = token;
    }
    else if (token->type == TOK_EOF)
    {
        node = malloc(sizeof(struct astnode));
        node->type = AST_INVALID;
        node->token = token;
    }

    return node;
}

struct astnode *
parse(struct listnode *tokens)
{
    struct astnode *node, *root;
    struct listnode *stack;
    struct listnode *token;
    struct parsetable_item *row, *cell;
    static int zero = 0;
    int i;

    list_init(&stack);

    /*
     * Stack starts at state 0.
     */
    list_prepend(&stack, &zero);

    for (token=tokens; token!=NULL; )
    {
        row = parsetable + *(int *)stack->data * NUM_SYMBOLS;

        node = token_to_astnode((struct token *)token->data);
        cell = row + INDEX(node->type);
        if (cell->shift)
        {
            /*
             * Shift involves pushing node and state onto stack.
             */
            list_prepend(&stack, node);
            list_prepend(&stack, &cell->state);

            /*
             * Consume a token
             */
            token=token->next;
        }
        else if (cell->reduce)
        {
            root = cell->rule->create(stack, cell->rule);

            /*
             * Reduce involves removing the astnodes that compose the rule from
             * the stack. Then create the reduced astnode and push it onto the
             * stack.
             */
            for (i=0; i<cell->rule->length_of_nodes; i++)
            {
                list_append(&root->children, stack->data);

                /*
                 * Remove astnode and cell state from the stack.
                 */
                stack = stack->next->next;
            }

            /*
             * Push the reduced node and the next state number.
             */
            row = parsetable + *(int *)stack->data * NUM_SYMBOLS;
            cell = row + INDEX(root->type);

            list_prepend(&stack, root);
            list_prepend(&stack, &cell->state);

            /*
             * Next iteration will use the cell->state, but should reuse the
             * current input token. (Do not increment token->next)
             */
        }
        else
        {
            /*
             * We expect to be neither shift nor reduce iff this is the last
             * token.
             */
            assert(token->next == NULL);
            break;
        }
    }

    return root;
}

struct astnode *
create_translation_unit_node(struct listnode *list, struct rule *rule)
{
    unsigned int i, node_size;
    struct astnode *node;
    struct astnode *child;


    if (rule->length_of_nodes == 1)
    {
        node_size = sizeof(struct astnode) + (sizeof(struct astnode *));
        node = malloc(node_size);
        memset(node, 0, node_size);

        node->translation_unit_items[0] = list_item(&list, 0);
        node->translation_unit_items_size = 1;
        node->type = rule->type;
    }
    else if (rule->length_of_nodes == 2)
    {
        /* index 3 is AST_TRANSLATION_UNIT astnode */
        /* index 2 is AST_TRANSLATION_UNIT state */
        child = list_item(&list, 3);

        node_size = sizeof(struct astnode) +
            (sizeof(struct astnode *) * child->translation_unit_items_size);
        node = realloc(child, node_size);

        /* index 1 is AST_EXTERNAL_DECLARATION astnode */
        /* index 0 is AST_EXTERNAL_DECLARATION state */
        child = list_item(&list, 1);

        node->translation_unit_items_size += 1;
        node->translation_unit_items[node->translation_unit_items_size] = child;
    }

    return node;
}

struct astnode *
create_elided_node(struct listnode *list, struct rule *rule)
{
    struct astnode *node;

    node = list_item(&list, 1);
    node->type = rule->type;

    return node;
}

struct astnode *
create_declaration(struct listnode *list, struct rule *rule)
{
    struct astnode *node, *child;

    assert(rule->length_of_nodes == 3);

    /* index 5 is AST_DECLARATION_SPECIFIERS astnode */
    /* index 3 is AST_INIT_DECLARATOR_LIST state */
    /* index 1 is AST_SEMICOLON state */
    node = list_item(&list, 5);
    child = list_item(&list, 3);

    node->declarator_identifier = child->declarator_identifier;
    node->declarators_size = child->declarators_size;
    memcpy(node->declarators, child->declarators, sizeof(struct astnode *) * child->declarators_size);

    node->type = rule->type;
    return node;
}

struct astnode *
create_declaration_specifiers(struct listnode *list, struct rule *rule)
{
    struct astnode *node, *child;

    if (rule->length_of_nodes == 1)
    {
        node = malloc(sizeof(struct astnode));
        memset(node, 0, sizeof(struct astnode));
        child = list_item(&list, 1);
    }
    else if (rule->length_of_nodes == 2)
    {
        /* index 1 is AST_DECLARATION_SPECIFIERS astnode */
        /* index 0 is AST_DECLARATION_SPECIFIERS state */
        node = list_item(&list, 1);

        child = list_item(&list, 3);
    }

    switch (child->type)
    {
        case AST_STORAGE_CLASS_SPECIFIER:
        {
            node->storage_class_specifiers[node->storage_class_specifiers_size++] = child->type;
            break;
        }
        case AST_TYPE_SPECIFIER:
        {
            /* FIXME: How to handle struct, union, or enum specifier? */
            node->type_specifiers[node->type_specifiers_size++] = child->type;
            break;
        }
        case AST_TYPE_QUALIFIER:
        {
            node->type_qualifier[node->type_qualifier_size++] = child->type;
            break;
        }
        default:
        {
            assert(0);
            break;
        }

    }

    node->type = rule->type;
    return node;
}

struct astnode *
create_declarator(struct listnode *list, struct rule *rule)
{
    struct astnode *node, *child;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    if (rule->length_of_nodes == 1)
    {
        /* index 1 is AST_IDENTIFIER astnode */
        child = list_item(&list, 1);

        node->declarator_identifier = child->token->value;
    }
    else if (rule->length_of_nodes == 3)
    {
        child = list_item(&list, 5);
        if (child->type == AST_DIRECT_DECLARATOR)
        {
            node = child;
        }
        else
        {
            /* index 3 is AST_DECLARATOR astnode */
            node = list_item(&list, 3);
        }
    }

    node->type = rule->type;
    return node;
}

struct astnode *
create_(struct listnode *list, struct rule *rule)
{
    struct astnode *node;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    node->type = rule->type;
    return node;
}

struct astnode *
create_binary_op(struct listnode *list, struct rule *rule)
{
    struct astnode *node;
    node = malloc(sizeof(struct astnode));
    memset(node, 0, sizeof(struct astnode));

    node->left = list->data;
    /*
     * next = token
     * next->next = next state
     * next->next->next = next token
     */
    node->right = list->next->next->next->next->data;

    node->type = rule->type;
    return node;
}

#ifdef GENPT
int
main(int argc, char *agv[])
{
    init_parsetable();
}
#endif

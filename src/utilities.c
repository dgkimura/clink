#include <stdio.h>

#include "parser.h"
#include "utilities.h"

void
print_state(struct state *s)
{
    struct listnode *items;
    struct item *item;
    int i;

    printf("state %d\n", s->identifier);
    for (items=s->items; items!=NULL; items=items->next)
    {
        item = (struct item *)items->data;
        printf("  (%d,%d)\n", (int)(item->rewrite_rule-get_grammar()), item->cursor_position);
    }
}

void
print_parsetable(struct parsetable_item *table)
{
    int i, j, num_states = 100; // FIXME: expose total states (state_identifier)..
    struct parsetable_item *cell;

    printf("parse table:\n");

    for (i=0; i<NUM_SYMBOLS; i++)
    {
        printf("%3d ,", i);
    }
    printf("\n");
    for (i=0; i<NUM_SYMBOLS; i++)
    {
        printf("-----");
    }
    printf("\n");

    for (i=0; i<num_states; i++)
    {
        for (j=0; j<NUM_SYMBOLS; j++)
        {
            cell = table + (i * NUM_SYMBOLS) + j;
            if (cell->shift)
            {
                printf("%3ds,", cell->state);
            }
            else if (cell->reduce)
            {
                printf("%3ldr,", cell->rule-get_grammar());
            }
            else if (cell->state)
            {
                printf("%3d ,", cell->state);
            }
            else
            {
                printf("    ,");
            }
        }
        printf("\n");
    }
}

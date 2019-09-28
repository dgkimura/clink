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

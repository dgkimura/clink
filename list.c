#include <stdlib.h>

#include "list.h"

void
list_init(
    struct listnode **head)
{
    *head = NULL;
    list_prepend(head, NULL);
}

void
list_prepend(
    struct listnode **head,
    void *data)
{
    struct listnode *t = malloc(sizeof(struct listnode));
    t->data = data;
    t->next = *head;
    *head = t;
}

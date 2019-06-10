#include <stdlib.h>
#include <string.h>

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

struct listnode *
list_findkey(struct listnode *head, char *key)
{
    struct listnode *current = head;

    while (current != NULL)
    {
        if (strcmp(key, ((struct pair *)current->data)->key) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return current;
}

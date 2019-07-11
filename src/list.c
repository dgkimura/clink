#include <stdlib.h>
#include <string.h>

#include "list.h"

void
list_init(
    struct listnode **head)
{
    *head = NULL;
}

void
list_prepend(
    struct listnode **head,
    void *data)
{
    struct listnode *t = malloc(sizeof(struct listnode));
    t->data = data;
    t->next = *head;

    if (*head != NULL)
    {
        t->tail = (*head)->tail;
    }

    *head = t;
}

void
list_append(
    struct listnode **head,
    void *data)
{
    struct listnode *t = malloc(sizeof(struct listnode));
    t->data = data;
    t->next = *head;

    if (*head != NULL)
    {
        (*head)->tail->next = t;
        (*head)->tail = t;
    }
    else
    {
        (*head) = t;
        (*head)->tail = t;
    }
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

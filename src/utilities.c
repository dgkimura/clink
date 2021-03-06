#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

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
    t->next = NULL;

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

void *
list_item(struct listnode **head, int index)
{
    int i;
    struct listnode *t = *head;

    for (i=0; i<index; i++)
    {
        t = t->next;
    }

    return t->data;
}

int
list_equal(struct listnode *a, struct listnode *b)
{
    struct listnode *_a, *_b;
    int match = 0;

    if (a == b)
    {
        return 1;
    }

    for (_a=a; _a!=NULL; _a=_a->next)
    {
        for (_b=b; _b!=NULL; _b=_b->next)
        {
            if (_a->data == _b->data)
            {
                match = 1;
                break;
            }
        }

        if (!match)
        {
            /*
             * a contained data that b does not contain.
             */
            return 0;
        }
    }

    for (_b=b; _b!=NULL; _b=_b->next)
    {
        for (_a=a; _a!=NULL; _a=_a->next)
        {
            if (_a->data == _b->data)
            {
                match = 1;
                break;
            }
        }

        if (!match)
        {
            /*
             * b contained data that a does not contain.
             */
            return 0;
        }
    }

    return 1;
}

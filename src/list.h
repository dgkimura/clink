#ifndef __LIST_H__
#define __LIST_H__

struct pair
{
    char *key;
    char *value;
};

struct listnode
{
    struct listnode *next;

    /* mantained only for first element in the list */
    struct listnode *tail;
    void *data;
};

void list_init(struct listnode **head);

void list_prepend(struct listnode **head, void *data);

void list_append(struct listnode **head, void *data);

struct listnode *list_findkey(struct listnode *head, char *key);

#endif

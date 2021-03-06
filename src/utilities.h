#ifndef __UTILITIES_H__
#define __UTILITIES_H__

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

#define foreach(item, list) \
    for (item=list; item!=NULL; item=item->next)

void list_init(struct listnode **head);

void list_prepend(struct listnode **head, void *data);

void list_append(struct listnode **head, void *data);

void *list_item(struct listnode **head, int index);

int list_equal(struct listnode *a, struct listnode *b);

#endif

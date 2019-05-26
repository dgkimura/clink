#ifndef __LIST_H__
#define __LIST_H__

struct listnode
{
    struct listnode *next;
    void *data;
};

void list_init(struct listnode **head);

void list_prepend(struct listnode **head, void *data);

#endif

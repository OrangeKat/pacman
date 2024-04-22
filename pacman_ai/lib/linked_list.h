#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

struct Node {
    void *data;
    struct Node *next;
};

typedef struct llist{
    size_t length;
    struct Node *head;
} llist;

// Creates a new empty linked list
llist *init_llist();

// Frees the linked list and all elements inside
void llist_free(llist *list);

// Appends at the start of the list
void llist_prepend(llist *list, void *data);

// Appends at the end of the list
void llist_append(llist *list, void *data);

// Appends the data at the location i in the list
void llist_insert(llist *list, size_t i, void *data);

// Pops the last item in the list
void *llist_fastpop(llist *list);

// Pops the item at location i
void *llist_pop(llist *list, size_t i);

// Reverses the list
void llist_reverse(llist *list);

// Returns the element at the location i in the list 
void *llist_use(llist *list, size_t i);

// Seaches for data in the list and returns 1 if found 0 otherwise
int llist_search(llist *list, void *data);

// Prints the list
void llist_print(llist *list);

#endif

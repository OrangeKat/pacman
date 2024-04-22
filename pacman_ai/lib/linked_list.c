#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"


llist *init_llist(){
    llist *list;
    list = malloc(sizeof(llist));
    list->length = 0;
    list->head = NULL;
    return list;
}

void llist_free(llist *list){
   struct Node *curr = list->head;
   struct Node *next;
   while (curr != NULL){
        next = curr->next;
        free(curr->data);
        free(curr);
        curr = next;
   }
   free(list);
}

void llist_prepend(llist *list, void *data){
    struct Node *new = malloc(sizeof(struct Node));
    new->data = data;
    new->next = list->head;
    list->head = new;
    list->length++;
}

void llist_append(llist *list, void *data){
    struct Node *l = list->head;
    struct Node *new = malloc(sizeof(struct Node));
    new->data = data;
    new->next = NULL;
    if (l == NULL){
        list->head = new;
    }
    else{
        while (l->next != NULL){
            l = l->next;
        }
        l->next = new;
    }
    list->length++;
}

void llist_insert(llist *list, size_t i, void *data){
    if (i == 0){
        llist_prepend(list, data);
    }
    else{
        if (list->length <= i){
            llist_append(list, data);
        }
        else{
            struct Node *l = list->head;
            while (i - 1 != 0){
                l = l->next;
                i--;
            }
            struct Node *new = malloc(sizeof(struct Node));
            new->data = data;
            new->next = l->next;
            l->next = new;
        }
    }
    list->length++;
}

void *llist_fastpop(llist *list){
    struct Node *n = list->head;
    void *key = NULL;
    while (n->next != NULL){
        n = n->next;
    }
    key = n->data;
    free(n);
    return key;
}

void *llist_pop(llist *list, size_t i){
    void *data;
    if (i >= list->length){
        data = llist_fastpop(list);
    }
    else{
        struct Node *delete;
        if (i == 0){
            data = list->head->data;
            delete = list->head;
            list->head = list->head->next;
            data = delete->data;
            free(delete);
        }
        else{
            delete = list->head;
            while (i - 1 != 0){
                delete = delete->next;
                i--;
            }
            struct Node *tmp = delete;
            delete = delete->next;
            tmp->next = delete->next;
            data = delete->data;
            free(delete);
        }
    }
    list->length--;
    return data;
}

void llist_reverse(llist *list){
    struct Node *prev = NULL;
    struct Node *curr = list->head;
    struct Node *next;
    while (curr != NULL){
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    list->head = prev;
}

void *llist_use(llist *list, size_t i){
    struct Node *l = list->head;
    if (list->length <= i){
        return NULL;
    }
    else{
        while (i != 0){
            i--;
            l = l->next;
        }
        return l->data;
    }
}

int llist_search(llist *list, void *data){
    struct Node *l = list->head;
    while (l != NULL){
        if (l->data == data){
            return 1;
        }
        l = l->next;
    }
    return 0;
}

void llist_print(llist *list){
    struct Node *l = list->head;
    while (l != NULL){
        printf("%p ",l->data);
        l = l->next;
    }
    putchar('\n');
}

#include "mapgen.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

llist* in_dict(Dict* dictionary, char* key){
    struct Index* i = dictionary->head;
    while (i != NULL){
        if (i->key == key){
            return i->value;
        }
        i = i->next;
    }
    return NULL;
}

Dict* init_dict(){
    Dict* new = malloc(sizeof(Dict));
    new->head = NULL;
    return new;
}

void dict_add(Dict* dictionary, char* key, char* value){
    struct Index* i = dictionary->head;
    while (i->next != NULL && i->key != key){
        i = i->next;
    }
    if (i->key != key){
        struct Index* new = malloc(sizeof(struct Index));
        new->key = key;
        new->next = NULL;
        new->value = init_llist();
        i = new;
    }
    llist_append(i->value, value);
}

Dict* learn_mapping(char* path){
    FILE *f;
    char buffer[ROW * COL];
    f = fopen(path, "r");
    if (f == NULL){
        return NULL;
    }

    while (fgets(buffer, sizeof(buffer), f) != NULL){
        for (int i = 0; i < ROW * COL; i++){
            i++;
        }
    }
    fclose(f);

    return NULL;
};

char* generate_map(Dict* dictionary){
    char* map = malloc(sizeof(char) * ROW * COL);
    
    return map;
};

int main(){
    char* map;    
    for (int i = 0; i < ROW * COL; i++){
        putchar(map[i]);
        if (i % COL == 0){
            putchar('\n');
        }
    }
}

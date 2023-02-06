#ifndef UTILS_H
#define UTILS_H

#define INIT_SIZE 100

/*
Stack structure:
    - size (int): the amount of items left in the stack
    - stack (char*): the stack containing the information
 */
struct Stack{
    int size;
    char *stack;
};

// Push the param *data* into the *stack* and update the size value.
void push_stack(struct Stack *stack, char data);

// Pop from the *stack* and return *data* at the top of the stack update the size
// value.
char pop_stack(struct Stack *stack);

// Checks if the *stack* is empty if true return 1 else 0. 
int is_stack_empty(struct Stack *stack);

// Init the new empty *stack* with size = 0 and stack[INIT_SIZE]
void init_stack(struct Stack *stack);

// Frees the *stack*
void free_stack(struct Stack *stack);

/*
Queue Structure:
    - length (int): length of the queue
    - queue (int**): queue containing the data in format (int*)
*/
struct Queue{
    int length;
    int **queue;
};

// Push the *data* into the *queue* and update the length
void push_queue(struct Queue *queue, int* data);

// Pop the *data* from the end of the *queue* and returns it. update the length
int *pop_queue(struct Queue *queue);

// Checks if queue is empty return 1 if true else 0
int is_queue_empty(struct Queue *queue);

// Init the new empty *queue* with length = 0 and queue[INIT_SIZE][2]
void init_queue(struct Queue *queue);

// Frees the queue
void free_queue(struct Queue *queue);

#endif

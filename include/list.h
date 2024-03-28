#pragma once

// Constants for visualization window dimensions and layout
typedef struct lnode {
    void *data;         // Pointer to data
    struct lnode *next; // Pointer to the next node
} lnode;

// Define a structure for a generic list
typedef struct {
    lnode *head;        // Pointer to the head of the list
    lnode *pointer;     // General purpose pointer that can be used by some functions
                        // By default, it points to the same element as head, but can be changed
} type_list;

// Define type aliases for List, Stack, and Queue
typedef type_list *List;
typedef type_list *Stack;
typedef type_list *Queue;

// Function prototypes for list operations

// Creates and initializes a new list
List list();

// Frees entire list, including data by using function pointed to by free_data pointer
// If free_data is NULL, data is left unfreed
// Function pointed to by free_data has to take one argument which is a pointer to the data type being freed
void list_(List list, void (*free_data)());

// Returns the length of the list
int list_length(List list);

// Checks if the list is a valid list
int list_is_valid(List list);

// Checks if the list is empty
int list_is_empty(List list);

// Prints the elements of the list
void list_print(List list, void (*printel)());

// Adds data at the beginning of a list
void list_add(List list, void *data);

// Adds data at position'th place in the list
void list_add_at(List list, int position, void *data);

// Adds data at the end of a list
void list_add_end(List list, void *data);

// Returns pointer to the data of the first node in the list
void *list_get(List list);

// Returns pointer to the data of the nth element's data field in the list (counted from one)
// Returns NULL if position is outside of range
void *list_get_nth(List list, int position);

// Returns pointer to the data of the last node in the list
void *list_get_last(List list);

// Removes the first node from the list
int list_remove(List list);

// Removes the node at position'th place in the list
int list_remove_at(List list, int position);

// Removes the last node from the list
int list_remove_end(List list);

// Appends list2 to list1
// list2 is left empty
void list_append(List list1, List list2);

// Reverses the order of elements in the list
void list_reverse(List list);

// Sorts the list using the comparison function comp
void list_sort(List l, int (*comp)());

// Iterates through all the elements in the list
int list_foreach(List list, void **el);

// Function prototypes for stack operations

// Creates and initializes a new stack
Stack stack();

// Frees entire stack, including data by using function pointed to by free_data pointer
// If free_data is NULL, data is left unfreed
// Function pointed to by free_data has to take one argument which is a pointer to the data type being freed
void stack_(Stack stack, void (*free_data)());

// Checks if the stack is empty
int stack_is_empty(Stack stack);

// Pushes data onto the stack
void push(Stack stack, void *data);

// Pops data from the stack
void *pop(Stack stack);

// Function prototypes for queue operations

// Creates and initializes a new queue
Queue queue();

// Frees entire queue, including data by using function pointed to by free_data pointer
// If free_data is NULL, data is left unfreed
// Function pointed to by free_data has to take one argument which is a pointer to the data type being freed
void queue_(Queue queue, void (*free_data)());

// Checks if the queue is empty
int queue_is_empty(Queue queue);

// Enqueues data into the queue
void enqueue(Queue queue, void *data);

// Dequeues data from the queue
void *dequeue(Queue queue);

#pragma once

typedef struct lnode{
    void *data;
    struct lnode *next;
}lnode;

typedef struct{
    lnode *head;
}type_list;

typedef type_list *List;
typedef type_list *Stack;
typedef type_list *Queue;

List list();

//Frees entire list, including data by using funtion pointed to by free_data pointer.
//If free_data is NULL data is left unfreed.
//Function pointed to by free_data has to take one argument wich is pointer to the data type being freed. 
void list_(List list, void (*free_data)());
int list_length(List list);
int list_is_empty(List list);
void list_print(List list, void (*printel)());

void list_add(List list, void *data);
void list_add_at(List list, int position, void *data);
void list_add_end(List list, void *data);

void *list_get(List list);

//Returns pointer to nth element's data field in the list (counted from one).
//In case position is outside of rage returns NULL.
void *list_get_nth(List list, int position);

void *list_get_last(List list);

int list_remove(List list);
int list_remove_at(List list, int position);
int list_remove_end(List list);

//TODO take function that compare data and deside what element to pick
//int list_search(List list, void *data);
void list_append(List list1, List list2);
void list_reverse(List list);

Stack stack();
void stack_(Stack stack, void (*free_data)());
int stack_is_empty(Stack stack);
void push(Stack stack, void *data);
void *pop(Stack stack);

Queue queue();
void queue_(Queue queue, void (*free_data)());
int queue_is_empty(Queue queue);
void enqueue(Queue queue, void *data);
void *dequeue(Queue queue);










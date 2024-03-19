#include <stdio.h>
#include <stdlib.h>
#include <assert.h> //TODO remove aserts

#include <list.h>

//CONSTRUCTOR AND DESTRUCTOR FOR NODE

lnode *create_lnode(void *data, lnode *next)
{
    lnode *n = malloc(sizeof(lnode));

    n->data = data;
    n->next = next;

    return n;
}

lnode *delete_lnode(lnode *del)
{
    lnode *tmp = del->next;

    free(del);

    return tmp;
}

//LIST HELPER FUNCTIONS

lnode *list_get_last_node(List list)
{
    if (list_is_empty(list))
    {
        return NULL;
    }

    lnode *cur = list->head;

    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    
    return cur;
}

//returns NULL when seconsd last elment doesn't exists (list size is 0 or 1)
//last argument is set to 1 if returned element is second last else 0 (in case of list size 1 elmement if return and second_last == 0)
lnode *list_get_second_last_node(List list, int *is_second_last)
{
    *is_second_last = 1;

    lnode *second_last = list->head;

    for (lnode *cur = list->head; cur->next != NULL; cur = cur->next)
    {
        second_last = cur;
    }

    //if only one lmement in list returned element is not second last
    if (list->head->next == NULL)
    {
        *is_second_last = 0;
    }

    return second_last;
}

//Returns pointer to nth node in the list (counted from one).
//In case place is outsid of rage of list returns NULL.
lnode *list_get_nth_node(List list, int position)
{
    assert(position > 0);

    lnode *cur = list->head;

    while (cur != NULL)
    {
        if (position == 1)
        {
            return cur;
        }

        position--;

        cur = cur->next;
    }

    return NULL;
}


//LIST LIBRARY FUNCTIONS

List list()
{
    List list = malloc(sizeof(type_list));
    list->head = NULL;

    return list;
}

void list_(List list, void (*free_data)())
{
    lnode *temp = NULL;

    for (lnode *cur = list->head; cur != NULL; cur = temp)
    {
        temp = cur->next;

        if (free_data != NULL)
        {
            (free_data)(cur->data);
        }

        free(cur);    
    }

    free(list);

    list = NULL;
}

int list_is_empty(List list)
{
    if (list->head == NULL)
    {
        return 1;
    }

    return 0;
}

int list_length(List list)
{
    int length = 0;

    for (lnode *cur = list->head; cur != NULL; cur = cur->next)
    {
        length += 1;
    }

    return length;
}

void list_print(List list, void (*printel)())
{
    printf("(");

    int first_printed = 0;

    for(lnode *cur = list->head; cur != NULL; cur = cur->next)
    {
        if (!first_printed)
        {
            if (printel != NULL)
            {
                (printel)(cur->data);
            }
            else
            {
                printf("%p", cur->data);
            }

            first_printed = 1;
        }
        else
        {
            printf(", ");

            if (printel != NULL)
            {
                (printel)(cur->data);
            }
            else
            {
                printf("%p", cur->data);
            }
        }
    }

    printf(")\n");
}

void list_add(List list, void *data)
{
    list->head = create_lnode(data, list->head);
}

void list_add_at(List list, int position, void *data)
{
    assert(position > 0);

    lnode *nth = list_get_nth_node(list, position - 1);

    nth->next = create_lnode(data, nth->next);
}

void list_add_end(List list, void *data)
{
    if (list_is_empty(list))
    {
        list_add(list, data);
    }
    else
    {
        lnode *last = list_get_last_node(list);

        last->next = create_lnode(data, NULL);
    }
}

void *list_get(List list)
{
    if (list_is_empty(list))
    {
        return NULL;
    }

    return list->head->data;
}

void *list_get_nth(List list, int position)
{    
    lnode *n = list_get_nth_node(list, position);

    if (n == NULL)
    {
        return NULL;
    }

    return n->data;    
}

void *list_get_last(List list)
{
    lnode *n = list_get_last_node(list);

    if (n == NULL)
    {
        return NULL;
    }
    
    return n->data;
}

int list_remove(List list)
{
    if (!list_is_empty(list))
    {
        list->head = delete_lnode(list->head);
        
        return 0;
    }

    return -1;
}

int list_remove_at(List list, int position)
{
    assert(position > 0);

    lnode *prev = list_get_nth_node(list, position - 1);

    if (prev == NULL)
    {
        return -1;
    }

    prev->next = delete_lnode(prev->next);

    return 0;
}

int list_remove_end(List list)
{    
    if (list_is_empty(list))
    {
        return -1;
    }

    //if list not empty delete will happen

    int is_seclast;
    lnode *seclast = list_get_second_last_node(list, &is_seclast);

    if (is_seclast)
    {
        seclast->next = delete_lnode(seclast->next);
    }
    else
    {
        list->head = delete_lnode(list->head);
    }

    return 0;
}

// int list_search(List list, int data)
// {
//     if (list_is_empty(list))
//     {
//         return -1;
//     }

//     int place = 1;

//     for (node *cur = list->head; cur != NULL; cur = cur->next)
//     {
//         if (cur->data == data)
//         {
//             return place;
//         }

//         place++;
//     }

//     return -1;
// }

void list_append(List list1, List list2)
{
    lnode *end = list_get_last_node(list1);

    end->next = list2->head;

    list2->head = NULL;
}

void list_reverse(List list)
{
    lnode *reversed = NULL;
    lnode *next = NULL;

    for (lnode *cur = list->head; cur != NULL; cur = next)
    {
        next = cur->next;

        cur->next = reversed;
        reversed = cur;
    }

    list->head = reversed;
}

//STACK IMPLEMENTATION

Stack stack()
{
    return list();
}

void stack_(Stack stack, void (*free_data)())
{
    list_(stack, free_data);
}

int stack_is_empty(Stack stack)
{
    return list_is_empty(stack);
}

void push(Stack stack, void *data)
{
    list_add(stack, data);
}

void *pop(Stack stack)
{
    void *data = stack->head->data;

    list_remove(stack);

    return data;
}


//QUEUE IMPLEMENTATION

Queue queue()
{
    return list();
}

void queue_(Queue queue, void (*free_data)())
{
    list_(queue, free_data);
}

int queue_is_empty(Queue queue)
{
    return list_is_empty(queue);
}

void enqueue(Queue queue, void *data)
{
    list_add(queue, data);
}

void *dequeue(Queue queue)
{
    int n;

    lnode *seclast = list_get_second_last_node(queue, &n);

    void *data;

    if (n)
    {
        data = seclast->next->data;

        seclast->next = delete_lnode(seclast->next);
    }
    else
    {
        data = seclast->data;

        queue->head = delete_lnode(queue->head);
    }

    return data;
}
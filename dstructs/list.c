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
//In case place is outsid of rage, returns pointer to last node in the list and sets outrange to 1 else outrage is set to 0.
//If list is empty retur NULL.
lnode *list_get_nth_node(List list, int position, int *outrange)
{
    assert(position > 0);

    if (list_is_empty(list))
    {
        *outrange = 1;
        return NULL;
    }

    if (position == 1)
    {
        *outrange = 0;
        return list->head;
    }

    lnode *cur = list->head;

    while (cur->next != NULL)
    {
        if (position == 2)
        {
            *outrange = 0;
            return cur->next;
        }

        position--;

        cur = cur->next;
    }

    *outrange = 1;
    return cur;
}


//LIST LIBRARY FUNCTIONS

List list()
{
    List list = malloc(sizeof(type_list));
    list->head = NULL;
    list->pointer = NULL;
    return list;
}

void list_(List list, void (*free_data)())
{
    assert(list_is_valid);

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

int list_is_valid(List list)
{
    if (list == NULL)
    {
        return 0;
    }

    return  1;
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
    assert(list_is_valid(list));

    list->head = create_lnode(data, list->head);
    list->pointer = list->head;
}

void list_add_at(List list, int position, void *data)
{
    assert(list_is_valid(list));
    assert(position > 0);

    if (position == 1)
    {
        list_add(list, data);
        return;
    }

    int outrange;
    lnode *nth = list_get_nth_node(list, position - 1, &outrange);

    if (nth == NULL)
    {
        list_add(list, data);
    }
    else
    {
        nth->next = create_lnode(data, nth->next);
    }
}

void list_add_end(List list, void *data)
{
    assert(list_is_valid(list));

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
    assert(list_is_valid(list));

    if (list_is_empty(list))
    {
        return NULL;
    }

    return list->head->data;
}

void *list_get_nth(List list, int position)
{
    assert(list_is_valid(list));

    int outrage;
    lnode *n = list_get_nth_node(list, position, &outrage);

    if (n == NULL)
    {
        return NULL;
    }

    return n->data;    
}

void *list_get_last(List list)
{
    assert(list_is_valid(list));

    lnode *n = list_get_last_node(list);

    if (n == NULL)
    {
        return NULL;
    }
    
    return n->data;
}

int list_remove(List list)
{
    assert(list_is_valid(list));

    if (!list_is_empty(list))
    {
        list->head = delete_lnode(list->head);
        list->pointer = list->head;
        
        return 0;
    }

    return -1;
}

int list_remove_at(List list, int position)
{
    assert(list_is_valid(list));
    assert(position > 0);

    int outrange;
    lnode *prev = list_get_nth_node(list, position - 1, &outrange);

    if (prev == NULL)
    {
        return -1;
    }

    prev->next = delete_lnode(prev->next);

    return 0;
}

int list_remove_end(List list)
{    
    assert(list_is_valid(list));

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
        list->pointer = list->head;
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

void list_sort(List l, int (*comp)())
{
    assert(list_is_valid(l));

    if (list_is_empty(l))
    {
        return;
    }

    List sorted = list();

    while (l->head != NULL)
    {
        lnode *prev_highest = l->head;
        int first_heighest = 1;
        for (lnode *cur = l->head; cur->next != NULL; cur = cur->next)
        {
            if (first_heighest)
            {
                if ((*comp)(l->head->data, cur->next->data) == -1)
                {
                    first_heighest = 0;
                    prev_highest = cur;
                }

                continue;
            }

            if ((*comp)(prev_highest->next->data, cur->next->data) == -1)
            {
                prev_highest = cur;
            }
        }

        //Remove highest from unsorted list
        lnode *highest;
        if (first_heighest) //if last element in the l
        {
            highest = l->head;
            l->head = highest->next;
        }
        else
        {
            highest = prev_highest->next;
            prev_highest->next = highest->next;
        }

        //Put highest into sorted list
        lnode *temp = sorted->head;
        sorted->head = highest;
        highest->next = temp;
    }


    l->head = sorted->head;
    l->pointer = l->head;

    sorted->head = NULL;
    list_(sorted, NULL);

}

void list_append(List list1, List list2)
{
    assert(list_is_valid(list1) && list_is_valid(list2));

    lnode *end = list_get_last_node(list1);

    end->next = list2->head;

    list2->head = NULL;
    list2->pointer = NULL;
}

void list_reverse(List list)
{
    assert(list_is_valid(list));

    lnode *reversed = NULL;
    lnode *next = NULL;

    for (lnode *cur = list->head; cur != NULL; cur = next)
    {
        next = cur->next;

        cur->next = reversed;
        reversed = cur;
    }

    list->head = reversed;
    list->pointer = reversed;
}

int list_foreach(List list, void **el)
{
    assert(list_is_valid(list));

    if (list->pointer != NULL)
    {
        *el = list->pointer->data;
        list->pointer = list->pointer->next;
        return 1;
    }

    list->pointer = list->head;
    return 0;
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
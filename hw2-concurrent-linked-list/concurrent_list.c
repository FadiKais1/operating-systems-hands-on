#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

struct node {
    int value;
    struct node* next;
    pthread_mutex_t lock;
};

struct list {
    node* head;
};

void print_node(node* node)
{
    // DO NOT DELETE
    if(node)
    {
        printf("%d ", node->value);
    }
}

static node* create_node(int value)
{
    node* new_node = (node*)malloc(sizeof(node));
    if(new_node == NULL)
    {
        return NULL;
    }

    new_node->value = value;
    new_node->next = NULL;

    if(pthread_mutex_init(&new_node->lock, NULL) != 0)
    {
        free(new_node);
        return NULL;
    }

    return new_node;
}

list* create_list()
{
    list* new_list = (list*)malloc(sizeof(list));
    if(new_list == NULL)
    {
        return NULL;
    }

    node* head = create_node(INT_MIN);
    node* tail = create_node(INT_MAX);

    if(head == NULL || tail == NULL)
    {
        if(head != NULL)
        {
            pthread_mutex_destroy(&head->lock);
            free(head);
        }

        if(tail != NULL)
        {
            pthread_mutex_destroy(&tail->lock);
            free(tail);
        }

        free(new_list);
        return NULL;
    }

    head->next = tail;
    new_list->head = head;

    return new_list;
}

void delete_list(list* list)
{
    if(list == NULL)
    {
        return;
    }

    node* curr = list->head;

    while(curr != NULL)
    {
        node* next = curr->next;
        pthread_mutex_destroy(&curr->lock);
        free(curr);
        curr = next;
    }

    free(list);
}

void insert_value(list* list, int value)
{
    if(list == NULL)
    {
        return;
    }

    node* new_node = create_node(value);
    if(new_node == NULL)
    {
        return;
    }

    node* prev = list->head;
    pthread_mutex_lock(&prev->lock);

    node* curr = prev->next;
    pthread_mutex_lock(&curr->lock);

    while(curr->next != NULL && curr->value < value)
    {
        pthread_mutex_unlock(&prev->lock);
        prev = curr;
        curr = curr->next;
        pthread_mutex_lock(&curr->lock);
    }

    new_node->next = curr;
    prev->next = new_node;

    pthread_mutex_unlock(&curr->lock);
    pthread_mutex_unlock(&prev->lock);
}

void remove_value(list* list, int value)
{
    if(list == NULL)
    {
        return;
    }

    node* prev = list->head;
    pthread_mutex_lock(&prev->lock);

    node* curr = prev->next;
    pthread_mutex_lock(&curr->lock);

    while(curr->next != NULL)
    {
        if(curr->value == value)
        {
            prev->next = curr->next;

            pthread_mutex_unlock(&curr->lock);
            pthread_mutex_unlock(&prev->lock);

            pthread_mutex_destroy(&curr->lock);
            free(curr);
            return;
        }

        pthread_mutex_unlock(&prev->lock);
        prev = curr;
        curr = curr->next;
        pthread_mutex_lock(&curr->lock);
    }

    pthread_mutex_unlock(&curr->lock);
    pthread_mutex_unlock(&prev->lock);
}

void print_list(list* list)
{
    if(list == NULL)
    {
        printf("\n"); // DO NOT DELETE
        return;
    }

    node* curr = list->head;
    pthread_mutex_lock(&curr->lock);

    node* next = curr->next;
    pthread_mutex_lock(&next->lock);

    pthread_mutex_unlock(&curr->lock);
    curr = next;

    while(curr->next != NULL)
    {
        print_node(curr);

        next = curr->next;
        pthread_mutex_lock(&next->lock);

        pthread_mutex_unlock(&curr->lock);
        curr = next;
    }

    pthread_mutex_unlock(&curr->lock);

    printf("\n"); // DO NOT DELETE
}

void count_list(list* list, int (*predicate)(int))
{
    int count = 0; // DO NOT DELETE

    if(list != NULL && predicate != NULL)
    {
        node* curr = list->head;
        pthread_mutex_lock(&curr->lock);

        node* next = curr->next;
        pthread_mutex_lock(&next->lock);

        pthread_mutex_unlock(&curr->lock);
        curr = next;

        while(curr->next != NULL)
        {
            if(predicate(curr->value))
            {
                count++;
            }

            next = curr->next;
            pthread_mutex_lock(&next->lock);

            pthread_mutex_unlock(&curr->lock);
            curr = next;
        }

        pthread_mutex_unlock(&curr->lock);
    }

    printf("%d items were counted\n", count); // DO NOT DELETE
}

void swap_values(list* list, int val1, int val2)
{
    if(list == NULL || val1 == val2)
    {
        return;
    }

    node* curr = list->head;
    pthread_mutex_lock(&curr->lock);

    node* next = curr->next;
    pthread_mutex_lock(&next->lock);

    pthread_mutex_unlock(&curr->lock);
    curr = next;

    node* first = NULL;

    while(curr->next != NULL)
    {
        if(curr->value == val1 || curr->value == val2)
        {
            first = curr;
            break;
        }

        next = curr->next;
        pthread_mutex_lock(&next->lock);

        pthread_mutex_unlock(&curr->lock);
        curr = next;
    }

    if(first == NULL || curr->next == NULL)
    {
        pthread_mutex_unlock(&curr->lock);
        return;
    }

    int needed = (first->value == val1) ? val2 : val1;

    curr = first->next;
    pthread_mutex_lock(&curr->lock);

    while(curr->next != NULL)
    {
        if(curr->value == needed)
        {
            int temp = first->value;
            first->value = curr->value;
            curr->value = temp;

            pthread_mutex_unlock(&curr->lock);
            pthread_mutex_unlock(&first->lock);
            return;
        }

        next = curr->next;
        pthread_mutex_lock(&next->lock);

        pthread_mutex_unlock(&curr->lock);
        curr = next;
    }

    pthread_mutex_unlock(&curr->lock);
    pthread_mutex_unlock(&first->lock);
}

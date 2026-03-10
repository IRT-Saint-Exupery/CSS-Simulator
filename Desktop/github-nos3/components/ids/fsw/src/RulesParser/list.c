#include <stdlib.h>
#include "list.h"

/*
 * The node type represents the node in a list. A node holds a reference to
 * the previous and the element of the list it is in. If a node is the first
 * (or last) element in the list, the previous (or next) node will be null. 
 */
typedef struct node
{
    void *user_data;
    struct node *previous;
    struct node *next;
} node_t;

void add_node(list_t *list, void* user_data)
{
    node_t *node = malloc(sizeof(node_t));
    node->user_data = user_data;
    node->previous = NULL;
    node->next = NULL;

    if (list->list_size == 0) 
    {
        list->first_node = node;
        list->last_node = node;
    } else
    {
        list->last_node->next = node;
        node->previous = list->last_node;
        list->last_node = node;
    }

    list->list_size += 1;
}

node_t* get_node(list_t *list, int index)
{
    // Index check
    if (index >= list->list_size)
    {
        return NULL;
    }

    node_t *node = list->first_node;
    for (int i = 0; i < index; i++) 
    {
        node = node->next;
    }

    return node;
}

int remove_first(list_t *list)
{
    if (list->list_size == 0)
    {
        return 0;
    }

    if (list->list_size == 1)
    {
        // Free the user data and the start node
        list->remove_handler(list->first_node->user_data);
        free(list->first_node);

        // Remove both from list
        list->first_node = NULL;
        list->last_node = NULL;
    } else 
    {
        node_t *first_node = list->first_node;
        list->first_node = first_node->next;

        // Set previous of 2nd node to NULL since it is first now
        list->first_node->previous = NULL;

        list->remove_handler(first_node->user_data);
        free(first_node);
    }

    list->list_size -= 1;

    return 1;
}

int remove_last(list_t *list)
{
    if (list->list_size == 0)
    {
        return 0;
    }

    if (list->list_size == 1) {
        return remove_first(list);
    } else
    {
        node_t *last_node = list->last_node;
        list->last_node = last_node->previous;

        // Set next of previous to last node to NULL since it is the last now
        list->last_node->next = NULL;

        list->remove_handler(last_node->user_data);
        free(last_node);

        list->list_size -= 1;
    }

    return 1;
}

int remove_node(list_t *list, unsigned int index)
{
    // Index check
    if (index >= list->list_size)
    {
        return 0;
    }

    if (index == 0)
    {
        return remove_first(list);
    }

    if (index == list->list_size - 1)
    {
        return remove_last(list);
    }

    node_t *node = get_node(list, index);
    node->previous->next = node->next;
    node->next->previous = node->previous;
    
    // Free user data and node
    list->remove_handler(node->user_data);
    free(node);

    list->list_size -= 1;

    return 1;
}

void* get(list_t *list, int index)
{
    node_t *node = get_node(list, index);
    if (node == NULL)
    {
        return NULL;
    }

    return node->user_data;
}
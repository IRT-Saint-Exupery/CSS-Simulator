/*
 * This file contains all functions that will be used in the list creation,
 * and all useful actions that can be made in a list: add, remove and get.
 */

#pragma once

/*
 * The remove handler type represents the function that will be called to
 * when remove_node, remove_first or remove last is be called. This is
 * used to clean the node's user data. The remove handler 
 * 
 * For instance let's take a list of char* values. Each value has been
 * malloc-ed and, upon removal, needs to be freed. The remove handler will
 * be of the form: 
 * 
 * void free_value(void *value)
 * {
 *    free((char *)value);
 * }
 * 
 * Note that if the value is a pointer to a struct that contains pointers,
 * you will need to free the inside pointers as well as the struct pointer.
 * For instance:
 * 
 * struct test
 * {
 *    char *name;
 * }
 * 
 * void free_value(void *value)
 * {
 *    struct test *value_struct = (struct test *)value;
 *    free(value_struct->name);
 *    free(value_struct);
 * }
 * 
 */
typedef void (*remove_handler_t)(void *);

typedef struct node node_t;

/*
 * The list type represents a double linked list. A list holds the list
 * size, the reference of the first list element, of the last list element
 * and the user data remove handler.  
 */
typedef struct list {
    unsigned int list_size;
    node_t *first_node;
    node_t *last_node;
    remove_handler_t remove_handler;
} list_t;

/*
 * Adds a node to the specified list. The void * parameter corresponds 
 * to the user data. 
 */
void add_node(list_t *, void *);

/*
 * Removes the node from the specified list at the specified index.
 */
int remove_node(list_t *, unsigned int);

/*
 * Removes the first node from the list.
 */
int remove_first(list_t *);

/*
 * Removes the last node from the list.
 */
int remove_last(list_t *);

/*
 * Returns the user data stored in the list at the specified index, or NULL 
 * if the index is not correct.
 */
void* get(list_t *, int);
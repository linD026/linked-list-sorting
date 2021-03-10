#include <stdlib.h>

#include "c_map.h"

void list_add_node_t(node_t **list, node_t *node_t) 
{
    node_t->next = *list;
    *list = node_t;
}

void list_concat(node_t **left, node_t *right) 
{
    while (*left)
        left = &((*left)->next);
    *left = right;
}

node_t *get_list_tail(node_t **left) 
{
    while ((*left) && (*left)->next)
        left = &((*left)->next);
    return *left;
}

int get_list_length(node_t **left) 
{
    int n = 0;
    while (*left) {
        ++n;
        left = &((*left)->next);
    }
    return n;
}


node_t *list_make_node_t(node_t *list, int n) 
{
    node_t *node = malloc(sizeof(node_t));
    node->value = n;
    node->next = list;
    return node;
}

void list_free(node_t **list) 
{
    node_t *node = (*list)->next;
    while (*list) {
        free(*list);
        *list = node;
        if (node)
            node = node->next;
    }
}

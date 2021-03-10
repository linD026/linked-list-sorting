#pragma once

#include <stddef.h>

typedef enum { C_MAP_RED, C_MAP_BLACK, C_MAP_DOUBLE_BLACK } c_map_color_t;

/*
 * Store the key, data, and values of each element in the tree.
 * This is the main basis of the entire tree aside from the root struct.
 */
typedef struct c_map_node {
    void *key, *data;
    struct c_map_node *left, *right, *up;
    c_map_color_t color;
} c_map_node_t;

typedef struct __node {                   
    int value;
    struct __node *next;
    struct __node *left, *right, *up;
    c_map_color_t color;
} node_t;


typedef struct {
    struct __node *prev, *node;
    size_t count;
} c_map_iter_t;

struct c_map_internal {
    struct __node *head;

    /* Properties */
    size_t key_size, element_size, size;

    c_map_iter_t it_end, it_most, it_least;

    int (*comparator)(void *, void *);
};
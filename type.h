#pragma once

#include <stddef.h>
/*
typedef enum { C_MAP_RED, C_MAP_BLACK, C_MAP_DOUBLE_BLACK } c_map_color_t;

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
*/

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


typedef struct __node { 
    unsigned long  color;
    struct __node *left;
    struct __node *right;
    struct __node *next;
    long value;
} node_t __attribute__((aligned(sizeof(long))));

#define C_MAP_RED          0
#define C_MAP_BLACK        1
#define rb_parent(r)   ((node_t *)((r)->color & ~3))
#define rb_color(r)   ((r)->color & 1)
#define rb_set_parent(r, p) do{ (r)->color =  rb_color(r) | (unsigned long)(p); } while (0)
#define rb_set_red(r)  do { (r)->color &= ~1; } while (0)
#define rb_set_black(r)  do { (r)->color |= 1; } while (0)
#define rb_is_red(r)      (!rb_color(r))
#define rb_is_black(r)    (rb_color(r))

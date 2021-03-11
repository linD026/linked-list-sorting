/*
 * C Implementation for C++ std::map using red-black tree.
 *
 * Any data type can be stored in a c_map, just like std::map.
 * A c_map instance requires the specification of two file types.
 *   1. the key;
 *   2. what data type the tree node will store;
 * It will also require a comparison function to sort the tree.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include "type.h"

#if defined(__GNUC__) || defined(__clang__)
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif


enum { _CMP_LESS = -1, _CMP_EQUAL = 0, _CMP_GREATER = 1 };

/* Integer comparison */
static inline int c_map_cmp_int(void *arg0, void *arg1)
{
    int *a = (int *) arg0, *b = (int *) arg1;
    return (*a < *b) ? _CMP_LESS : (*a > *b) ? _CMP_GREATER : _CMP_EQUAL;
}

/* Unsigned integer comparison */
static inline int c_map_cmp_uint(void *arg0, void *arg1)
{
    unsigned int *a = (unsigned int *) arg0, *b = (unsigned int *) arg1;
    return (*a < *b) ? _CMP_LESS : (*a > *b) ? _CMP_GREATER : _CMP_EQUAL;
}



#define container_of(ptr, type, member) ({ \
     const typeof( ((type *)0)->member ) *__mptr = (ptr); \
     (type *)( (char *)__mptr - offsetof(type,member) );})
     
/*
 * Store access to the head node, as well as the first and last nodes.
 * Keep track of all aspects of the tree. All c_map functions require a pointer
 * to this struct.
 */
typedef struct c_map_internal *c_map_t;

/* Constructor */
c_map_t c_map_new(size_t, size_t, int (*)(void *, void *));

/* Add function */
bool c_map_insert(c_map_t obj, node_t *node, void *value);

/* Destructor */
void c_map_delete(c_map_t);

node_t *c_map_first(c_map_t obj);
node_t *c_map_next(node_t *node);

static node_t *c_map_create_node(node_t *node);

#define c_map_init(key_type, element_type, __func) \
    c_map_new(sizeof(key_type), sizeof(element_type), __func)

#define c_map_iter_value(it, type) (*(type *) (it)->node->data)

#ifdef __cplusplus
};  // ifdef __cplusplus
#endif
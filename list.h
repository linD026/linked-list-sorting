#pragma once

#include "type.h"

void list_add_node_t(node_t **list, node_t *node_t);
void list_concat(node_t **left, node_t *right);
node_t *get_list_tail(node_t **left);
int get_list_length(node_t **left);
node_t *list_make_node_t(node_t *list, int n); 
void list_free(node_t **list);
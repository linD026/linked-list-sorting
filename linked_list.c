#include <sys/random.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>

typedef struct __node {                   
    int value;
    struct __node *next;
} node_t;

static inline void list_add_node_t(node_t **list, node_t *node_t) {
    node_t->next = *list;
    *list = node_t;
}


static inline void list_concat(node_t **left, node_t *right) {
    while (*left)
        left = &((*left)->next);
    *left = right;
}

static inline node_t *get_list_tail(node_t **left) {
    while ((*left) && (*left)->next)
        left = &((*left)->next);
    return *left;
}

void insert_sorted(node_t *entry, node_t **list)
{
    while (*list && (*list)->value < entry->value)
        list = &(*list)->next;
    entry->next = *list;
    *list = entry;
}

void insertsort(node_t **list) 
{
    node_t *sorted = NULL;
    node_t *cur = *list;
    while (cur) {
        node_t *node = cur;
        cur = cur->next;
        insert_sorted(node, &sorted);
    }
    *list = sorted;
}

void introsort(node_t **list)
{
    if (!*list)
        return;

    node_t *pivot = *list;
    int value = pivot->value;
    int l = 0, r = 0;
    node_t *p = pivot->next;
    pivot->next = NULL;

    node_t *left = NULL, *right = NULL;
    while (p) {
        node_t *n = p;
        p = p->next;
        if (n->value > value) {
            list_add_node_t(&right, n);
            r++;
        }
        else {
            list_add_node_t(&left, n);
            l++;
        }
    }

    if (l < 20)
        insertsort(&left);
    else 
        introsort(&left);
    if (r < 20)
        insertsort(&right);
    else
        introsort(&right);

    node_t *result = NULL;
    list_concat(&result, left);
    list_concat(&result, pivot); 
    list_concat(&result, right);;
    *list = result;
}

void quicksort_recursion(node_t **list)
{
    if (!*list)
        return;

    node_t *pivot = *list;
    int value = pivot->value;
    node_t *p = pivot->next;
    pivot->next = NULL;

    node_t *left = NULL, *right = NULL;
    while (p) {
        node_t *n = p;
        p = p->next;
        list_add_node_t(n->value > value ? &right : &left, n);
    }

    quicksort_recursion(&left);
    quicksort_recursion(&right);

    node_t *result = NULL;
    list_concat(&result, left);
    list_concat(&result, pivot); 
    list_concat(&result, right);;
    *list = result;
}

void quicksort_norecursion(node_t **list)
{
    int value;
    int i = 0;
    int max_level = 100;
    node_t *node_beg[max_level], *node_end[max_level];
    node_t *result = NULL, *left = NULL, *right = NULL;
    node_t *pivot, *p, *L, *R;
    
    node_beg[0] = *list;
    node_end[0] = get_list_tail(list);
    
    while (i >= 0) {
        L = node_beg[i]; 
        R = node_end[i];
        if (L != R) {
            pivot = L;
            value = pivot->value;
            p = pivot->next;
            pivot->next = NULL;
            
            while (p) {
                node_t *n = p;
                p = p->next;
                list_add_node_t(n->value > value ? &right : &left, n);
            }

            node_beg[i] = left;
            node_end[i] = get_list_tail(&left);
            node_beg[i + 1] = pivot;
            node_end[i + 1] = pivot;
            node_beg[i + 2] = right;
            node_end[i + 2] = get_list_tail(&right);
            
            left = NULL;
            right = NULL;
            i += 2;
        }
        else {
            if (L) {
                list_add_node_t(&result, L);
            }
            i--;
        }
    }
    *list = result;
}

static bool list_is_ordered(node_t *list) {
    bool first = true;
    int value;
    while (list) {
        if (first) {
            value = list->value;
            first = false;
        } else {
            if (list->value < value)
                return false;
            value = list->value;
        }
        list = list->next;
    }
    return true;
}

static void list_display(node_t *list) {
    printf("%s IN ORDER : ", list_is_ordered(list) ? "   " : "NOT");
    while (list) {
        printf("%d ", list->value);
        list = list->next;
    }
    printf("\n");
}

node_t *list_make_node_t(node_t *list, int n) {
    node_t *node = malloc(sizeof(node_t));
    node->value = n;
    node->next = list;
    return node;
}

void list_free(node_t **list) {
    node_t *node = (*list)->next;
    while (*list) {
        free(*list);
        *list = node;
        if (node)
            node = node->next;
    }
}

int main(int argc, char **argv) {

    size_t times = 1000000;

    struct timespec tt1, tt2;
    time_t time1, time2;

    while (times--) {
        
        int buf;
        size_t count = 20000;
        node_t *list1 = NULL, *list2 = NULL;

        while (count--) {
            getrandom(&buf, 2, GRND_NONBLOCK);
            int value = buf % 1024;
            list1 = list_make_node_t(list1, value);
            list2 = list_make_node_t(list2, value);
        }

        clock_gettime(CLOCK_MONOTONIC, &tt1);
        quicksort_recursion(&list1);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        time1 = (tt2.tv_nsec - tt1.tv_nsec);

        clock_gettime(CLOCK_MONOTONIC, &tt1);
        quicksort_norecursion(&list2);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        time2 = (tt2.tv_nsec - tt1.tv_nsec);
        
        assert(list_is_ordered(list1));
        assert(list_is_ordered(list2));

        list_free(&list1);
        list_free(&list2);

        printf("%ld %ld\n", time1, time2);
    }
}        

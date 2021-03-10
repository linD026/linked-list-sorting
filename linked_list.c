#include <sys/random.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include "list.h"
#include "type.h"
#include "c_map.h"

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

void treesort(node_t **list) {
    node_t **record = list;
    c_map_t map = c_map_new(sizeof(int), sizeof(NULL), c_map_cmp_int);
    while (*list) {
        c_map_insert(map, *list, NULL);
        list = &(*list)->next;
    }
    node_t *node = c_map_first(map), *first = node;
    for ( ;node; node = c_map_next(node)) {
        *list = node;
        list = &(*list)->next;
    }
    *list = NULL;
    *record = first;
    free(map);
}

/* intro sort used insertion sort and tree sort to implement */
void introsort(node_t **list, int max_level, int insert)
{
    if (!*list)
        return;

    if (max_level == 0) {
        treesort(list);
        return;
    }

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
    if (l < insert)
        insertsort(&left);
    else 
        introsort(&left, max_level - 1, insert);
    if (r < insert)
        insertsort(&right);
    else
        introsort(&right, max_level - 1, insert);

    node_t *result = NULL;
    list_concat(&result, left);
    list_concat(&result, pivot); 
    list_concat(&result, right);;
    *list = result;
}

/* quick sort with recursion version */
void quicksort_recursion(node_t **list)
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
        quicksort_recursion(&left);
    if (r < 20)
        insertsort(&right);
    else
        quicksort_recursion(&right);

    node_t *result = NULL;
    list_concat(&result, left);
    list_concat(&result, pivot); 
    list_concat(&result, right);;
    *list = result;
}

/* quick sort with no recursion version */
void quicksort_norecursion(node_t **list)
{
    int n = get_list_length(list);
    int value;
    int i = 0;
    int max_level = 2 * n;
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

/* Verify if list is order */
static bool list_is_ordered(node_t *list) {
    bool first = true;
    int value;
    while (list) {
        if (first) {
            value = list->value;
            first = false;
        } else {
            if (list->value < value) {
                return false;
            }
            value = list->value;
        }
        list = list->next;
    }
    return true;
}

/* Display list */
static void list_display(node_t *list) {
    printf("%s IN ORDER : ", list_is_ordered(list) ? "   " : "NOT");
    while (list) {
        printf("%d ", list->value);
        list = list->next;
    }
    printf("\n");
}

/* Calculate difference of time */
static time_t diff_in_ns(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000000.0 + diff.tv_nsec);
}

/* shuffle array, only work if n < RAND_MAX */
void shuffle(int *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

int main(int argc, char **argv) {

    size_t times = 1000;

    struct timespec tt1, tt2;
    time_t time = 0;

    int max_level = 32, insert = 21;
    node_t *list1 = NULL, *list2 = NULL, *list3 = NULL, *list4 = NULL;

    while (times--) {
        
        size_t count = 100000;
        node_t *list1 = NULL, *list2 = NULL, *list3 = NULL;

        int *test_arr = malloc(sizeof(int) * count);

        for (int i = 0; i < count; ++i) {
            test_arr[i] = i;
        }
        shuffle(test_arr, count);

        while (count--) {
            list1 = list_make_node_t(list1, test_arr[count]);
            list2 = list_make_node_t(list2, test_arr[count]);
            list3 = list_make_node_t(list3, test_arr[count]);
            list4 = list_make_node_t(list4, test_arr[count]);
        }

        clock_gettime(CLOCK_MONOTONIC, &tt1);
        introsort(&list1, max_level, insert);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        time = diff_in_ns(tt1, tt2);
        printf("%ld ", time);
        
        clock_gettime(CLOCK_MONOTONIC, &tt1);
        treesort(&list2);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        time = diff_in_ns(tt1, tt2);
        printf("%ld ", time);
        
        clock_gettime(CLOCK_MONOTONIC, &tt1);
        quicksort_recursion(&list3);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        time = diff_in_ns(tt1, tt2);
        printf("%ld ", time);
        
        clock_gettime(CLOCK_MONOTONIC, &tt1);
        quicksort_norecursion(&list4);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        time = diff_in_ns(tt1, tt2);
        printf("%ld\n", time);

        assert(list_is_ordered(list1));
        assert(list_is_ordered(list2));
        assert(list_is_ordered(list3));
        assert(list_is_ordered(list4));
        
        list_free(&list1);
        list_free(&list2);
        list_free(&list3);
        list_free(&list4);
        
        free(test_arr);
    }
}        

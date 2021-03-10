#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "c_map.h"

static node_t *c_map_create_node(node_t *node)
{
    /* Setup the pointers */
    node->left = node->right = node->up = NULL;

    /* Set the color to black by default */
    node->color = C_MAP_RED;

    return NULL;
}


/*
 * Perform left rotation with "node". The following happens (with respect
 * to "C"):
 *
 *         B                C
 *        / \              / \
 *       A   C     =>     B   D
 *            \          /
 *             D        A
 *
 * Returns the new node pointing in the spot of the original node.
 */
static node_t *c_map_rotate_left(c_map_t obj, node_t *node)
{
    node_t *r = node->right, *rl = r->left, *up = node->up;

    /* Adjust */
    r->up = up;
    r->left = node;

    node->right = rl;
    node->up = r;

    if (node->right)
        node->right->up = node;

    if (up) {
        if (up->right == node)
            up->right = r;
        else
            up->left = r;
    }

    if (node == obj->head)
        obj->head = r;

    return r;
}

/*
 * Perform a right rotation with "node". The following happens (with respect
 * to "C"):
 *
 *         C                B
 *        / \              / \
 *       B   D     =>     A   C
 *      /                      \
 *     A                        D
 *
 * Return the new node pointing in the spot of the original node.
 */
static node_t *c_map_rotate_right(c_map_t obj, node_t *node)
{
    node_t *l = node->left, *lr = l->right, *up = node->up;

    // Adjust
    l->up = up;
    l->right = node;

    node->left = lr;
    node->up = l;

    if (node->left)
        node->left->up = node;

    if (up) {
        if (up->right == node)
            up->right = l;
        else
            up->left = l;
    }

    if (node == obj->head)
        obj->head = l;

    return l;
}

static void c_map_l_l(c_map_t obj,
                      node_t *node UNUSED,
                      node_t *parent UNUSED,
                      node_t *grandparent,
                      node_t *uncle UNUSED)
{
    /* Rotate to the right according to grandparent */
    grandparent = c_map_rotate_right(obj, grandparent);

    /* Swap grandparent and uncle's colors */
    c_map_color_t c1 = grandparent->color, c2 = grandparent->right->color;

    grandparent->color = c2;
    grandparent->right->color = c1;
}

static void c_map_l_r(c_map_t obj,
                      node_t *node,
                      node_t *parent,
                      node_t *grandparent,
                      node_t *uncle)
{
    /* Rotate to the left according to parent */
    parent = c_map_rotate_left(obj, parent);

    /* Refigure out the identity */
    node = parent->left;
    grandparent = parent->up;
    uncle =
        (grandparent->left == parent) ? grandparent->right : grandparent->left;

    // Apply left-left case
    c_map_l_l(obj, node, parent, grandparent, uncle);
}

static void c_map_r_r(c_map_t obj,
                      node_t *node UNUSED,
                      node_t *parent UNUSED,
                      node_t *grandparent,
                      node_t *uncle UNUSED)
{
    /* Rotate to the left according to grandparent */
    grandparent = c_map_rotate_left(obj, grandparent);

    /* Swap grandparent and uncle's colors */
    c_map_color_t c1 = grandparent->color, c2 = grandparent->left->color;

    grandparent->color = c2;
    grandparent->left->color = c1;
}

static void c_map_r_l(c_map_t obj,
                      node_t *node,
                      node_t *parent,
                      node_t *grandparent,
                      node_t *uncle)
{
    /* Rotate to the right according to parent */
    parent = c_map_rotate_right(obj, parent);

    /* Refigure out the identity */
    node = parent->right;
    grandparent = parent->up;
    uncle =
        (grandparent->left == parent) ? grandparent->right : grandparent->left;

    /* Apply right-right case */
    c_map_r_r(obj, node, parent, grandparent, uncle);
}

static void c_map_fix_colors(c_map_t obj, node_t *node)
{
    /* If root, set the color to black */
    if (node == obj->head) {
        node->color = C_MAP_BLACK;
        return;
    }

    /* If node's parent is black or node is root, back out. */
    if (node->up->color == C_MAP_BLACK && node->up != obj->head)
        return;

    /* Find out the identity */
    node_t *parent = node->up, *grandparent = parent->up, *uncle;

    if (!parent->up)
        return;

    /* Find out the uncle */
    if (grandparent->left == parent)
        uncle = grandparent->right;
    else
        uncle = grandparent->left;

    if (uncle && uncle->color == C_MAP_RED) {
        /* If the uncle is red, change color of parent and uncle to black */
        uncle->color = C_MAP_BLACK;
        parent->color = C_MAP_BLACK;

        /* Change color of grandparent to red. */
        grandparent->color = C_MAP_RED;

        /* Call this on the grandparent */
        c_map_fix_colors(obj, grandparent);
    } else if (!uncle || uncle->color == C_MAP_BLACK) {
        /* If the uncle is black. */
        if (parent == grandparent->left && node == parent->left)
            c_map_l_l(obj, node, parent, grandparent, uncle);
        else if (parent == grandparent->left && node == parent->right)
            c_map_l_r(obj, node, parent, grandparent, uncle);
        else if (parent == grandparent->right && node == parent->left)
            c_map_r_l(obj, node, parent, grandparent, uncle);
        else if (parent == grandparent->right && node == parent->right)
            c_map_r_r(obj, node, parent, grandparent, uncle);
    }
}

/*
 * Fix the red-black tree post-BST deletion. This may involve multiple
 * recolors and/or rotations depending on which node was deleted, what color
 * it was, and where it was in the tree at the time of deletion.
 *
 * These fixes occur up and down the path of the tree, and each rotation is
 * guaranteed constant time. As such, there is a maximum of O(lg n) operations
 * taking place during the fixup procedure.
 */
static void c_map_delete_fixup(c_map_t obj,
                               node_t *node,
                               node_t *p,
                               bool y_is_left,
                               node_t *y UNUSED)
{
    node_t *w;
    c_map_color_t lc, rc;

    if (!node)
        return;

    while (node != obj->head && node->color == C_MAP_BLACK) {
        if (y_is_left) { /* if left child */
            w = p->right;

            if (w->color == C_MAP_RED) {
                w->color = C_MAP_BLACK;
                p->color = C_MAP_RED;
                p = c_map_rotate_left(obj, p)->left;
                w = p->right;
            }

            lc = !w->left ? C_MAP_BLACK : w->left->color;
            rc = !w->right ? C_MAP_BLACK : w->right->color;

            if (lc == C_MAP_BLACK && rc == C_MAP_BLACK) {
                w->color = C_MAP_RED;
                node = node->up;
                p = node->up;

                if (p)
                    y_is_left = (node == p->left);
            } else {
                if (rc == C_MAP_BLACK) {
                    w->left->color = C_MAP_BLACK;
                    w->color = C_MAP_RED;
                    w = c_map_rotate_right(obj, w);
                    w = p->right;
                }

                w->color = p->color;
                p->color = C_MAP_BLACK;

                if (w->right)
                    w->right->color = C_MAP_BLACK;

                p = c_map_rotate_left(obj, p);
                node = obj->head;
                p = NULL;
            }
        } else {
            /* Same except flipped "left" and "right" */
            w = p->left;

            if (w->color == C_MAP_RED) {
                w->color = C_MAP_BLACK;
                p->color = C_MAP_RED;
                p = c_map_rotate_right(obj, p)->right;
                w = p->left;
            }

            lc = !w->left ? C_MAP_BLACK : w->left->color;
            rc = !w->right ? C_MAP_BLACK : w->right->color;

            if (lc == C_MAP_BLACK && rc == C_MAP_BLACK) {
                w->color = C_MAP_RED;
                node = node->up;
                p = node->up;
                if (p)
                    y_is_left = (node == p->left);
            } else {
                if (lc == C_MAP_BLACK) {
                    w->right->color = C_MAP_BLACK;
                    w->color = C_MAP_RED;
                    w = c_map_rotate_left(obj, w);
                    w = p->left;
                }

                w->color = p->color;
                p->color = C_MAP_BLACK;

                if (w->left)
                    w->left->color = C_MAP_BLACK;

                p = c_map_rotate_right(obj, p);
                node = obj->head;
                p = NULL;
            }
        }
    }

    node->color = C_MAP_BLACK;
}

/*
 * Recalculate the positions of the "least" and "most" iterators in the
 * tree. This is so iterators know where the beginning and end of the tree
 * resides.
 */
static void c_map_calibrate(c_map_t obj)
{
    if (!obj->head) {
        obj->it_least.node = obj->it_most.node = NULL;
        return;
    }

    // Recompute it_least and it_most
    obj->it_least.node = obj->it_most.node = obj->head;

    while (obj->it_least.node->left)
        obj->it_least.node = obj->it_least.node->left;

    while (obj->it_most.node->right)
        obj->it_most.node = obj->it_most.node->right;
}

/*
 * Sets up a brand new, blank c_map for use. The size of the node elements
 * is determined by what types are thrown in. "s1" is the size of the key
 * elements in bytes, while "s2" is the size of the value elements in
 * bytes.
 *
 * Since this is also a tree data structure, a comparison function is also
 * required to be passed in. A destruct function is optional and must be
 * added in through another function.
 */
c_map_t c_map_new(size_t s1, size_t s2, int (*cmp)(void *, void *))
{
    c_map_t obj = malloc(sizeof(struct c_map_internal));

    // Set all pointers to NULL
    obj->head = NULL;

    // Set up all default properties
    obj->key_size = s1;
    obj->element_size = s2;
    obj->size = 0;

    // Function pointers
    obj->comparator = cmp;

    obj->it_end.prev = obj->it_end.node = NULL;
    obj->it_least.prev = obj->it_least.node = NULL;
    obj->it_most.prev = obj->it_most.node = NULL;
    obj->it_most.node = NULL;

    return obj;
}

/*
 * Insert a key/value pair into the c_map. The value can be blank. If so,
 * it is filled with 0's, as defined in "c_map_create_node".
 */
bool c_map_insert(c_map_t obj, node_t *node, void *value)
{
    /* Copy the key and value into new node and prepare it to put into tree. */
    //c_map_node_t *new_node =
    //    c_map_create_node(&node->value, value, obj->key_size, obj->element_size);
    
    c_map_create_node(node);
    
    //node->rb_node = *new_node;

    obj->size++;

    if (!obj->head) {
        /* Just insert the node in as the new head. */
        obj->head = node;
        obj->head->color = C_MAP_BLACK;

        /* Calibrate the tree to properly assign pointers. */
        c_map_calibrate(obj);
        return true;
    }

    /* Traverse the tree until we hit the end or find a side that is NULL */
    node_t *cur = obj->head;

    while (1) {
        int res = obj->comparator(&node->value, &cur->value);
        if (res == 0) { /* If the key matches something else, don't insert */
            assert(0 && "not support repetitive value");
        }

        if (res < 0) {
            if (!cur->left) {
                cur->left = node;
                node->up = cur;
                c_map_fix_colors(obj, node);
                break;
            }
            cur = cur->left;
        } else {
            if (!cur->right) {
                cur->right = node;
                node->up = cur;
                c_map_fix_colors(obj, node);
                break;
            }
            cur = cur->right;
        }
    }

    c_map_calibrate(obj);
    return true;
}

node_t *c_map_first(c_map_t obj)
{
	node_t *n;

	n = obj->head;
	if (!n)
		return NULL;
	while (n->left)
		n = n->left;
	return n;
}

node_t *c_map_next(node_t *node)
{
	node_t *parent;

	if (!node)
		return NULL;

	/*
	 * If we have a right-hand child, go down and then left as far
	 * as we can.
	 */
	if (node->right) {
		node = node->right;
		while (node->left)
			node = node->left;
		return node;
	}

	/*
	 * No right-hand children. Everything down and left is smaller than us,
	 * so any 'next' node must be in the general direction of our parent.
	 * Go up the tree; any time the ancestor is a right-hand child of its
	 * parent, keep going up. First time it's a left-hand child of its
	 * parent, said parent is our 'next' node.
	 */
	while ((parent = node->up) && node == parent->right)
		node = parent;

	return parent;
}

/* Free the c_map from memory and delete all nodes. */
void c_map_delete(c_map_t obj)
{
    /* Free the map itself */
    free(obj);
}
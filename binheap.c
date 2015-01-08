/*
 * binheap.c
 *
 *  Created on: 2015-1-6
 *      Author: dpwoo
 */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include "binheap.h"

struct binheap {
	int nodes;
	struct binheap_node *head;
};

struct binheap*
binheap_create()
{
	struct binheap *h;
	if((h = calloc(1, sizeof(*h))) == NULL) {
		fprintf(stderr, "out of memory!\n");
		return NULL;
	}
	return h;
}

int
binheap_destroy(struct binheap *heap)
{
	free(heap);
	return 0;
}

static int
binheap_link(struct binheap_node *parent, struct binheap_node *child)
{
	if(!(parent && child
		&& parent->degree == child->degree
		&& parent->key <= child->key)) {
		fprintf(stderr, "invalid paramter!\n");
		return -1;
	}

	child->parent = parent;
	child->sibling = parent->child;
	parent->child = child;
	parent->degree++;
	return 0;
}

static struct binheap_node*
binheap_merge(struct binheap_node *h1, struct binheap_node *h2)
{
	struct binheap_node *tmp;
	struct binheap_node *heap = NULL, **tail = &heap;

	while(h1 && h2) {
		if(h1->degree <= h2->degree) {
			tmp = h1;
			h1 = h1->sibling;
		} else {
			tmp = h2;
			h2 = h2->sibling;
		}
		*tail = tmp;
		tail = &(*tail)->sibling;
	}

	if(h1) {
		*tail = h1;
	} else {
		*tail = h2;
	}

	return heap;
}

static struct binheap_node*
inner_heap_union(struct binheap_node *r1, struct binheap_node *r2)
{
	struct binheap_node *root, *prev = NULL, *x, *next;

	if(!r1 || !r2) {
		return r1 ? r1 : r2;
	}

	root = binheap_merge(r1, r2);

	x = root;
	for(next = x->sibling; next; next = x->sibling) {
		if(x->degree != next->degree
				|| (next->sibling && x->degree == next->sibling->degree)) { // case 1, case 2
			prev = x;
			x = next;
		} else if(x->key <= next->key) { //case 4
			x->sibling = next->sibling;
			binheap_link(x, next);
		} else { // case 3
			if(!prev) {
				root = next;
			} else {
				prev->sibling = next;
			}
			binheap_link(next, x);
			x = next;
		}
	}

	return root;
}

struct binheap*
binheap_union(struct binheap *h1, struct binheap *h2)
{
	if(!h1 || !h2) {
		fprintf(stderr, "[%s]invalid param!\n", __func__);
		return NULL;
	}

	h1->head = inner_heap_union(h1->head, h2->head);
	binheap_destroy(h2);

	return h1;
}

static struct binheap_node**
binbeap_find_min_root_before(struct binheap_node **root)
{
	struct binheap_node **r, **min = root;
	for(r = root; *r; r = &(*r)->sibling) {
		if((*min)->key > (*r)->key) {
			min = r;
		}
	}
	return min;
}

struct binheap_node*
binheap_min(struct binheap *heap)
{
	if(!heap || !heap->head) {
		fprintf(stderr, "empty heap!\n");
		return NULL;
	}

	struct binheap_node **min;
	min = binbeap_find_min_root_before(&heap->head);
	return *min;
}

struct binheap_node*
binheap_extract_min(struct binheap *heap)
{
	if(!heap || !heap->head) {
		fprintf(stderr, "empty heap!\n");
		return NULL;
	}

	// select min root and cut off from the root list
	struct binheap_node **min, *me;
	min = binbeap_find_min_root_before(&heap->head);
	me = *min;
	*min = (*min)->sibling;

	//generate new binomial heap(degree by ascending order)
	struct binheap_node *tmp, *r = NULL, *r1 = me->child;
	while(r1) {
		tmp = r1;
		r1 = r1->sibling;
		tmp->parent = NULL;
		tmp->sibling = r;
		r = tmp;
	}

	heap->nodes--;
	heap->head = inner_heap_union(heap->head, r);

	return me;
}

struct binheap_node*
binheap_insert(struct binheap *heap, int key)
{
	if(!heap)  {
		fprintf(stderr, "invalid param!\n");
		return NULL;
	}

	struct binheap_node *me;
	me = calloc(1, sizeof(*me));
	if(!me) {
		fprintf(stderr, "out of memory!\n");
		return NULL;
	}
	me->key = key;

	heap->nodes++;
	heap->head = inner_heap_union(heap->head, me);

	return me;
}

struct binheap_node*
binheap_decrease_key(struct binheap *heap, struct binheap_node *key, int new_key_value)
{
	if(!heap || !heap->head || !key || key->key < new_key_value) {
		fprintf(stderr, "invalid param!\n");
		return NULL;
	}

	key->key = new_key_value;
	for(; key->parent && key->key < key->parent->key; key = key->parent) {
			int tmp = key->key;
			key->key = key->parent->key;
			key->parent->key = tmp;
	}

	return key;
}

struct binheap_node*
binheap_delete(struct binheap *heap, struct binheap_node *key)
{
	if(!heap || !heap->head || !key) {
		fprintf(stderr, "invalid param!\n");
		return NULL;
	}

	struct binheap_node *r;
	for(r = key; r->parent; r = r->parent) {
		;//do nothing
	}
	int root_key = r->key;
	key = binheap_decrease_key(heap, key, INT_MIN);
	if(key != r) {
		assert(root_key == INT_MIN);
	}

	return binheap_extract_min(heap);
}

static struct binheap_node*
inner_binheap_search(struct binheap_node *root, int key)
{
	struct binheap_node *found = NULL;

	for(; root; root = root->sibling){
		if(root->key == key) {
			return root;
		}
		if(root->child) {
			found = inner_binheap_search(root->child, key);
			if(found) {
				return found;
			}
		}
	}

	return NULL;
}

struct binheap_node*
binheap_search(struct binheap *heap, int key)
{
	if(!heap || !heap->head) return NULL;

	return inner_binheap_search(heap->head, key);
}

static void
inner_binheap_dump(struct binheap_node *root)
{
	for(; root; root = root->sibling){
		if(root->parent == NULL) {
			fprintf(stderr, "[[");
		}
		fprintf(stderr, "%d:%d ", root->key, root->degree);
		if(root->child) {
			inner_binheap_dump(root->child);
		}
		if(root->parent == NULL) {
			fprintf(stderr, "]]");
		}
	}
}

void
binheap_dump(struct binheap *heap)
{
	if(!heap || !heap->head) return ;
	inner_binheap_dump(heap->head);
	fprintf(stderr, "\n");
	fflush(stderr);
}
/* end of file */


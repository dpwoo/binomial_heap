/*
 * binheap.h
 *
 *  Created on: 2015-1-6
 *      Author: dpwoo
 */

#ifndef BINHEAP_H_
#define BINHEAP_H_

#ifdef __cplusplus
extern "C" {
#endif

struct binheap_node {
	int key;
	int degree;
	struct binheap_node *parent;
	struct binheap_node *child, *sibling;
};

struct binheap;

struct binheap*
binheap_create();

int
binheap_destroy(struct binheap *heap);

struct binheap_node*
binheap_min(struct binheap *heap);

struct binheap_node*
binheap_extract_min(struct binheap *heap);

struct binheap_node*
binheap_insert(struct binheap *heap, int key);

struct binheap_node*
binheap_decrease_key(struct binheap *heap, struct binheap_node *key, int new_key_value);

struct binheap_node*
binheap_delete(struct binheap *heap, struct binheap_node *key);

struct binheap*
binheap_union(struct binheap *h1, struct binheap *h2);

//for test
struct binheap_node*
binheap_search(struct binheap *heap, int key);

void
binheap_dump(struct binheap *heap);

#ifdef __cplusplus
extern "C" }
#endif

#endif /* BINHEAP_H_ */

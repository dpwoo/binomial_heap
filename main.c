/*
 * main.c
 *
 *  Created on: 2015-1-7
 *      Author: dpwoo
 */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <signal.h>
#include "binheap.h"

static void
print_and_delete_node(struct binheap_node *node)
{
	if(node) {
		printf("%d\t", node->key);
		free(node);
	}
}

static int
binheap_test()
{
	int array[] = {
		10, 1, 12, 18, 25, 6, 8, 11, 17, 27, 14, 38, 29,
	};

	struct binheap *heap;

	heap = binheap_create();

	int i;
	for(i = 0; i < sizeof(array) / sizeof(array[0]); i++) {
		binheap_insert(heap, array[i]);
		binheap_dump(heap);
	}

	int del_key = 17;
	struct binheap_node *node;
	node = binheap_search(heap, del_key);
	if(!node || node->key != del_key) {
		fprintf(stderr, "Error, not found key %d\n", del_key);
	}

	if(node && node->key == del_key) {
		node = binheap_delete(heap, node);
		free(node);
	}

	node = binheap_search(heap, del_key);
	if(node) {
		fprintf(stderr, "Error, still find deleted key %d\n", del_key);
	}

	fprintf(stderr, "\n");
	binheap_dump(heap);

	// inset angain for min op bellow
	binheap_insert(heap, del_key);
	node = binheap_search(heap, del_key);
	if(!node || node->key != del_key) {
		fprintf(stderr, "Error, not found key %d\n", del_key);
	}

	binheap_dump(heap);
	fprintf(stderr, "\n");

	for(i = 0; i < sizeof(array) / sizeof(array[0]); i++) {
		node = binheap_min(heap);
		int min_key = node->key;
		node = binheap_extract_min(heap);
		if(min_key != node->key) {
			fprintf(stderr, "Error, keys not equal[%d, %d]!\n", min_key, node->key);
		}
		print_and_delete_node(node);

		binheap_dump(heap);
	}

	fflush(stdout);
	binheap_destroy(heap);

	return 0;
}

static void
signal_handle(int signo)
{
	if(signo == SIGSEGV) {
		fprintf(stderr, "signal SIGSEGV catch\n");
		exit(0);
	}
}

int main(int argc, char *argv[])
{
	signal(SIGSEGV, signal_handle);
	binheap_test();
	fprintf(stderr, "\neverything go well!\n");
	return 0;
}

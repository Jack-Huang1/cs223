#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include "/c/cs223/hw6/dict.h"
#include "/c/cs223/hw6/heap.h"

bool empty(Heap h)
{
	return h->n == 0;
}

void demand(int cond, char * msg)
{
	if (!cond)
	{
		fprintf(stderr, "%s\n", msg);
		exit(1);
	}
	return;
}

void printHeap(Heap h)
{
	for (int i = 1; i <= h->n; i++)
		printf("[%d] %s key: %d ",
		 i, h->heap[i]->value->room, h->heap[i]->key);
	printf("\n");
}

struct room * findmin(Heap h)
{
	if (empty(h))
	{
		fprintf(stderr, "findmin not allowed on empty heap.\n");
		destroyHeap(h);
		exit(1);
	}
	return h->heap[1]->value;
}

Heap initheap()
{
	Heap newheap = malloc(MAXHEAP * sizeof(struct heap));
	newheap->n = 0;
	newheap->size = MAXHEAP;
	newheap->heap = malloc(sizeof(struct heapslot) * MAXHEAP);
	newheap->heap[0] = malloc(sizeof(struct heapslot));
	newheap->heap[0]->key = INT_MIN;
	newheap->heap[0]->value = NULL;
	return newheap;
}

void swap(heapslot *s1, heapslot *s2)
{
	heapslot temp = *s1;
	*s1 = *s2;
	*s2 = temp;
}

void insert(Heap h, int key, struct room * r)
{
	while (h->n >= h->size)
	{
		h->size = h->size * 2;
		h->heap = realloc(h->heap, sizeof(struct heapslot) * (h->size));
	}

	int cur, parent;

	cur = ++h->n;
	h->heap[cur] = malloc(sizeof(struct heapslot));
	h->heap[cur]->key = key;
	h->heap[cur]->value = r;
	parent = cur/2;
	while (h->heap[parent]->key > h->heap[cur]->key)
	{
		demand(parent > 0, "inserted item rising past root\n");
		swap(h->heap[parent], h->heap[cur]);
	    cur = parent;
	    parent = cur/2;
	}
}

void deletemin(Heap h)
{
	int cur, child;
	demand(! empty(h), "deletemin not allowed on empty heap\n");
	free(h->heap[1]);
	h->heap[1] = h->heap[(h->n)--];
	cur = 1;
	child = 2;
	while (child <= h->n)
	{
		if (child < h->n && h->heap[child+1]->key < h->heap[child]->key)
			child++;
		if (h->heap[cur]->key > h->heap[child]->key)
		{
			demand(child <= h->n, "falling past leaves\n");
			swap(h->heap[cur], h->heap[child]);
			cur = child;
			child = 2*cur;
		} else
			break;
	}
}

void destroyHeap(Heap h)
{
	for (int i = 0; i <= h->n; i++)
	{
		free(h->heap[i]->value);
		free(h->heap[i]);
	}

	free(h->heap);
	free(h);
}

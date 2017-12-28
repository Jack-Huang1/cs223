#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "/c/cs223/hw3/hash.h"

struct elt {
  struct elt *next;
  char *key;
};


struct hash {
  int size;           // size of the pointer table 
  int n;              // number of elements stored 
  int buckets;
  struct elt **table;
};

static unsigned long hashf(char * key) // used the djb2 hash function
{
  long hash = 5381;
  int c;
  while ((c = *key++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

char *strdup(const char *s) // makes a duplicate to working with const params
{
	char *d = malloc((strlen(s)) + 1);
    if (d == NULL) return NULL;
    strcpy(d,s);
    return d;
}

Hash internalHashCreate(int size) // creates a new hashtable
{
	Hash myhash;
	myhash = malloc(sizeof(*myhash));
	assert(myhash != 0);
	myhash->size = size;
	myhash->n = 0;
	myhash->buckets = 0;
	myhash->table = malloc(sizeof(struct elt *) * size);
	assert(myhash->table);
	for (int i = 0; i < size; i++)
		myhash->table[i] = NULL;
	return myhash;
}

Hash HashCreate(void) // create an empty hashtable
{
	return internalHashCreate(INITIAL_SIZE);
}

void HashDestroy(Hash myhash) // destroy a hash table
{
	struct elt *x;
	struct elt *next;
	for (int i = 0; i < myhash->size; i++)
	{
		for (x = myhash->table[i]; x != NULL; x = next)
		{
			next = x->next;
			free(x->key);
			free(x); 
		}
	}
	free(myhash->table);
	free(myhash);
}

static void HashGrow(Hash myhash) // makes a new table 2x the size of the input
{
	Hash newhash;
	struct hash nhash;
	struct elt *x;
	newhash = internalHashCreate(myhash->size * GROWTH_FACTOR);
	for (int i = 0; i < myhash->size; i++)
	{
		for (x = myhash->table[i]; x != 0; x = x->next)
			HashInsert(newhash, x->key);
	}
	// the next three lines switches the old hashtable with our new one
	nhash = *myhash;
	*myhash = *newhash;
	*newhash = nhash;
	HashDestroy(newhash); // destroys old hashtable
	if (debugflag) // runs debug
	{
		double occupancy = 1.0 * (myhash->buckets)/(myhash->n);
		printf(
		"Growing to size: %d. n: %d. Used buckets: %d. Occupancy rate: %.02f\n", 
		myhash->size, myhash->n, myhash->buckets, occupancy);
	}
}

void HashInsert(Hash myhash, const char *key) // insert a new key into hashtable 
{
	assert(key);
	struct elt *temp = malloc(sizeof(* temp));
	assert(temp);
	temp->key = strdup(key);
	long index = hashf(temp->key) % myhash->size;
	if (myhash->table[index] == NULL)
		myhash->buckets++;
	temp->next = myhash->table[index];
	myhash->table[index] = temp;
	myhash->n++;
	if (myhash->n >= myhash->size * MAX_LOAD_FACTOR)
		HashGrow(myhash);
}

bool HashSearch(Hash myhash, const char *key) // checks if the key is in table
{
	struct elt *x;
	char *akey = strdup(key);
	for (x = myhash->table[hashf(akey) % myhash->size]; x != 0; x = x->next)
	{
		if (!strcmp(x->key, akey))
		{
			free(akey);
			return true;
		}
	}
	free(akey);
	return false;
}

void HashDisplay(Hash myhash) // print out a hashtable
{
	for (int i = 0; i < myhash->size; i++)
	{
		if (myhash->table[i] == NULL)
			continue;
		printf("[%d]", i);
		for (struct elt *x = myhash->table[i]; x != NULL; x = x->next)
			printf(" %s", x->key);
		printf("\n");
	}
}

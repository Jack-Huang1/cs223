#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include "AVLtree.h"

char *strdup2 (const char *s) 
{
    char *d = malloc (strlen (s) + 1);   // Space for length plus nul
    if (d == NULL) return NULL;          // No memory
    strcpy (d,s);                        // Copy the characters
    return d;                            // Return the new string
}

int
treeHeightAVL(const struct tree *root)
{
    if(root == 0) {
        return TREE_EMPTY_HEIGHT;
    } else {
        return root->height;
    }
}

/* recompute height from height of kids */
static int
treeComputeHeight(const struct tree *root)
{
    int childHeight;
    int maxChildHeight;
    int i;

    if(root == 0) {
        return TREE_EMPTY_HEIGHT;
    } else {
        maxChildHeight = TREE_EMPTY_HEIGHT;

        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            childHeight = treeHeightAVL(root->child[i]);
            if(childHeight > maxChildHeight) {
                maxChildHeight = childHeight;
            }
        }

        return maxChildHeight + 1;
    }
}

size_t
treeSizeAVL(const struct tree *root)
{
    if(root == 0) {
        return 0;
    } else {
        return root->size;
    }
} 

/* recompute size from size of kids */
static int
treeComputeSize(const struct tree *root)
{
    int size;
    int i;

    if(root == 0) {
        return 0;
    } else {
        size = 1;

        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            size += treeSizeAVL(root->child[i]);
        }

        return size;
    }
}

/* fix aggregate data in root */
/* assumes children are correct */
static void
treeFix(struct tree *root)
{
    if(root) {
        root->height = treeComputeHeight(root);
        root->size = treeComputeSize(root);
    }
}

/* rotate child in given direction to root */
static void
treeRotate(struct tree **root, int direction)
{
    struct tree *x;
    struct tree *y;
    struct tree *b;

    /*
     *      y           x 
     *     / \         / \
     *    x   C  <=>  A   y
     *   / \             / \
     *  A   B           B   C
     */

    y = *root;                  assert(y);
    x = y->child[direction];    assert(x);
    b = x->child[!direction];

    /* do the rotation */
    *root = x;
    x->child[!direction] = y;
    y->child[direction] = b;

    /* fix y then x */
    treeFix(y);
    treeFix(x);
}

/* restore AVL property at *root after an insertion or deletion */
static void
treeRebalance(struct tree **root)
{
    int tallerKid;

    if(*root) {
        for(tallerKid = 0; tallerKid < TREE_NUM_CHILDREN; tallerKid++) {
            if(treeHeightAVL((*root)->child[tallerKid]) >= 
            treeHeightAVL((*root)->child[!tallerKid]) + 2) {

                /*check if zig-zag: opposite-direction nephew is the tall one */
                /* this also covers case where both nephews are too tall */
                if(treeHeightAVL((*root)->child[tallerKid]->child[!tallerKid]) 
                            >= treeHeightAVL((*root)->child[tallerKid]) - 1) {
                    /* zig zag case */
                    treeRotate(&(*root)->child[tallerKid], !tallerKid);
                }

                /* fall through to zig zig case */
                treeRotate(root, tallerKid);

                /* don't bother with other kid */
                break;
            }
        }
    }
}


/* free all elements of a tree, replacing it with TREE_EMPTY */
void 
treeDestroyAVL(struct tree **root)
{
    int i;

    if(*root) {
        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            treeDestroyAVL(&(*root)->child[i]);
        }
        free((*root)->key);
        free(*root);
    }
}



/* insert an element into a tree pointed to by root */
void
treeInsertAVL(struct tree **root, char * newElement)
{

    if(*root == 0) {
        /* not already there, put it in */
        struct tree *e = malloc(sizeof(*e));
        assert(e);
        e->key = strdup2(newElement);
        e->child[LEFT] = e->child[RIGHT] = 0;
        e->next = NULL;
        e->count = 1;

        *root = e;
    } else {
        /* do this recursively so we can fix data on the way back out */
        treeInsertAVL(&(*root)->child[strcmp((*root)->key, newElement) < 0],
         newElement);
    }

    /* fix the aggregate data */
    treeFix(*root);
    treeRebalance(root);
}

struct tree *
treeContains(struct tree *t, char * target)
{
    while(t && strcmp(t->key, target)) {
        t = t->child[strcmp(t->key,target) < 0];
    }

    return t;
}

/* delete minimum element from the tree and return its key */
/* do not call this on an empty tree */
char *
treeDeleteMin(struct tree **root)
{
    struct tree *toFree;
    char * retval;

    assert(*root);  /* can't delete min from empty tree */

    if((*root)->child[LEFT]) {
        /* recurse on left subtree */
      printf("DELETE: left child\n");
        retval = treeDeleteMin(&(*root)->child[LEFT]);
    } else {
        /* delete the root */
      printf("DELETE: root\n");
        toFree = *root;
        retval = strdup2(toFree->key);
        *root = toFree->child[RIGHT];
        free(toFree->key);
        free(toFree);
    }

    /* fix the aggregate data */
    treeFix(*root);
    treeRebalance(root);

    return retval;
}

/* delete target from the tree */
/* has no effect if target is not in tree */
void 
treeDelete(struct tree **root, char * target)
{
    struct tree *toFree;

    /* do nothing if target not in tree */
    if(*root) {
        if(!strcmp((*root)->key, target)) {
            if((*root)->child[RIGHT]) {
                /* replace root with min value in right subtree */
                (*root)->key = strdup2(treeDeleteMin(&(*root)->child[RIGHT]));
            } else {
                /* patch out root */
                toFree = *root;
                *root = toFree->child[LEFT];
                free(toFree->key);
                free(toFree);
            }
        } else {
            treeDelete(&(*root)->child[(*root)->key < target], target);
        }

        /* fix the aggregate data */
        treeFix(*root);
        treeRebalance(root);
    }
}


/* how far to indent each level of the tree */
#define INDENTATION_LEVEL (2)

/* print contents of a tree, indented by depth */
static void
treePrintIndented(const struct tree *root, int depth)
{
    int i;

    if(root != 0) {
        treePrintIndented(root->child[LEFT], depth+1);

        for(i = 0; i < INDENTATION_LEVEL*depth; i++) {
            putchar(' ');
        }
        printf("%s Height: %d Size: %lu Count: %d (%p)\n", root->key,
         root->height, root->size, root->count, (void *) root);

        treePrintIndented(root->child[RIGHT], depth+1);
    }
}

void printTreePreAVL(const struct tree * tree)
{
    if (tree == 0)
        return;
    printf("%s [%d / %d] ", tree->key, tree->count, treeHeightAVL(tree));
    printTreePreAVL(tree->child[LEFT]);
    printTreePreAVL(tree->child[RIGHT]);
}
void printTreeInAVL(const struct tree * tree)
{
    if (tree == 0)
        return;
    printTreeInAVL(tree->child[LEFT]);
    printf("%s [%d / %d] ", tree->key, tree->count, treeHeightAVL(tree));
    printTreeInAVL(tree->child[RIGHT]);
}
void printTreePostAVL(const struct tree * tree)
{
    if (tree == 0)
        return;
    printTreePostAVL(tree->child[LEFT]);
    printTreePostAVL(tree->child[RIGHT]);
    printf("%s [%d / %d] ", tree->key, tree->count, treeHeightAVL(tree));
}

/* print the contents of a tree */
void 
treePrint(const struct tree *root, int which)
{
    if (which == 0)
        treePrintIndented(root, 0);
    if (which == 1)
        printTreePreAVL(root);
    if (which == 2)
        printTreeInAVL(root);
    if(which == 3)
        printTreePostAVL(root);
}

size_t
treeRank(const struct tree *t, char * target)
{
    size_t rank = 0;

    while(t && strcmp(t->key, target)) {
        if(strcmp(t->key, target) < 0) {
            /* go right */
            /* root and left subtree are all less than target */
            rank += (1 + treeSizeAVL(t->child[LEFT]));
            t = t->child[RIGHT];
        } else {
            /* go left */
            t = t->child[LEFT];
        }
    }

    /* we must also count left subtree */
    return rank + treeSizeAVL(t->child[LEFT]);
}

char *
treeUnrank(const struct tree *t, size_t rank)
{
    size_t leftSize;

    /* basic idea: if rank < treeSize(child[LEFT]), recurse in left child */
    /* if it's equal, return the root */
    /*else recurse in right child with rank = rank - treeSize(child[LEFT]) - 1*/
    while(rank != (leftSize = treeSizeAVL(t->child[LEFT]))) {
        if(rank < leftSize) {
            t = t->child[LEFT];
        } else {
            t = t->child[RIGHT];
            rank -= (leftSize + 1);
        }
    }

    return t->key;
}

void
treeSanityCheck(const struct tree *root)
{
    int i;

    if(root) {
        assert(root->height == treeComputeHeight(root));
        assert(root->size == treeComputeSize(root));

        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            treeSanityCheck(root->child[i]);
        }
    }
}

#ifdef TEST_MAIN
/* check that aggregate data is correct throughout the tree */

int
main(int argc, char **argv)
{
    struct tree *root = TREE_EMPTY;

    if(argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        return 1;
    }

    assert(!treeContains(root, "boi"));
    treeInsertAVL(&root, "boi");
    assert(treeContains(root, "boi"));
#ifdef PRINT_AFTER_OPERATIONS
    treePrint(root, 0);
    puts("---");
#endif

    treeSanityCheck(root);

    /* now delete everything */
    assert(treeContains(root, "boi"));
    treeDelete(&root, "boi");
    assert(!treeContains(root, "boi"));
#ifdef PRINT_AFTER_OPERATIONS
    treePrint(root, 0);
    puts("---");
#endif

    treeSanityCheck(root);

    treeDestroyAVL(&root);

    return 0;
}
#endif

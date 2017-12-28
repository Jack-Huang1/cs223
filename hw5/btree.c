#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "/c/cs223/hw5/btree.h"


char *strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);   // Space for length plus nul
    if (d == NULL) return NULL;          // No memory
    strcpy (d,s);                        // Copy the characters
    return d;                            // Return the new string
}

struct node *makeNode(char * key)
{
	struct node * newnode = malloc(sizeof(struct node));
	assert(newnode);
	newnode->key = strdup(key);
	newnode->count = 1;
	newnode->left = NULL;
	newnode->right = NULL;
	newnode->next = NULL;
    return newnode;
}

int treeSize(struct node *root)
{
	if(!root)
		return 0;

	return 1 + treeSize(root->left) + treeSize(root->right);
}
int treeHeight(struct node *root)
{
	if(!root)
		return -1;

	int left = treeHeight(root->left);
	int right = treeHeight(root->right);

	return 1 + (left > right ? left : right);
}

struct node * treeSearch(struct node *root, char * target)
{
	while(root && strcmp(root->key, target)) 
	{
        if(strcmp(target, root->key) < 0)
        {
            root = root->left;
        } 
        else 
        {
            root = root->right;
        }
    }

    return root;
}

void treeInsert(struct node *root, char * key)
{
	struct node *newNode = makeNode(key);

    for(;;) 
    {
        if(strcmp(key, root->key) < 0)
        {
            if(root->left)
            {
                root = root->left;
            } 
            else 
            {
                root->left = newNode;
                return;
            }
        } 
        else 
        {
            if(root->right) 
            {
                root = root->right;
            } 
            else 
            {
                root->right = newNode;
                return;
            }
        }
    }
}

void printTreePre(struct node * tree)
{
    if (tree == NULL)
        return;
    printf("%s [%d / %d] ", tree->key, tree->count, treeHeight(tree));
    printTreePre(tree->left);
    printTreePre(tree->right);
}
void printTreeIn(struct node * tree)
{
    if (tree == NULL)
        return;
    printTreeIn(tree->left);
    printf("%s [%d / %d] ", tree->key, tree->count, treeHeight(tree));
    printTreeIn(tree->right);
}
void printTreePost(struct node * tree)
{
    if (tree == NULL)
        return;
    printTreePost(tree->left);
    printTreePost(tree->right);
    printf("%s [%d / %d] ", tree->key, tree->count, treeHeight(tree));
}

void treeDestroy(struct node * tree)
{
    if (tree == NULL)
        return;
    treeDestroy(tree->left);
	treeDestroy(tree->right);
    free(tree->key);
	free(tree);
}

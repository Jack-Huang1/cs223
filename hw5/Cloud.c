#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "/c/cs223/hw5/btree.h"
#include "AVLtree.h"

int main(int argc, char **argv)
{
	bool debug = false, preorder = false, inorder = false, 
	postorder = false, html = false, avl = false;
	long threshold = 5;

	//reading flags
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-debug"))
		{
			debug = true;
		}
		else if (!strcmp(argv[i], "-threshold"))
		{
			i++;
			if (i == argc)
			{
				printf("Missing threshold argment on command line.\n");
				break;
			}
			char *ptr = NULL;
			long hold = threshold;
   			threshold = strtol(argv[i], &ptr, 10);
   			if (*ptr || threshold < 2)
   			{
   				threshold = hold;
   				fprintf(stderr, "Invalid threshold value: %s\n", argv[i]);
   				continue;
   			}
		}
		else if (!strcmp(argv[i], "-preorder"))
		{
			preorder = true;
		}
		else if (!strcmp(argv[i], "-inorder"))
		{
			inorder = true;
		}
		else if (!strcmp(argv[i], "-postorder"))
		{
			postorder = true;
		}
		else if (!strcmp(argv[i], "-html"))
		{
			html = true;
		}
		else if (!strcmp(argv[i], "-avl"))
		{
			avl = true;
		}
		else
		{
			fprintf(stderr, "Fatal error: invalid command line argument: %s\n",
			 argv[i]);
			exit(1);
		}
	}

	//avl tree initialization
	struct tree *avlroot = TREE_EMPTY;
	struct tree *linkedlist = TREE_EMPTY;

	//binary tree initialization
	struct node *root = makeNode("**root**");
	root->parent = NULL;
	root->height = 0;

	//reading input line(s)
	char buffer[1025];
    while (fgets(buffer, 1025, stdin))
    {
    	//gets rid of newline character
        buffer[strlen(buffer) - 1] = '\0';

        if (debug)
        	printf("Input: %s\n", buffer);

        //loop to add words onto tree
        char *token = strtok(buffer, " ");
        while (token != NULL)
        {
        	//checks for valid input
        	for (int p = 0; token[p]; p++)
			{
				if (!isalpha(token[p]))
					goto jump;
				token[p] =  tolower(token[p]);
			}

			//processing word
			if (avl) // avl tree operations
			{
				struct tree * x  = treeContains(avlroot, token);
				if (x)
				{
					x->count++;
					if (x->count == threshold)
					{
						if (linkedlist == NULL)
						{
							linkedlist = x;
							x->next = NULL;
						}
						else
						{
							x->next = linkedlist;
							linkedlist = x;
						}
					}
				}
				else
				{
					treeInsertAVL(&avlroot, token);
				}
				//treeSanityCheck(avlroot);
			}
			else // binary tree operations
			{
				struct node * x  = treeSearch(root, token);
				if(x)
				{
					x->count++;
					if (x->count == threshold)
					{
						x->next = root->next;
						root->next = x;
					}
				}
				else
				{
					treeInsert(root, token);
				}
			}

			jump:
        	//continues the loop
            token = strtok(NULL, " ");
        }
    }

    //processing print flags
    if (debug)
    {
    	if (avl)
    	{
    		printf("Tree height: %d\n", treeHeightAVL(avlroot));
    		printf("Tree size: %zu\n", treeSizeAVL(avlroot));
    		treePrint(avlroot, 0);
    	}
    	else
    	{
    		printf("Tree height: %d\n", treeHeight(root));
    		printf("Tree size: %d\n", treeSize(root));
    	}
    		
    }
    if (preorder)
    {
    	printf("PREORDER\n");
    	if (avl)
    	{
    		treePrint(avlroot, 1);
    	}
    	else
    	{
    		printTreePre(root);
    	}
    	printf("\n");
    }
    if (inorder)
    {
    	printf("INORDER\n");
    	if (avl)
    	{
    		treePrint(avlroot, 2);
    	}
    	else
    	{
    		printTreeIn(root);
    	}
    	printf("\n");
    }
    if (postorder)
    {
    	printf("POSTORDER\n");
    	if (avl)
    	{
    		treePrint(avlroot, 3);
    	}
    	else
    	{
    		printTreePost(root);
    	}
    	printf("\n");
    }
    
    //final print
    if (html)
    {
    	if (avl)
    	{
    		if (linkedlist == NULL)
    		{
    			printf("No words seen %ld times.\n", threshold);
    		}
    		else
    		{
	    		while (linkedlist)
		    	{
		    		printf("<span style=\"font-size: %dpx\"> %s </span>\n",
		    		 linkedlist->count, linkedlist->key);
		    		linkedlist = linkedlist->next;
		    	}
	    	}
    	}
    	else
    	{
    		if (root->next == NULL)
    		{
    			printf("No words seen %ld times.\n", threshold);
    		}
    		else
    		{
    			struct node *read = root->next;
	    		while (read)
		    	{
		    		printf("<span style=\"font-size: %dpx\"> %s </span>\n",
		    		 read->count, read->key);
		    		read = read->next;
		    	}
    		}
    	}
    }
    else
    {
    	if (avl)
    	{
    		if (linkedlist == NULL)
    		{
    			printf("No words seen %ld times.\n", threshold);
    		}
    		else
	    	{
		    	printf("The Word Cloud:\n");
		    	int b = 0;
	    		while (linkedlist)
		    	{
		    		printf("[%d] %s [%d]\n", b++, linkedlist->key,
		    		 linkedlist->count);
		    		linkedlist = linkedlist->next;
		    	}
		    }
    	}
    	else
    	{
    		if (root->next == NULL)
    		{
    			printf("No words seen %ld times.\n", threshold);
    		}
    		else
	    	{
		    	printf("The Word Cloud:\n");
		    	struct node *read = root->next; 
		    	int b = 0;
	    		while (read)
		    	{
		    		printf("[%d] %s [%d]\n", b++, read->key, read->count);
		    		read = read->next;
		    	}
		    }
    	}
    }

	treeDestroyAVL(&avlroot);
	treeDestroy(root);
	exit(0);
}

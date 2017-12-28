#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "/c/cs223/hw6/dict.h"
#include "/c/cs223/hw6/heap.h"

void depth(struct room * start, Dict d, bool * found, bool conn, int * sum)
{
	if (*found) // fire has been found, base case for exiting recursion
	{
		return;
	}
	else if (conn && start->visited != 4) // function for connectedness
	{
		(*sum)++; // counting nodes visited
		start->visited = 4;

		//recursively check neighbors
		for (int i = 0; i < start->ncount; i++)
		{
			//neighbor isn't in graph or is the room itself
			if (!DictSearch(d, start->neighbors[i])
			 || !strcmp(start->neighbors[i], start->room))
				continue;

			depth(DictSearch(d, start->neighbors[i]), d, found, conn, sum);
		}
	}
	else if (!conn && start->visited != 1) // regular depth function
	{
		printf("%s ", start->room);
		start->visited = 1;

		if (start->temp > 400) // check for fire
		{
			printf(" SUCCESS!");
			*found = true;
			return;
		}
		else
		{
			//recursively check neighbors
			for (int i = 0; i < start->ncount; i++)
			{
				//neighbor isn't in graph or is the room itself
				if (!DictSearch(d, start->neighbors[i])
				 || !strcmp(start->neighbors[i], start->room))
					continue;

				depth(DictSearch(d, start->neighbors[i]), d, found, conn, sum);
			}
		}
	}
}

void bb(struct room * start, Heap h, Dict d, bool breadth)
{
	int count = 1;
	while (!empty(h))
	{
		printf("%s ", start->room);
		deletemin(h);
		if (start->temp > 400)
		{
			printf(" SUCCESS!");
			return;
		}

		//add neighbors to heap
		for (int i = 0; i < start->ncount; i++)
		{
			//neighbor isn't in graph or is the room itself
			if (!DictSearch(d, start->neighbors[i])
			 || !strcmp(start->neighbors[i], start->room))
				continue;

			if (breadth) // key is order in which the node is added
			{
				if (DictSearch(d, start->neighbors[i])->visited == 2)
					continue;

				insert(h, count++, DictSearch(d, start->neighbors[i]));
				DictSearch(d, start->neighbors[i])->visited = 2;
			}
			else // key is the negative temperature
			{
				if (DictSearch(d, start->neighbors[i])->visited == 3)
					continue;

				insert(h, (-1) * (DictSearch(d, start->neighbors[i])->temp),
				 DictSearch(d, start->neighbors[i]));
				DictSearch(d, start->neighbors[i])->visited = 3;
			}
		}

		if (empty(h))
			break;

		start = findmin(h);
	}

	printf(" FAILED");
}

bool directed(Dict dict)
{
	bool undirected = true;

	for(int i = 0; i < dict->size; i++) 
	{
      if (dict->table[i])
      {
      	for (struct elt * e = dict->table[i]; e != NULL; e = e->next)
		{
			//looping through neighbors
			for (int z = 0; z < e->value->ncount; z++)
			{
				//room is not in the graph
				if (!DictSearch(dict, e->value->neighbors[z]))
				{
					undirected = false;
					printf("Rooms %s and %s are not symmetric.\n",
					 e->value->neighbors[z], e->key);
					continue;
				}

				bool dirr = false;

				//loop to check the neighbor's neighbors
				for (int p = 0; p < DictSearch(dict,
				 e->value->neighbors[z])->ncount; p++)
				{
					if (!strcmp(e->key, DictSearch(dict,
					 e->value->neighbors[z])->neighbors[p]))
					{
						dirr = true;
						break;
					}
				}

				//neighbor does not have the room listed as a neighbor
				if (!dirr)
				{
					undirected = false;
					printf("Rooms %s and %s are not symmetric.\n",
					 e->value->neighbors[z], e->key);
				}
			}
		}
      }
    }

    return undirected;
}

int main(int argc, char **argv)
{
	//flag variables
	bool room = false, dfs = false, bfs = false, best = false, 
	conn = false, dir = false;
	char start[4];

	//bad command line
	if (argc == 1)
	{
		fprintf(stderr,
		 "Usage: Fire -room value [-dfs | -bfs | -best | -conn | -dir]\n");
		exit(1);
	}

	//reading flags
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-room"))
		{
			room = true;
			i++;
			if (i == argc)
				goto bad;
			strcpy(start, argv[i]);
		}
		else if (!strcmp(argv[i], "-dfs"))
		{
			dfs = true;
		}
		else if (!strcmp(argv[i], "-bfs"))
		{
			bfs = true;
		}
		else if (!strcmp(argv[i], "-best"))
		{
			best = true;
		}
		else if (!strcmp(argv[i], "-conn"))
		{
			conn = true;
		}
		else if (!strcmp(argv[i], "-dir"))
		{
			dir = true;
		}
		else
		{
			fprintf(stderr, "Fatal error: unknown command line argument: %s\n",
			 argv[i]);
			exit(1);
		}
	}

	//default setting to depth first
	if (!dfs && !bfs && !best && !conn & !dir)
		dfs = true;

	//room was never given
	if (!room)
	{
		bad:
		fprintf(stderr, "Fatal error: no room given.\n");
		fprintf(stderr,
		 "Usage: Fire -room value [-dfs | -bfs | -best | -conn | -dir]\n");
		exit(1);
	}

	//initialize dictionary
	Dict dict = DictCreate();

	//loading dictionary
	char buffer[1025];
    while (fgets(buffer, 1025, stdin))
    {
    	//gets rid of newline character
    	if (buffer[strlen(buffer) - 1] == '\n')
        	buffer[strlen(buffer) - 1] = '\0';

        //create room struct to add to dictionary
        char * token = strtok(buffer, " ");
        if(!token)
        	continue;

        struct room * r = malloc(sizeof(struct room));

        //room was already added
        if (DictSearch(dict, token))
        {
        	fprintf(stderr, "Room %s already in graph.  Replacing it.\n",token);
        	DictDelete(dict, token);
        }

        strcpy(r->room, token);
        r->ncount = 0;
        r->visited = 0;

        //adding temperature value
        char * temp = strtok(NULL, " ");
        if (!temp)
        {
        	r->temp = 0;
        	DictInsert(dict, r->room, r);
        	continue;
        }	
        r->temp = atoi(temp); // add temperature of room
        
        //loop through inputs to add neighbors
        token = strtok(NULL, " ");
        while (token != NULL)
        {
        	//neighbor is room itself
        	if (!strcmp(token, r->room))
        		goto next;

        	//loop to skip duplicates
        	for (int i = 0; i < r->ncount; i++)
        	{
        		if (!strcmp(r->neighbors[i], token))
        			goto next;
        	}

        	//adding neighbor
        	strcpy(r->neighbors[r->ncount++], token);

        	next:
        	token = strtok(NULL, " ");
        }

        DictInsert(dict, r->room, r);
    }

    //room specified in commard argument line to start
    struct room * go = DictSearch(dict, start);

    //room given in command line is not on the graph
    if (!go)
    {
    	DictDestroy(dict);
    	fprintf(stderr, "Fatal error: room %s not included in graph.\n", start);
		exit(1);
    }

    //check to see if there are bad neighbors
    for(int i = 0; i < dict->size; i++) 
	{
      if (dict->table[i])
      {
      	for (struct elt * e = dict->table[i]; e != NULL; e = e->next)
		{
			for (int z = 0; z < e->value->ncount; z++)
			{
				if (!DictSearch(dict, e->value->neighbors[z]))
				{
					fprintf(stderr, "Warning: room %s has neighbor ", e->key);
					fprintf(stderr, "%s which is not in the dictionary.\n",
					 e->value->neighbors[z]);
				}
			}
		}
      }
    }

    //variables for checking connectedness
    int count = 0;
    int * sum = &count;

    //variables for ending depth recursion
    bool success = false;
	bool * found = &success;

    //depth-first
    if (dfs)
    {
    	printf("Starting depth first search: ");

    	depth(go, dict, found, false, sum);

    	if (!*found) // no fire was found
    		printf(" FAILED");

    	printf("\n");
    }
    //breadth-first
    if (bfs)
    {
    	printf("Starting breadth first search: ");

    	//initializing and loading heap
		Heap h = initheap();
		insert(h, 0, go);
		go->visited = 2;

		bb(go, h, dict, true);

		//free heap
		for (int i = 0; i <= h->n; i++)
			free(h->heap[i]);
		free(h->heap);
		free(h);

		printf("\n");
    }
    //best-first
    if (best)
    {
    	printf("Starting best first search: ");

    	//initializing and loading heap
		Heap h = initheap();
		insert(h, ((-1) * go->temp), go);
		go->visited = 3;

    	bb(go, h, dict, false);

    	//free heap
    	for (int i = 0; i <= h->n; i++)
			free(h->heap[i]);
		free(h->heap);
		free(h);

    	printf("\n");
    }
    //connected
    if (conn)
    {
    	success = false;

    	depth(go, dict, found, true, sum);

    	if (count == dict->n)
    	{
    		printf("Graph is connected.\n");
    	}
    	else
    	{
    		printf("Graph is NOT connected.\n");
    	}
    }
    //directed
    if (dir)
    {
	    if (directed(dict))
	    {
	    	printf("Graph is undirected.\n");
	    }
	    else
	    {
	    	printf("Graph is directed.\n");
	    }
    }

    DictDestroy(dict);
}

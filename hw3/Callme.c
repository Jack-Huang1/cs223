/* 
 CS 223b, hw2
 Name: Jack Huang
 netid: jh2533
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "/c/cs223/hw3/hash.h"

void convertletters(char* word, int newint[]) // converts word to a number
{
	char ch;
	for (int i = 0; word[i] != '\0'; i++)
	{
		ch = tolower(word[i]);
		switch(ch)
		{
			case 'a': case 'b': case 'c':
				newint[i] = 2;
				continue;
			case 'd': case 'e': case 'f':
				newint[i] = 3;
				continue;
			case 'g': case 'h': case 'i':
				newint[i] = 4;
				continue;
			case 'j': case 'k': case 'l':
				newint[i] = 5;
				continue;
			case 'm': case 'n': case 'o':
				newint[i] = 6;
				continue;
			case 'p': case 'q': case 'r': case 's':
				newint[i] = 7;
				continue;
			case 't': case 'u': case 'v':
				newint[i] = 8;
				continue;
			case 'w': case 'x': case 'y': case 'z':
				newint[i] = 9;
				continue;
			default: // checks for invalid inputs
				fprintf(stderr, "Fatal error: invalid argument %s\n", word);
				exit(0);
		}
	}
}

// holds values for letters corresponding to a digit
const char values[10][5] = {"", "", "abc", "def", "ghi", "jkl",
                               "mno", "pqrs", "tuv", "wxyz"};
void convertnumbers(int input[], int x, int l, char term[], Hash d, bool* match)
{
	if (x == l) // base case: we've reached the end of the term
	{
		if (HashSearch(d, term))
		{
			printf(" %s", term);
			*match = true;
		}
	}
	else
	{
		for (int i = 0; i < strlen(values[input[x]]); i++)
		{
			term[x] = values[input[x]][i];
			convertnumbers(input, x + 1, l, term, d, match);
			if (input[x] == 0 || input[x] == 1)
			{
				printf("numeric: %d => ** no matches **\n", *input);
				exit(0);
			}
		}
	}
}

int main(int argc, char **argv)
{
	char file[] = "";
	bool filecheck = false;
	if (argc == 1 || argc > 5) // checks if there is an input to begin with
	{
		fprintf(stderr, 
		"usage: Callme (digits | letters) [-debug]? [-file filename]\n");
		exit(0);
	}

	for (int i = 1; i < argc; i++) // loop to read flags
	{
		if (!strcmp(argv[i], "-debug"))
		{
			if (!filecheck)
				debugflag = true;
			else
			{
				fprintf(stderr, "Fatal error: invalid argument %s\n", argv[2]);
				exit(0);
			}
		}
		else if (!strcmp(argv[i], "-file"))
		{
			if (i + 1 < argc)
			{
				strcat(file, argv[i + 1]);
				filecheck = true;
			}
			else
			{
				fprintf(stderr, "Fatal error: invalid argument %s\n", argv[2]);
				exit(0);
			}
			
		}
		else if (i != 1 && strcmp(argv[i-1], "-file"))
		{
			fprintf(stderr, "Fatal error: invalid argument %s\n", argv[2]);
			exit(0);
		}
	}

    char *ep = NULL;
    long read = strtol(argv[1], &ep, 10);
    if (*ep == '.') // catches floats
	{
    	fprintf(stderr, "Fatal error: Invalid argument: %s\n", argv[1]);
		exit(0);
	}

	if (!read) // input is a word
	{
	    for (char* check = argv[1]; check[0] != '\0'; check++)
	    {
			if (check[0] == '0') // catches when the entire term is all zeroes
				continue;
			int convletter[strlen(argv[1])];
			convertletters(argv[1], convletter);
			printf("alphabetic: %s => ", argv[1]);
			for (int z = 0; z < strlen(argv[1]); z++) // prints conversion
				printf("%d", convletter[z]);
			printf("\n");
			exit(0);
		}
		printf("numeric: %s => ** no matches **\n", argv[1]);
		exit(0);
	}
	else // input is a number
	{
		// check if the input is valid
		int input[strlen(argv[1])];
		for (char* temp = argv[1]; temp[0] != '\0'; temp++)
		{
			if (!isdigit(temp[0]))
			{
				fprintf(stderr, "Fatal error: invalid argument %s\n", argv[1]);
				exit(0);
			}
			input[temp-argv[1]] = temp[0] - '0';
		}

		// loading dictionary
		if (!filecheck) // use default dictionary when no file specified
			strcat(file, "/usr/share/dict/words");
		FILE* filepointer = fopen(file, "r");
		if (debugflag)
			printf("Loading dictionary: %s\n", file);
		if (filepointer == NULL)
		{
			fprintf(stderr, "Fatal error: dictionary not found.\n");
			exit(0);
		}
		Hash dict = HashCreate();
		long wordcount = 0;
		char *hold = malloc(257);
		while (fscanf(filepointer, "%s", hold) != EOF)
		{
			bool valid = true;
			for (int p = 0; hold[p]; p++)
			{
				hold[p] =  tolower(hold[p]); // makes entire word lowercase
				if (!isalpha(hold[p])) // checks for only letters
					valid = false;
			}
			if (!valid)
				continue;
			if (strlen(hold) == strlen(argv[1])) // checks for right length
			{
				hold[strlen(argv[1]) + 1] = '\0';
				if (HashSearch(dict, hold)) // checks for repeats
					continue;
				HashInsert(dict, hold);
				wordcount++;
			}
		}
		free(hold);
		fclose(filepointer);
		if (debugflag)
			printf("Word Count: %lu\n", wordcount);
		// end of loading dictionary

		printf("numeric: %s =>", argv[1]);
		char term[strlen(argv[1]) + 1];
		term[strlen(argv[1])] = '\0';
		bool anymatches = false;
		bool *match = &anymatches;
		convertnumbers(input, 0, strlen(argv[1]), term, dict, match);
		if (!*match) // there were no matches
			printf(" ** no matches **");
		HashDestroy(dict);
		printf("\n");
	}
}

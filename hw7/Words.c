#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "/c/cs223/hw7/hash.h"

void twowords(char * word, Hash dict)
{
	//array to hold the indexes of valid splits
	int hold[strlen(word)];
	int count = 0;

	//looping through the word to check for splits
	for (int i = 0; i < strlen(word) - 1; i++)
	{
		//making substrings
		char front[i + 2];
		for (int y = 0; y <= i; y++)
			front[y] = word[y];
		front[i + 1] = '\0';
		char back[strlen(word) - i];
		int what = 0;
		for (int z = i + 1; z < strlen(word); z++)
			back[what++] = word[z];
		back[strlen(word) - i - 1] = '\0';
		
		//checks if the split is valid
		if (HashSearch(dict, front) && HashSearch(dict, back))
			hold[count++] = i;
	}

	printf("Two words: ");
	if (!count)
	{
		printf("FAILURE\n");
		return;
	}

	//printing output
	printf("SUCCESS: ");
	for (int x = 0; x < count; x++)
	{
		//making substrings
		char front[hold[x] + 2];
		for (int y = 0; y <= hold[x]; y++)
			front[y] = word[y];
		front[hold[x] + 1] = '\0';
		char back[strlen(word) - hold[x]];
		int what = 0;
		for (int z = hold[x] + 1; z < strlen(word); z++)
			back[what++] = word[z];
		back[strlen(word) - hold[x] - 1] = '\0';

		printf("%s %s.  ", front, back);
	}
	printf("\n");
}

void dpalg(char * word, Hash dict)
{
	printf("DP: ");
	//checks if whole input is a word
	if (HashSearch(dict, word))
	{
		printf("SUCCESS: %s \n", word);
		return;
	}

	//initializing array to track splits
	int matrix[strlen(word) + 1];
	matrix[strlen(word)] = 0;
	for (int i = 0; i < strlen(word); i++)
		matrix[i] = -1;

	//looping through the word backwards
	for (int i = (strlen(word) - 1); i >= 0; i--)
	{
		//checking for splits
		for (int j = i; j < strlen(word); j++)
		{
			//making substring
			char temp[j - i + 2];
			int count = 0;
			for (int y = i; y <= j; y++)
				temp[count++] = word[y];
			temp[j - i + 1] = '\0';

			if (HashSearch(dict, temp) && matrix[j + 1] != -1)
				matrix[i] = j + 1;
		}
	}

	//printing output
	if (matrix[0] == -1)
	{
		printf("FAILURE\n");
	}
	else
	{
		printf("SUCCESS: ");
		for (int i = 0; i < strlen(word); i++)
		{
			if (matrix[i] != -1)
			{
				char temp[matrix[i] - i + 1];
				int count = 0;
				for (int y = i; y < matrix[i]; y++)
					temp[count++] = word[y];
				temp[matrix[i] - i] = '\0';
				printf("%s ", temp);
				i = matrix[i] - 1;
			}
		}
		printf(" \n");
	}

	if (debugflag)
	{
		for (int i = 0; i < strlen(word); i++)
			printf("%d ", matrix[i]);
		printf("\n");
	}
}

int main(int argc, char **argv)
{
	char file[] = "";
	bool filecheck = false;

	//reading flags
	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-dict"))
		{
			i++;
			if (i == argc)
			{
				fprintf(stderr, "usage: Words [-dict filename | -debug]\n");
				exit(1);
			}
			else
			{
				strcat(file, argv[i]);
				filecheck = true;
			}
		}
		else if (!strcmp(argv[i], "-debug"))
		{
			debugflag = true;
		}
		else
		{
			fprintf(stderr, "usage: Words [-dict filename | -debug]\n");
			exit(1);
		}
	}

	/*loading dictionary*/
	if (!filecheck)
			strcat(file, "/c/cs223/hw7/words");

	if(debugflag)
	{
		if (!filecheck)
		{
			printf("Loading dictionary: words\n");
		}
		else
		{
			printf("Loading dictionary: %s\n", file);
		}
	}

	FILE* filepointer = fopen(file, "r");

	if (filepointer == NULL)
	{
		fprintf(stderr, "Fatal error: dictionary not found.\n");
		exit(1);
	}
	Hash dict = HashCreate();
	long wordcount = 0;

	//adding words to hash table
	char *hold = malloc(257);
	while (fscanf(filepointer, "%s", hold) != EOF)
	{
		bool valid = true;

		//converts to lowercase and catches non-alphabetic characters
		for (int p = 0; hold[p]; p++)
		{
			hold[p] = tolower(hold[p]);
			if (!isalpha(hold[p]))
				valid = false;
		}

		if (!valid)
			continue;

		hold[strlen(hold) + 1] = '\0';

		//checks for repeats
		if (HashSearch(dict, hold))
			continue;

		HashInsert(dict, hold);
		wordcount++;
	}
	free(hold);
	fclose(filepointer);
	/*end of loading dictionary*/
	if (debugflag)
		printf("Word count: %lu\n", wordcount);

	//processing inputs
	char * word = malloc(1025);
    while (fgets(word, 1025, stdin))
    {
    	//empty line
    	if (word[0] == '\n')
    	{
			printf("Two words: FAILURE\nDP: SUCCESS: \n---\n");
			continue;
		}

    	//gets rid of newline character
    	if (word[strlen(word) - 1] == '\n')
        	word[strlen(word) - 1] = '\0';

    	//converts to lowercase
		for (int p = 0; word[p]; p++)
			word[p] = tolower(word[p]);

		if (debugflag)
			printf("Input: '%s'\n", word);

    	twowords(word, dict);
    	dpalg(word, dict);
    	printf("---\n");
    }

    free(word);
    HashDestroy(dict);
}

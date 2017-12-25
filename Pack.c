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

int nextfit(int pack[], int size, bool trace)
{
	if (size == 0) // checks if size array is empty
		return 0;
	int bins[size]; // array of bins (made to be the size of array of sizes)
	memset(bins, 0, sizeof(bins)); // clears memory just in case
	int binposition = 0; // indicates what bin to work with
	for (int i = 0; i < size; i++)
	{
		if (bins[binposition] + pack[i] > 100) // checks when to make a new bin
		{
			binposition++;
			bins[binposition] = pack[i];
		}
		else
		{
			bins[binposition] += pack[i];
		}
		if (trace) // implements trace
		{
			printf("arg: %d val: %d bin: %d total: %d\n", i, pack[i], 
			binposition, bins[binposition]);
		}
	}
	return binposition + 1; // adds one because position started at zero
}

int firstfit(int pack[], int size, bool trace)
{
	if (size == 0) // checks if size array is empty
		return 0;
	int bins[size]; // array of bins (made to be the size of array of sizes)
	memset(bins, 0, sizeof(bins)); // clears memory just in case
	int binnum = 0; // counts how many bins used
	int binposition = 0;
	for (int i = 0; i < size; i++)
	{
		for (int x = 0; x < size; x++)
		{
			binposition = x;
			if (bins[x] + pack[i] <= 100) // checks if it fits in bin
			{
				bins[x] += pack[i];
				if (x > binnum) // counts number of bins used
					binnum = x;
				break;
			}
		}
		if (trace) // implements trace
		{
			printf("arg: %d val: %d bin: %d total: %d\n", i, pack[i], 
			binposition, bins[binposition]);
		}
	}
	return binnum + 1; // adds one because bins index starts at zero
}

int bestfit(int pack[], int size, bool trace)
{
	if (size == 0) // checks if size array is empty
		return 0;
	int bins[size]; // array of bins (made to be the size of array of sizes)
	memset(bins, 0, sizeof(bins)); // clears memory just in case
	int bintouse = 0; // indicates which bin fits best
	int binnum = 0; // counts how many bins used
	for (int i = 0; i < size; i++)
	{
		int tightest = 100; // keeps track of tightest fit value
		for (int x = 0; x < size; x++) // loops to check fit with all bins
		{
			int check = 100 - bins[x] - pack[i]; // checks the fit margin
			if (bins[x] == 0 && binnum == x) 
			{
				bintouse = x;
				break;
			}
			if (0 <= check && check < tightest) // checks for tightest fit
			{
				tightest = check;
				bintouse = x;
			}
		}
		bins[bintouse] += pack[i];
		if (bintouse > binnum) // counts number of bins used
			binnum = bintouse; 
		if (trace) // implements trace
		{
			printf("arg: %d val: %d bin: %d total: %d\n", i, pack[i], 
			bintouse, bins[bintouse]);
		}
	}
	return binnum + 1; // adds one because bins index starts at zero
}

int optmrecur(int pack[], int place, int size, int bins[], int bincount)
{
	int min = size; // keeps track of mininum used bins
	int check = 0; // tracks the number of bins used so far
	if (place == size) // all sizes have been put into bins
	{
		return bincount;
	}
	for (int i = 0; i < bincount; i++)
	{
		if (bins[i] + pack[place] <= 100)
		{
			bins[i] += pack[place];
			check = optmrecur(pack, place + 1, size, bins, bincount);
			if (check < min)
				min = check; // checks for minimum used bins after recursion
			bins[i] -= pack[place]; // backtracks
		}
	}
	bins[bincount] = pack[place];
	check = optmrecur(pack, place + 1, size, bins, bincount + 1);
	if (check < min)
		min = check; // checks for minimum used bins after recurion
	bins[bincount] -= pack[place]; // backtracks
	return min;
}

int optm(int pack[], int size) // function to initialize values for optm
{
	int placement = 0;
	int bincount = 0;
	if (size == 0) // checks if size array is empty
		return 0;
	int bins[size]; // array of bins (made to be the size of array of sizes)
	memset(bins, 0, sizeof(bins)); // clears memory just in case
	return optmrecur(pack, placement, size, bins, bincount);
}

int comp(const void * a, const void * b) // function to sort array
{
   return (*(int*)b - *(int*)a);
}

int main(int argc, char **argv)
{
	int sizes[argc];
	int sposition = 0; // provides position for sizes array
	int fposition = 1; // provides position for navigating argv for flags
	bool trace = false, nexttrace = false, firsttrace = false, 
	besttrace = false, ffdtrace = false, bfdtrace = false;

	if (argc == 1)
	{
		fprintf(stderr, "usage: Pack [sizes]* [-next | ");
		fprintf(stderr, "-first | -best | -ffd | -bfd | -optm]+ -trace*\n");
		exit(0);
	}

	for (int i = 1; i < argc; i++) // loop to add values to sizes array
	{
		if (atoi(argv[i]) < 0 || atoi(argv[i]) > 100)
		{
			fprintf(stderr, "Fatal error: Invalid size: %s\n", argv[i]);
			exit(0);
		}
		if (atoi(argv[i]) > 0 && atoi(argv[i]) < 100)
		{
			if (strlen(argv[i]) > 3)
			{
				fprintf(stderr, "Fatal error: Invalid size: %s\n", argv[i]);
				exit(0);
			}
			char *ep = NULL;
		    strtol (argv[i], &ep, 10);
		    if (*ep == '.')
		    {
		    	fprintf(stderr, "Fatal error: Invalid size: %s\n", argv[i]);
				exit(0);
		    }
		}
		if (i == argc - 1 && !atoi(argv[i]) == 0)
		{
			fprintf(stderr, "Fatal error: no algorithm specified.\n");
			exit(0);
		}
		if (i == argc - 1 && atoi(argv[i]) == 0 && !strcmp(argv[i], "-trace"))
		{
			fprintf(stderr, "Fatal error: no algorithm specified.\n");
			exit(0);
		}
		if (atoi(argv[i]) == 0) // ends loop when we reach flags
		{
			fposition = i;
			break;
		}
		sizes[sposition] = atoi(argv[i]);
		sposition++;
	}

	for (int y = fposition; y < argc; y++) // loop to check for trace and errors
	{
		if (!strcmp(argv[y], "-trace"))
		{
			trace = true;
			for (int t = fposition; t < argc; t++)
			{
				if (!strcmp(argv[t], "-next"))
					nexttrace = true;
				else if (!strcmp(argv[t], "-first"))
					firsttrace = true;
				else if (!strcmp(argv[t], "-best"))
					besttrace = true;
				else if (!strcmp(argv[t], "-ffd"))
					ffdtrace = true;
				else if (!strcmp(argv[t], "-bfd"))
					bfdtrace = true;
			}
		}
		else if (!atoi(argv[y]) == 0)
		{
			fprintf(stderr, "Fatal error: Size option out of order: %d\n", 
			atoi(argv[y]));
			exit(0);
		}
		else if (strcmp(argv[y], "-next") && strcmp(argv[y], "-first") &&
			strcmp(argv[y], "-best") && strcmp(argv[y], "-ffd") &&
			strcmp(argv[y], "-bfd") && strcmp(argv[y], "-optm"))
		{
			fprintf(stderr, "Fatal error: bad argument: %s\n", argv[y]);
			exit(0);
		}
	}

	int sizesp[sposition]; // makes a copy of the array to be sorted
	for (int i = 0; i < sposition; i++)
		sizesp[i] = sizes[i];
	qsort(sizesp, sposition, sizeof(int), comp);

	if (trace) // implements trace (in order) when necessary
	{
		if (nexttrace)
		{
			printf("Trace -next\n");
			nextfit(sizes, sposition, true);
		}
		if (firsttrace)
		{
			printf("Trace -first\n");
			firstfit(sizes, sposition, true);
		}
		if (besttrace)
		{
			printf("Trace -best\n");
			bestfit(sizes, sposition, true);
		}
		if (ffdtrace)
		{
			printf("Trace -ffd\n");
			firstfit(sizesp, sposition, true);
		}
		if (bfdtrace)
		{
			printf("Trace -bfd\n");
			bestfit(sizesp, sposition, true);
		}
	}
	
	for (int z = fposition; z < argc; z++) // loop for algorithm calls
	{
		if (!strcmp(argv[z], "-next"))
			printf("-next: %d\n", nextfit(sizes, sposition, false));
		else if (!strcmp(argv[z], "-first"))
			printf("-first: %d\n", firstfit(sizes, sposition, false));
		else if (!strcmp(argv[z], "-best"))
			printf("-best: %d\n", bestfit(sizes, sposition, false));
		else if (!strcmp(argv[z], "-ffd"))
			printf("-ffd: %d\n", firstfit(sizesp, sposition, false));
		else if (!strcmp(argv[z], "-bfd"))
			printf("-bfd: %d\n", bestfit(sizesp, sposition, false));
		else if (!strcmp(argv[z], "-optm"))
			printf("-optm: %d\n", optm(sizesp, sposition));
	}
}

#ifndef SORTER_H
#define SORTER_H

typedef struct csv 
{
	char** filedata;
} CSV;

typedef struct coltypes 
{
	int type;
} COLTYPES;

typedef struct sorterargs 
{
	int argc;
	int coltosorton;
	char *colname;
	char* inputdirectory;
	char* filename;
} SORTERARGS;

typedef struct csvargs 
{
	int rows;
	int cols;
} CSVPARAMS;

int loadAndSort(char* buffer, int coltosorton);
//Suggestion: prototype a mergesort function


#endif

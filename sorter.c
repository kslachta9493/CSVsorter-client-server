
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorter.h"
#include "mergesort.c"
#include "builder.h"
#include <dirent.h>

struct node *BST = NULL;
int maxColMain=0;
pthread_mutex_t lockBST;

struct node
{
    	int coltoCompare;
    	char **fullrow;
    	struct node *left, *right;
};
  

struct node *newNode(int col, char **filedata, int maxCol)
{
    	struct node *temp =  (struct node *)malloc(sizeof(struct node));
    	temp->coltoCompare = col;
    	temp->fullrow=(char**) malloc(sizeof(char*) * maxCol);
    	temp->fullrow=filedata;
    	temp->left = NULL;
    	temp->right = NULL;
    	return temp;
}

void printBST(struct node *root, int maxCol, FILE* fp)
{

	int i=0;
    	if (root != NULL)
    	{
        	printBST(root->left, maxCol, fp);
		for (i=0; i<maxCol; i++)
		{

			if (strchr(root->fullrow[i], ','))
			{
				int cursize = strlen(root->fullrow[i]);
				char *tmp = (char*) malloc ((cursize + 2) * sizeof(char));
				int start;
				for (start = 0; start < cursize + 2; start++)
				{
					tmp[start] = '\0';
				}

				for (start = 1; start < cursize; start++) {
					tmp[start] = root->fullrow[i][start - 1];
				}
					tmp[0] = '"';
					tmp[cursize - 1] = '"';
					root->fullrow[i] = strdup(tmp);
					free(tmp);
			}

			if (i==maxCol-1)
			{
				fprintf(fp, "%s", root->fullrow[i]);
				//printf("%s\n", root->fullrow[i]);	
			}
			else
			{
				fprintf(fp, "%s,", root->fullrow[i]);	
				//printf("%s,", root->fullrow[i]);	
			}
		}
		fprintf(fp, "\n");	
        	printBST(root->right, maxCol, fp);
    	}
}

struct node* insert(struct node* node, int col, char **filedata, int maxCol, int type)
{

    	if (node == NULL) return newNode(col, filedata, maxCol);
 
 
	if (type==1) // it is an int
	{ 
		if (filedata[col][0] == ' ')
		{
			node->left  = insert(node->left, col, filedata, maxCol, type);
		}
		else if (node->fullrow[col][0] == ' ')
		{
        		node->right = insert(node->right, col, filedata, maxCol, type);   
		}
		else // both are actual int
		{
   			int new=atoi(filedata[col]);
    			int inBST=atoi(node->fullrow[col]);

			if (new < inBST)
        			node->left  = insert(node->left, col, filedata, maxCol, type);
    			else
        			node->right = insert(node->right, col, filedata, maxCol, type);   
		}
	}
	else if (type==0) // it is a string
	{
		char *rightc, *leftc;
		if (node->fullrow[col] != NULL) 
		{
			rightc = strdup(node->fullrow[col]);
		}
		if (filedata[col] != NULL) 
		{
			leftc = strdup(filedata[col]);
		}

		
         	int ai=0;
		for(ai = 0; leftc[ai]; ai++){
       			leftc[ai] = (char) tolower(leftc[ai]); 
  		}

	 	for(ai = 0; rightc[ai]; ai++){
         		rightc[ai] = (char) tolower(rightc[ai]); 
         	}

		int aSize=strlen(filedata[col]);
            	if (aSize>strlen(node->fullrow[col]))
	    		aSize=strlen(node->fullrow[col]);
		
		int x=0; int count=0;
            	for (x=0; x<aSize; x++)
            	{
	    		if (leftc[x]==rightc[x])
	    			count++;
	    		else
   				break;
  
           	}

		free(rightc);
		free(leftc);

		if (count>0)
	      	{   
			if (strcmp(filedata[col], node->fullrow[col]) < 0)
       			{
        	    		node->left  = insert(node->left, col, filedata, maxCol, type);
      			}
        		else
        		{
       			        node->right = insert(node->right, col, filedata, maxCol, type);
        	       	}
		}
            	else 
	     	{
			if (strcasecmp(filedata[col], node->fullrow[col]) < 0)
       			{
            			node->left  = insert(node->left, col, filedata, maxCol, type);
      			}	
        		else
        		{
       	        		node->right = insert(node->right, col, filedata, maxCol, type);
               		}
	     	 }



	} 
	else // it is a float 
	{ 
		if (filedata[col][0] == ' ')
		{
			node->left  = insert(node->left, col, filedata, maxCol, type);
		}
		else if (node->fullrow[col][0] == ' ')
		{
        		node->right = insert(node->right, col, filedata, maxCol, type);   
		}
		else // both are actual int
		{
   			float new=atof(filedata[col]);
    			float inBST=atof(node->fullrow[col]);

			if (new < inBST)
        			node->left  = insert(node->left, col, filedata, maxCol, type);
    			else
        			node->right = insert(node->right, col, filedata, maxCol, type);   
		}

	} // float ends
 
 
    return node;
}


/*
	TO-DO:	
		walk entire directory					NOT-DONE
		fork for every .csv found				NOT-DONE
		open on filename					NOT-DONE
		check if valid csv					NOT-DONE
		save every pid generated				NOT-DONE
		output sorted file to filename-sorted-.csv		NOT-DONE
		add more functions to view code better			NOT-DONE
		add more flag for command line input			NOT-DONE
*/
			

int loadAndSort (char* buffer, int coltosorton)
{
	//SORTERARGS *sortingargs = (SORTERARGS *) sortingparam;
	//iterators
	int i = 0;
	int q = 0;
	int j = 0;

	//filechecks
	int linesize = 300;
	int stringsize = 50000;

	//file buffer
	char *holder;
	char *structinput = (char*) malloc (linesize*sizeof(char));
	char *tmp;

	//function declaration
	char* buildBuffer(int, char*, CSVPARAMS*);
	void printMetadata(int, int, int[]);
	int checkType(char*);
	int validateinput(char*);
	int checkInputString(char*, CSV input[], int argc, int cols);
	void populateDIR(char *name);
	void setBounds(char*, void*);


	

	i = 0;
	CSVPARAMS* csvparams = (CSVPARAMS *) malloc (sizeof(CSVPARAMS));
	//setBounds(buffer, csvparams);
	//int rows = csvparams->rows;
	//int cols = csvparams->cols;
	int rows = 5044;
	int cols = 28;

	maxColMain = cols;
	CSV input[rows];
	int type[cols];

	for (i = 0; i < cols; i++)
	{
		type[i] = 0;
	}
	for (i = 0; i < rows; i++)
	{
		input[i].filedata = (char**) malloc(sizeof(char*) * cols);
	}
	/*
		Parsing the entire character buffer we made earlier
		Line = currect line of the csv = current input[i] such that the input array resembles the csv input
		structline = the secondary array inside of the struct input creating a 2d "string" array
		ignore is a check for quotes and skips the split if a comma is detected inside a quote
		removewhite is a check to strip leading and trailing whitespace
	*/
	buildCSV(buffer, input, structinput, linesize, rows, cols);

	//get first line of data and assign a value to each one, 0 for string, 1 for int, 2 for float
	for (i = 0; i < cols; i++)
	{
		type[i] = checktype(input[1].filedata[i]);
	}

	//mergeSort(input, 1, rows - 1, coltosorton, type[coltosorton]);
	//printSorted(input, rows, cols, "./test.csv");
	pthread_mutex_lock(&lockBST);
	int r=0; // row index
	//BST = insert(BST, coltosorton, input[1].filedata, cols, type[coltosorton]);
	for (r=1; r<rows; r++)
	{	

		BST = insert(BST, coltosorton, input[r].filedata, cols, type[coltosorton]);
	}
	
	pthread_mutex_unlock(&lockBST);
	//Give back the memory so the OS doesn't hate me
 
	// had to delete to make so the GLOBAL BST not give garbage
	// we have to either find a different way to make a Global BST or free memory;	
	return 1;

}
/*
	FUNCTION NAME: checktype	

	PURPOSE:
		Return a value to define the type of data passed to it. Determines whether the char* given to the function is a string,
		float or int value.
	
	PARAMETERS:
	name		type			value/reference		description
	----------------------------------------------------------------------------
	test		char*			value			char* to be defined as int string or float
	
	RETURN VALUE: int

	CALLED FROM: main

	METHOD: The following is pseudocode describing how the function works.
		1. Check if the char* has any letters in it
			1a. if it contains a letter it must be a string and return 0;
		2. If no letters are found it must be an int or a float
			2a. if it contains a decimal it must be a float
			2b. if not it must be an int
		return  0 for string
			1 for int
			2 for float
	
*/
int checktype (char *test)
{
	int check = 0;
	int i = 0;
	for (i = 0; i < strlen(test); i++)
	{
		if (isalpha(test[i]))
		{
			//if any letter is found, define it as a string
			return 0;
		} else {
			if (test[i] == '.')
			{
				//if a decimal is found mark it as a possible float
				check = 1;
			}
		}
	}

	/*
		Should the method reach this if statement, no letters were found in the string
		therefor it must be an int or a float, if a decimal was found earlier
		it must be a float, if not it must be an int
	*/

	if (check == 1) 
	{
		return 2;
	} else {
		return 1;
	}
	return 0;
}
int checkInputString (char *colname, CSV input[], int argc, int cols)
{
	//Check if user input sorting string matchs any column from the given csv
	int i = 0;
	int j = 0;
	int acceptedargs = -1;
	for (i = 0; i < cols; i++)
	{
		//printf("%s : %s\n", colname, input[0].filedata[j]);
		if (strcmp(colname, input[0].filedata[i]) == 0)
		{
			acceptedargs = i;
		}
	}
	return acceptedargs;
}

void setBounds (char *holdera, void *inputstruct)
{
	CSVPARAMS *csvparams = (CSVPARAMS*) inputstruct;
	int rows = 0;
	int cols = 0;
	int firstline = 0;
	int ignore = 0;
	int i = 0;
	for (i = 0; i < strlen(holdera); i++)
	{
		if (holdera[i] == '\n')
		{
			firstline = 1;
			rows++;
		}
		if (holdera[i] == '"' && ignore == 0)
		{
			ignore = 1;
		} else if (holdera[i] == '"' && ignore == 1)
		{
			ignore = 0;
		}
		if (holdera[i] == ',' && ignore == 0 && firstline == 0)
		{
			cols++;
		}
	}
	cols++;
	csvparams->rows = rows;
	csvparams->cols = cols;
}

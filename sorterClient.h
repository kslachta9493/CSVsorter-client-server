#ifndef _sorterClient_h
#define _sorterClient_h

typedef struct listInfo{
	char* name;
	int isChild;
	char* field;
}listInfo;

typedef struct sortInfo{
	 char* name;
	char* field;
}sortInfo;

int check_csv_file(char *fileName);

int check_valid_csv(char *fileName);

int hostname_to_ip(char * hostname , char* ip);

void* sendF(void* filename);

void* listdir(void* linfO);

int findColIndex(int argc, char** argv);

int findDirIndex(int argc, char** argv);

int findOutIndex(int argc, char** argv);

int findHostIndex(int argc, char** argv);

int findPortIndex(int argc, char** argv);

typedef struct pidList{
	
	pthread_t tid;
	
	struct pidList* next;
}pidList;
#endif

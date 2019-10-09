#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h> //hostent
#include <arpa/inet.h>
#include "sorterClient.h"

char* targetDir;
int PORT;
char* HOST;


//Function to trim out the white space
char* trim(char *s){
    // Trim leading space
    while(isspace(*s) != 0) {
        s++;
    }

    //All spaces
    if(*s == 0)  
        return s;

    // Trim trailing space
    //Need tail to trim trailing white space
    char *trail = s + strlen(s) - 1;

    while((trail > s) && (isspace(*trail) != 0)) {
        trail--;
    }

    //Null terminate
    *(trail+1) = '\0';

    return s;
}

int check_csv_file(char *fileName) {
		//printf("path in check check csv: %s\n", fileName);
	char c[5];
	if(strlen(fileName) >= 4){
		memcpy(c, &fileName[strlen(fileName)]-4, 4);
			c[4] = '\0';
			//printf("CCCCCCC %s\n", c);
		//printf("STRCMP %d\n", strcmp(c, ".csv"));
		if(strcmp(c, ".csv")== 0) {
			if(check_valid_csv(fileName) == 1){
				//printf("FOUND LEGIT CSV!!%s\n", fileName);
    				return 1;
			}
		}
	}	
					//printf("Finish sa check csv\n");
	return 0;
}

int check_valid_csv(char *fileName) {
		//printf("check valid csv filename: %s \n",fileName);
    FILE *f;
	f = fopen(fileName, "r");
    char* input = (char*) malloc(sizeof(char)*600);
    char delims[] = ",";
    char *fields[] = {"color", "director_name", "num_critic_for_reviews", "duration", "director_facebook_likes", "actor_3_facebook_likes","actor_2_name", "actor_1_facebook_likes", "gross", "genres", "actor_1_name", "movie_title", "num_voted_users","cast_total_facebook_likes","actor_3_name", "facenumber_in_poster", "plot_keywords", "movie_imdb_link","num_user_for_reviews","language","country", "content_rating", "budget", "title_year", "actor_2_facebook_likes", "imdb_score", "aspect_ratio", "movie_facebook_likes"}; 
    char *token;
    fgets(input, 600, f);
    input = trim(input);
    int count = 0;

    while((token = strsep(&input, delims)) != NULL){
		//printf("COUNT %d TOK %s\n", count, token);
		if(count > 27){
			free(input);
			//printf("INVALID CSV\n");
			return 0;
		}
        if(strcmp(token, fields[count]) == 0){
            count++;
		}
		else{
			//printf("IMPROPER MATCH\n");
			//free(input);
			return 0;
		}
    }
   // printf("after while loop\n");
    if(strstr(fileName, "-sorted") == NULL && count == 28){
    //printf("in if statement\n");
		free(input);
		fclose(f);
    //printf("return 1\n");
        return 1;
    }
    //printf("after if loop\n");
	free(input);

    return 0;
}

int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
      //   printf("hello\n");
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
          //printf("goodbye\n");
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
     
    return 1;
}

void* sendF(void* info){
	//printf("child %d\n", pthread_self());
   // printf("ins senf\n");
	struct 	sortInfo* sinfo = (struct sortInfo*) info;
    char *fields[] = {"color", "director_name", "num_critic_for_reviews", "duration", "director_facebook_likes", "actor_3_facebook_likes","actor_2_name", "actor_1_facebook_likes", "gross", "genres", "actor_1_name", "movie_title", "num_voted_users","cast_total_facebook_likes","actor_3_name", "facenumber_in_poster", "plot_keywords", "movie_imdb_link","num_user_for_reviews","language","country", "content_rating", "budget", "title_year", "actor_2_facebook_likes", "imdb_score", "aspect_ratio", "movie_facebook_likes"};

	int i = 0;
	for( i = 0; strcmp(fields[i],sinfo->field)!= 0; i++){
	}
    //  printf("in sendf A\n");
	int field = i;
	char* fieldS;
	fieldS = (char *) malloc (sizeof(char)*100);
	char x[100];
	//      printf("in sendf B\n");
	snprintf(x, 100, "%d", i);
	     // printf("in sendf C\n");
	strcpy(fieldS, x);

	char* sort = malloc(500 * sizeof(char));
	//printf("in sendf D fieldS: %s\n", fieldS);
	strcpy(sort,"nowsh ");
	//printf("in sendf E\n");
	strcat(sort,fieldS);
	//printf("in sendf F\n");
	char* filename = sinfo->name;
	int fd = open(sinfo->name, O_RDONLY);
	struct stat stat_buf;

	//SEND FILE!!!!!!
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
     // printf("in sendf A\n");
	if (fd == -1) {
      fprintf(stderr, "unable to open '%s': %s\n", filename, strerror(errno));
      exit(1);
    }
	//printf("in sendf G\n");
    fstat(fd, &stat_buf);
	char sizeholder[100];
		sprintf(sizeholder, "%d", stat_buf.st_size);
	strcat(sort, " ");
	strcat(sort, sizeholder);
	//printf("SORT LINE SEND: %s\n", sort);
    char buffer[200000] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

	char ip[100];
       	//printf("in sendf k\n");
	hostname_to_ip(HOST, ip);
       //	printf("in sendf o\n");
    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
    }

	int fileSent;
	off_t offset = 0;
	
	//send(sock, sort, strlen(sort) , 0 );
	//printf("sent sort\n");
	send(sock, (char*) sort, strlen(sort) , 0 );
	int n =0;
	char buffer1[256];
	bzero(buffer1, 256);

	n = read(sock, buffer1, 255);

	if (n < 0) 
	{
		error("ERROR reading from socket");
	}
	//printf("%s\n",buffer1);
	//printf("in sendf I\n");
	//send(sock, field, strlen(field), 0);
	
    fileSent = sendfile (sock, fd, &offset, stat_buf.st_size);

	if (fileSent == -1) {
      fprintf(stderr, "error from sendfile: %s\n", strerror(errno));
      exit(1);
    }    
	//printf("going to wait\n");
    valread = read( sock , buffer, 200000);
  //  if (strcmp(buffer, "done") == 1){		
//	}
	//printf("gets back %d\n",valread);
	//printf("gets back also: %s\n",buffer);
	free(sinfo);

	return 0;
	
}



void* listdir(void* linfO) {
	//printf("enters listdir\n");
	struct listInfo* linfo = (struct listInfo*) linfO;
	struct pidList* pidHead = NULL;
	struct pidList* pids;
	sortInfo* sinfo = malloc(sizeof(sinfo));
	pids = NULL;
	struct pidList* temp;
    DIR *dir;
    struct dirent *entry;
    char *name = linfo->name;
    int isChild = linfo->isChild;
    char* field = linfo->field;
	//printf("in directory %s\n", name);
    if ((dir = opendir(name)) == 0){
        return;
	}
	
	int k = 1;
	int status;

	int pid;
	if(isChild == 0){
		//printf("Initial TID: %d\n", getpid());
		//printf("Initial TID: %lu\n", pthread_self());
		//printf("Initial TID again: %d\n", pthread_self());
		//printf("TIDS of all child processes: ");
		linfo->isChild = 1;
		fflush(stdout);
	}
	else{
		//printf("%lu,", pthread_self());
		fflush(stdout);
	}
	//printf("\nenters while loop \n");
	while ((entry = readdir(dir)) != NULL) {
		//char path[1024];
		char* path = malloc(sizeof(char)*5000);
		//linfo = malloc(sizeof(struct listInfo));
		/*
		if(isChild){
			printf("------------file in child %s: %s k = %d\n",name,entry->d_name,k);
		}
		*/
		if (entry->d_type == DT_DIR) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
				//printf("coontinuing!!+!__)_)\n");
		        continue;
			}

			if(strcmp(name,".") ==0){
				//printf("name is .\n");
				strcpy(path,"./");
				//strcat(path,name);
			} else{
				strcpy(path,name);
				//strcat(path,"/");
			}

			if(path[strlen(path)-1] != '/'){
				strcat(path,"/");
			}
			//printf("entry name adding %s\n",entry->d_name);
			strcat(path,entry->d_name );
			strcat(path,"/" );
			//path[0] = '/';
			//printf("path = %s\n",path);
			//printf("forks to %s\n", path);
			//printf("forks directory: %s child pid =%d\n",path, getpid());
			//printf("%d,", getpid());
			linfo->name = path;
			//printf("path!!!!!: %s \n",path);
	    	//listdir(linfo);
	    	if(pids == NULL){
				pids = malloc(sizeof(struct pidList));
				//pids->tid = malloc(sizeof(pthread_t));
				pidHead = pids;
			} else{
				temp = malloc(sizeof(struct pidList));
				pids->next = temp;
				pids = temp;
	    		//pids->tid = malloc(sizeof(pthread_t));
			}	
			
			pids->tid = (pthread_t)malloc(sizeof(pthread_t));
			pids->next = NULL;
			pthread_create(&(pids->tid),NULL,listdir,linfo); //NOT SURE WHAT TID IS 
			path = malloc(sizeof(char)*5000);
			linfo = malloc(sizeof(struct listInfo));
			linfo->isChild = 1;
			linfo->field = field;
			
		} 

		else {
			//printf("CSV FOUND!!!!!!\n\n");
				if(isChild){
					strcpy(path,name);
					strcat(path,entry->d_name);
				}
				else{
					if(name[0] != '.'){
						strcpy(path,name);
						strcat(path,"/");
						strcat(path,entry->d_name);
					}
					else{
						strcpy(path,entry->d_name);
					}
				}
				if(check_csv_file(path) == 1){
			
					if(pids == NULL){
						pids = malloc(sizeof(pidList));
						//pids->tid = malloc(sizeof(pthread_t));
						pidHead = pids;
					}
					else{
						temp = malloc(sizeof(pidList));
						pids->next = temp;
						pids = temp;
				    	//pids->tid = malloc(sizeof(pthread_t));
					}	
					pids->tid = (pthread_t)malloc(sizeof(pthread_t));
					pids->next = NULL;
					//printf("%d,", getpid());
					//sort(path, field);
					sinfo->field = field;
					sinfo->name = path;
					path = malloc(sizeof(char)*5000);
					//printf("path for filesort: %s\n", path);
					//printf("about to create thread\n");
					pthread_create(&(pids->tid),NULL,sendF,sinfo); //NOT SURE WHAT TID IS
					sinfo = malloc(sizeof(sinfo));
				
					//printf("creates new thread\n"); 
					//exit(0); // notsure if exit needs anything
				}
	    	}
	//printf("reading directory\n");
    }
	//printf("done reading directory\n");
	if(isChild){
		//printf("child thread collecting of other threads\n");
		while(pidHead != NULL){
			pthread_join(pidHead->tid,NULL);
			pidHead = pidHead->next;
		}
		//exit(0);
	}
	
	else{
		//printf("End of main thread. pidHead\n");
		while(pidHead != NULL){
			//printf("main thread collecting of other threads. pidhead tid = %d\n", pidHead->tid);
			pthread_join(pidHead->tid,NULL);
		   // printf("done joining\n");
			pidHead = pidHead->next;
		   // printf("done nexting\n");
		}
		//need to actually receive the file now lol
		//wait for "done"
	    //printf("done collecting\n\n\n");
		struct sockaddr_in address;
	    int sock = 0, valread;
	    struct sockaddr_in serv_addr;
	    //printf("Hello from client\n");
	    char buffer[1024] = {0};
		
	    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	    {
	        printf("\n Socket creation error \n");
	        return ;
	    }
  		//printf("socket did something\n");
	    memset(&serv_addr, '0', sizeof(serv_addr));
        		//printf("serv addr something pre\n");
	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_port = htons(PORT);
      		//printf("serv addr something after\n");
		char ip[100];

		hostname_to_ip(HOST, ip);
		
	    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0) 
    	{
    	    printf("\nInvalid address/ Address not supported \n");
    	    return;
    	}
  
    	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    	{
    	    printf("\nConnection Failed \n");
    	    return;
    	}
		//printf("about to send dump request\n");
   		send(sock , "dump" , strlen("dump") , 0 );
	    //recv(sock, buffer, 1024, 0);
	int read_size = 0;
	int maxsize = 900000;
	char server_message[maxsize];
	memset(server_message, 0, maxsize);
	printf("RECIEVING NOW\n");

   	char *fields[] = {"color", "director_name", "num_critic_for_reviews", "duration", "director_facebook_likes", "actor_3_facebook_likes","actor_2_name", "actor_1_facebook_likes", "gross", "genres", "actor_1_name", "movie_title", "num_voted_users","cast_total_facebook_likes","actor_3_name", "facenumber_in_poster", "plot_keywords", "movie_imdb_link","num_user_for_reviews","language","country", "content_rating", "budget", "title_year", "actor_2_facebook_likes", "imdb_score", "aspect_ratio", "movie_facebook_likes"};

	int ff = 0;
	for( ff = 0; strcmp(fields[ff],field)!= 0; ff++){
	}
	FILE* out;
	char outfile[100];
	memset(outfile, 0, 100);
	strcat(outfile, targetDir);
	strcat(outfile, "/");
	strcat(outfile, "All-sorted-");
	strcat(outfile, fields[ff]);
	strcat(outfile, ".csv");
	out = fopen(outfile, "w");
	int i = 0;
	for (i = 0; i < 28; i++)
	{
		if (i == 27)
			fprintf(out, "%s\n" ,fields[i]);
		else
			fprintf(out, "%s," ,fields[i]);
	}
	while( (read_size = recv(sock , server_message , maxsize, 0)) > 0 )
	{
		if (strcmp(server_message, "finished") == 0)
		{
			break;
		} else
		{
			fprintf(out, "%s", server_message);
			fflush(out);
			memset(server_message, 0, maxsize);
		}
	}
	fflush(out);
	fclose(out);
	printf("RECIEVED FILE\n");
    	printf("%s\n",buffer);

			
	}
	//printf("close main thread");
	closedir(dir);
}

int findColIndex(int argc, char** argv){
	int x = 0;
	while ((x < argc) && (strcmp(argv[x], "-c") != 0)) {
		x++;
	}
	return x+1;
}

int findDirIndex(int argc, char** argv){
	int x = 0;
	while ((x < argc) && (strcmp(argv[x], "-d") != 0)) {
		x++;
	}
	return x+1;
}

int findOutIndex(int argc, char** argv){
	int x = 0;
	while ((x < argc) && (strcmp(argv[x], "-o") != 0)) {
		x++;
	}
	return x+1;
}

int findHostIndex(int argc, char** argv){
	int x = 0;
	while ((x < argc) && (strcmp(argv[x], "-h") != 0)) {
		x++;
	}
	return x+1;
}

int findPortIndex(int argc, char** argv){
	int x = 0;
	while ((x < argc) && (strcmp(argv[x], "-p") != 0)) {
		x++;
	}
	return x+1;
}

int main(int argc, char** argv) {

    //Search Directory and Destination Directory
	DIR* sdir;
	DIR* ddir;
	
	int c = findColIndex(argc, argv);
	int d = findDirIndex(argc, argv);
	int o = findOutIndex(argc, argv);
	int h = findHostIndex(argc, argv);
	int p = findPortIndex(argc, argv);

	struct listInfo* details = (struct listInfo*)malloc(sizeof(struct listInfo));

	if(argc == 11){
		if(c < argc && d < argc && o < argc && h < argc && p < argc){
			PORT = atoi(argv[p]);
			HOST = argv[h];
			details->field = argv[c];
			details->name = argv[d];
			targetDir = argv[o];
			listdir(details);
		} else {
			printf("Invalid\n");
		}
	} else if(argc == 9){
		if(c < argc && h < argc && p < argc && d < argc) {
			PORT = atoi(argv[p]);
			HOST = argv[h];
			details->field = argv[c];
			details->name = argv[d];
			targetDir = ".";
			listdir(details);
		} else if(c < argc && h < argc && p < argc && o < argc) {
			PORT = atoi(argv[p]);
			HOST = argv[h];
			details->field = argv[c];
			details->name = ".";
			targetDir = argv[o];
			listdir(details);
		} else {
			printf("Invalid\n");
		}
	} else if(argc == 7){
		if(c < argc && h < argc && p < argc) {
			PORT = atoi(argv[p]);
			HOST = argv[h];
			details->name = ".";
			details->field = argv[c];
			targetDir = ".";
			listdir(details);
		} else {
			printf("Invalid\n");
		}
	} else {
		//printf("Why is this here\n");
	    printf("Invalid\n");
	}
	return 0;
}

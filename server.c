#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <fcntl.h>
#include <sys/stat.h>

#include "sorter.h"
#include "builder.h"
 
//the thread function
void *connection_handler(void *);

extern struct node *BST;
extern int maxColMain;
int usersocket;
void checkSocket(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Must use -p socketnumber\n");
		exit(-1);
	}
	if (strcmp(argv[1], "-p"))
	{
		printf("Invalid flag %s\n", argv[1]);
		exit(-1);
	}
	usersocket = atoi(argv[2]);

}
int main(int argc , char *argv[])
{
	checkSocket(argc, argv);
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
	pthread_mutex_t lockTD;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
	exit(-1);
    }
    //puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(usersocket);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
   }
   // puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
    
     
    //Accept and incoming connection
   // puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
       // puts("Connection accepted");
	printf("%s , ", inet_ntoa(client.sin_addr));
         fflush(stdout);
	//pthread_mutex_lock(&lockTD);
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
	//pthread_mutex_unlock(&lockTD);
       // puts("Handler assigned");
    }
     
    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 

void *connection_handler(void *socket_desc)
{

   	int sock = *(int*)socket_desc;
   	int read_size;
	int maxsize = 3000000;
	int sort = 0;
	int num = 0;
	int notdone = 1;
	int len = 0;
   	char *message , client_message[maxsize];
	char output[maxsize];
	memset(output, 0, maxsize);
	int totalrecieved = 0;
    while( (read_size = recv(sock , client_message , maxsize, 0)) > 0 )
    {

		client_message[read_size] = '\0';
		//printf("Received %d\n", strlen(client_message));
		if (strstr(client_message, "nowsh"))
		{
			char *token;
			token = strtok(client_message, " ");
			//printf("TOKEN1 %s\n", token);
			token = strtok(NULL, " ");
			//printf("TOKEN2 %s\n", token);
			num = atoi(token);
			token = strtok(NULL, " ");
			//printf("TOKEN3 %s\n", token);
			len = atoi(token);


			//printf("COL %d LEN %d\n", num);
			//puts("SORT REQUEST");
			fflush(stdout);
			sort = 1;
        		write(sock , client_message , strlen(client_message));
		} else if (strcmp(client_message, "dump\0") == 0)
		{
			//printf("Dump Requested\n");
			FILE* fp;
			fp = fopen("n7n7n.txt", "w");
			printBST(BST, maxColMain, fp);
			BST = NULL;
			fflush(fp);
			fclose(fp);
			int fd;
			struct stat stat_buff;

			fd = open("n7n7n.txt", O_RDONLY);
			fstat(fd, &stat_buff);
			int test = 0;
			test = sendfile(sock, fd, NULL, stat_buff.st_size);
			if (test < 0)
			{
				printf("FAILED SENDINGb\n");
				close(sock);
				exit(-1);
			}
			else
			{	
				//printf("SENT FINISHED\n");
				//send(sock, "finished", 10, 0);
				close(sock);
				//send(sock, NULL, 0, 0);
				return 0;
			}
			//write(sock, "printing\0", 9); 
		}
		else 
		{
			//puts(client_message);
			if (totalrecieved < len && sort == 1)
			{		

				totalrecieved += strlen(client_message);
				//printf("TOTAL: %d\n", totalrecieved);
				strcat(output, client_message);
				//printf("Added\n");
				
			}
			if (totalrecieved >= len && sort == 1)
			{
				//printf("Sorting Now\n");
				loadAndSort(output, num);
				//printf("Sorting Complete - Done Sent\n");				
				//puts(client_message);
				sort = 0;
				len = 0;
        			write(sock , "done\0" , 5);
				memset(output, 0, maxsize);
			}
			//puts(client_message);
		}

		
		//clear the message buffer
		memset(client_message, 0, maxsize);
    }

    if(read_size == 0)
    {
        //puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 

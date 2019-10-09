#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main (int argc, char* argv[])
{
	int sockfd = -1;
	int portno = -1;
	int n = 0;
	struct sockaddr_in serverAddressInfo;
	struct hostent * serverIPAddress;
	
	char buffer[256];

	portno = atoi(argv[2]);

	serverIPAddress = gethostbyname(argv[1]);

	if (serverIPAddress == NULL)
	{
		printf("ERROR, no such host\n");
		exit(0);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		error("ERROR CREATING SOCKET\n");
	}
	
	bzero((char*) &serverAddressInfo, sizeof(serverAddressInfo));
	serverAddressInfo.sin_family = AF_INET;
	serverAddressInfo.sin_port = htons(portno);
	
	bcopy((char*) serverIPAddress->h_addr, (char*) &serverAddressInfo.sin_addr.s_addr, serverIPAddress->h_length);

	if (connect(sockfd, (struct sockaddr *) &serverAddressInfo, sizeof(serverAddressInfo)) < 0)
	{
		error("ERROR CONNECTING");
	}


	int sc = 0;
	int sz = 0;
	int test = 0;
	char col[2] = "1";
	off_t offset = 0;
	char sort[50] = "nowshad 1\0";
	char dump[5] = "dump";
	char fin[14] = "finished";
	int maxsize = 900000;
	char sizeholder[50];
	char server_message[maxsize];
	memset(server_message, 0, maxsize);
	struct stat stat_buff;

	int fp;
	fp = open("movie.csv", O_RDONLY);
	fstat(fp, &stat_buff);
	strcat(sort, " ");
	sprintf(sizeholder, "%d", stat_buff.st_size);
	strcat(sort, sizeholder);
	printf("Starting Up...%s\n", sort);
	test = send(sockfd, (char*)sort, strlen(sort), 0);
	if (test < 0)
	{
		printf("FAILED SENDINGa\n");
		close(sockfd);
		exit(-1);
	}
	else
	{
		printf("SENT: %d\n", test);
	}


	bzero(buffer, 256);

	n = read(sockfd, buffer, 255);

	if (n < 0) 
	{
		error("ERROR reading from socket");
	}
	printf("%s\n",buffer);
	

	printf("size %d\n", stat_buff.st_size);

	test = sendfile(sockfd, fp, NULL, stat_buff.st_size);
	if (test < 0)
	{
		printf("FAILED SENDINGb\n");
		close(sockfd);
		exit(-1);
	}
	else
	{	
		printf("SENT: %d\n", test);
	}


	close(fp);

	bzero(buffer, 256);

	n = read(sockfd, buffer, 255);

	if (n < 0) 
	{
		error("ERROR reading from socket");
	}
	printf("%s\n",buffer);
////////////SECOND SEND

	if (strcmp(buffer, "done") == 0)
		printf("DONE SORTING\n");
	else
		printf("WAT\n");
//////////////////////////////

	test = send(sockfd, (char*)dump, sizeof(char) * 5, 0);
	if (test < 0)
	{
		printf("FAILED SENDINGa\n");
		close(sockfd);
		exit(-1);
	}
	else
	{
		printf("SENT DUMP: %d\n", test);
	}
	int read_size = 0;
	printf("RECIEVING NOW\n");
	FILE* out;
	out = fopen("rec.csv", "w");
	while( (read_size = recv(sockfd , server_message , maxsize, 0)) > 0 )
	{
		if (strcmp(server_message, "finished") == 0)
		{
			break;
		} else
		{
			fprintf(out, "%s", server_message);
			memset(server_message, 0, maxsize);
		}
	}
	fflush(out);
	fclose(out);
	printf("RECIEVED FILE\n");
	puts(server_message);	

	close(fp);
	close(sockfd);
	return 0;	


}

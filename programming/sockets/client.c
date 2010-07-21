#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];

	if (argc < 3) {
		fprintf(stderr, "ERROR, no host port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	portno = atoi(argv[2]);
	server = gethostbyname(argv[1]);

	serv_addr.sin_family = AF_INET;
	memcpy(&serv_addr.sin_addr.s_addr,
			server->h_addr,
			server->h_length);
	serv_addr.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("connect");
	printf("Message: ");
	memset(buffer, 0, 256);
	fgets(buffer, 255, stdin);

	n = write(sockfd, buffer, strlen(buffer));
	if (n < 0)
		error("write");

	close(sockfd);

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	unsigned int clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	int pid;

	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	signal(SIGCHLD, SIG_IGN);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	portno = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0)
		error("bind");
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	for(;;) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0)
			error("newsockfd");

		pid = fork();
		if (pid == 0) {
			close(sockfd);

			memset(buffer, 0, 256);
			n = read(newsockfd, buffer, 255);
			if (n < 0)
				error("read");
			close(newsockfd);

			printf("Message %s\n", buffer);
			exit(0);
		}
		else
			close(newsockfd);
	}

	return 0;
}

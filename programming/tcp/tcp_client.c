#include <stdio.h>		/* io library */
#include <sys/socket.h>		/* socket (), connect (), send (), recv () */
#include <arpa/inet.h>		/* sockaddr_in, inet_addr () */
#include <string.h>		/* memset () */
#include <unistd.h>		/* close () */

#define RECVBUFFER 4096		/* buffer receive size */

int main (int argc, char **argv)
{
	int sock;
	char *server_host;
	struct sockaddr_in server_sock;
	unsigned short server_port;
	char *request = "GET / HTTP/1.0\nHost: www.uol.com.br\n\n";
	char buffer[RECVBUFFER];
	int bytes_recv;

	if (argc != 3)
	{
		fprintf (stderr, "Usage: %s <server> <port>\n", argv[0]);
		return 1;
	}

	/* if socket < 0, error */
	sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf ("%d\n", sock);

	server_host = argv[1];
	server_port = atoi (argv[2]);
	memset (&server_sock, 0, sizeof (server_sock));
	server_sock.sin_family		= AF_INET; /* Internet address familiy */
	server_sock.sin_addr.s_addr	= inet_addr (server_host);
	//server_sock.sin_addr.s_addr	= gethostname (server_host, sizeof (server_host));
	server_sock.sin_port		= htons (server_port);

	/* if connect < 0, error */
	printf ("%d\n",
	connect (sock, (struct sockaddr *) &server_sock, sizeof (server_sock))
	);

	send (sock, request, strlen (request), 0);
	do
	{
		bytes_recv = recv (sock, buffer, RECVBUFFER, 0);
		printf ("%s", buffer);
	} while (bytes_recv == RECVBUFFER);

	return 0;
}

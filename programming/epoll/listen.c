#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_EVENTS 10

int main (int argc, char **argv)
{
	struct epoll_event ev, events[MAX_EVENTS];
	struct sockaddr_in server_addr, client_addr;
	int sock, conn_sock, epollfd, nfds;
	int n;

	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
		perror ("socket");
		return EXIT_FAILURE;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons (5000);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero (&(server_addr.sin_zero), 8);
	if (bind (sock, (struct sockaddr*) &server_addr, sizeof (struct sockaddr))) {
		perror ("bind");
		return EXIT_FAILURE;
	}

	if (listen (sock, 5) == -1) {
		perror ("listen");
		return EXIT_FAILURE;
	}

	epollfd = epoll_create (MAX_EVENTS);
	if (epollfd == -1) {
		perror ("epoll_create");
		return EXIT_FAILURE;
	}

	ev.events = EPOLLIN;
	ev.data.fd = sock;
	if (epoll_ctl (epollfd, EPOLL_CTL_ADD, sock, &ev) == -1) {
		perror ("epoll_ctl: sock");
		return EXIT_FAILURE;
	}

	for (;;) {
		nfds = epoll_wait (epollfd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			perror ("epoll_wait");
			return EXIT_FAILURE;
		}
		for (n = 0; n < nfds; ++n) {
			if (events[n].data.fd == sock) {
				conn_sock = accept (sock,
					(struct sockaddr *) &client_addr, sizeof (struct sockaddr_in));
				if (conn_sock == -1) {
					perror ("accept");
					exit (EXIT_FAILURE);
				}
				//setnonblocking(conn_sock);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
					perror("epoll_ctl: conn_sock");
					exit(EXIT_FAILURE);
				}
			} else {
				//do_use_fd(events[n].data.fd);
			}
		}
	}

	close (epollfd);
	return 0;
}

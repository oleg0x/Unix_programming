#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define BUF_SIZE 100
#define SOCKETNAME "my_socket"



int main()
{
	struct sockaddr_un sa;
	sa.sun_family = AF_LOCAL;				// == AF_UNIX
	strcpy(sa.sun_path, SOCKETNAME);

	int fd_skt = socket(AF_LOCAL, SOCK_STREAM, 0);
	connect(fd_skt, (const struct sockaddr *)&sa, sizeof(sa));
	puts("Client has started...");

	char buf[BUF_SIZE];
	write(fd_skt, "Some message from client", 24);
	read(fd_skt, buf, sizeof(buf));
	printf("Got message: %s\n", buf);

	close(fd_skt);
}

// gcc -std=c17 -Wall -Wextra -o skt_local_client skt_local_client.c
// Run server first

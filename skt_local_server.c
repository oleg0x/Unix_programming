#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define BUF_SIZE 100
#define SOCKETNAME "my_socket"
#define QUEUE_LEN 1024
// Max value in the file /proc/sys/net/core/somaxconn, equals 4096



int main()
{
	unlink(SOCKETNAME);

/*	struct sockaddr_un {
		sa_family_t sun_family;				// Address family
		char sun_path[];					// Socket pathname
       }; */
	struct sockaddr_un sa;
	sa.sun_family = AF_LOCAL;				// == AF_UNIX
	strcpy(sa.sun_path, SOCKETNAME);

	int fd_skt = socket(AF_LOCAL, SOCK_STREAM, 0);
	bind(fd_skt, (const struct sockaddr *)&sa, sizeof(sa));
	listen(fd_skt, QUEUE_LEN);
	puts("Server has started...");
	int fd_client = accept(fd_skt, NULL, NULL);		// Return a fd for the accepted socket

	char buf[BUF_SIZE];
	read(fd_client, buf, sizeof(buf));
	printf("Got message: %s\n", buf);
	write(fd_client, "Some message from server", 24);

	close(fd_skt);
	close(fd_client);
}

// gcc -std=c17 -Wall -Wextra -o skt_local_server skt_local_server.c

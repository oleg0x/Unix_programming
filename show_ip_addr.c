#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>



/* struct addrinfo
{
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	socklen_t ai_addrlen;
	struct sockaddr *ai_addr;
	char *ai_canonname;
	struct addrinfo *ai_next;
}; */



void print_addr_info(const char *node)
{
	struct addrinfo hint;
	memset (&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;		// Return socket addresses for any address family
	hint.ai_socktype = 0;			// Return socket addresses of any type
	hint.ai_protocol = 0;			// Return socket addresses with any protocol

	struct addrinfo *res = NULL;
	int r = getaddrinfo(node, "80"/*NULL*/, &hint, &res);
	if ( r != 0 )
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
		exit(EXIT_FAILURE);
	}

	char addr_str[INET6_ADDRSTRLEN];
	in_port_t port;					// in_port_t == uint16_t
	puts(node);
	for ( struct addrinfo *aip = res; aip != NULL; aip = aip->ai_next )
	{
		if ( aip->ai_family == AF_INET )
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in*)aip->ai_addr;
			port = ntohs(ipv4->sin_port);
			inet_ntop(aip->ai_family, &ipv4->sin_addr, addr_str, INET_ADDRSTRLEN);
		}
		else if ( aip->ai_family == AF_INET6 )
		{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)aip->ai_addr;
			port = ntohs(ipv6->sin6_port);
			inet_ntop(aip->ai_family, &ipv6->sin6_addr, addr_str, INET6_ADDRSTRLEN);
		}
		printf("Address: %s:%d,\tFamily: %2d,  Type: %d,  Protocol: %2d\n",
			addr_str, port, aip->ai_family, aip->ai_socktype, aip->ai_protocol);
	}

	freeaddrinfo(res);
}



int main(int argc, char *argv[])
{
	if ( argc < 2 )
	{
		puts("Usage: show_ip_addr <hostname1> [hostname2]...");
		exit(-1);
	}

	printf("Families: AF_UNIX = %d, AF_LOCAL = %d, AF_INET = %d, AF_INET6 = %d\n",
		   AF_UNIX, AF_LOCAL, AF_INET, AF_INET6);
	printf("Types: SOCK_STREAM = %d, SOCK_DGRAM = %d, SOCK_RAW = %d\n",
		   SOCK_STREAM, SOCK_DGRAM, SOCK_RAW);

	for ( int i = 1; i < argc; ++i )
		print_addr_info(argv[i]);
}

// gcc -std=gnu17 -Wall -Wextra -o show_ip_addr show_ip_addr.c    (Note: gnu17 instead of c17)

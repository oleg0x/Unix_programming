#include "error_handling.h"

#define ERM_FILE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)



ssize_t write_all(int fd, const void *buf, size_t nbyte)
{
	ssize_t nwritten = 0, n;
	do {
		if ( (n = write(fd, &((const char *)buf)[nwritten], nbyte - nwritten)) == -1 )
		{
			if ( errno == EINTR )  continue;
			else  return -1;
		}
		nwritten += n;
	} while ( nwritten < nbyte );
	return nwritten;
}



ssize_t read_all(int fd, void *buf, size_t nbyte)
{
	ssize_t nread = 0, n;
	do {
		if ( (n = read(fd, &((char *)buf)[nread], nbyte - nread)) == -1 )
		{
			if ( errno == EINTR )  continue;
			else  return -1;
		}
		if ( n == 0 )  return nread;
		nread += n;
	} while ( nread < nbyte );
	return nread;
}



int main(void)
{
	int fd;
	const char buf1[] = "The quick brown fox jumps over the lazy dog";
	char buf2[50];

	EC_NEG1( fd = open("newfile.txt", O_CREAT | O_RDWR, ERM_FILE) )
	write_all(fd, buf1, 43);
	EC_NEG1( lseek(fd, 10, SEEK_SET) )		// Just after "The quick "
	read_all(fd, buf2, 9);					// "brown fox"
	puts(buf2);

	exit(EXIT_SUCCESS);
EC_CLEANUP_BEGIN
	exit(EXIT_FAILURE);
EC_CLEANUP_END
}

// gcc -std=c17 -Wall -Wextra write_read_all.c

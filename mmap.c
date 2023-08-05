#include "error_handling.h"
#include <sys/mman.h>



int main(void)
{
	int fd;
	EC_NEG1( fd = open("hobbit.txt", O_RDWR) )

	struct stat sb;
	EC_NEG1( fstat(fd, &sb) )			// To obtain file size

	off_t offset = 0;
	size_t length = sb.st_size - offset;

	// Create a mapping
	char *addr;
	addr = mmap(NULL,			// Starting address for the mapping, NULL for kernel choice
		length,					// Length of the mapping, in bytes, must be > 0
		PROT_READ | PROT_WRITE,	// Memory protection of the mapping
		MAP_SHARED,				// Visibility of updates
		fd,						// File  descriptor
		offset					// Offset in the file, a multiple of the page size
	);
	EC_NEG1( addr )
	close(fd);

	// Some work with the mapping
	puts(addr);
	for ( int i = 0; i < 20; ++i )
		addr[i] ^= 'X';
	EC_NEG1( msync(addr, length, MS_SYNC) )		// Flush  changes

	for ( int i = 50; i < 70; ++i )
		addr[i] ^= 'Y';

	EC_NEG1( munmap(addr, length) )				// Also flushes changes

	// Automatically unmapped when the process is terminated

	exit(EXIT_SUCCESS);
EC_CLEANUP_BEGIN
	exit(EXIT_FAILURE);
EC_CLEANUP_END
}

// gcc -std=c17 -Wall -Wextra mmap.c

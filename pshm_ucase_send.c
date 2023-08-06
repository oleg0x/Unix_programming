#include "pshm_ucase.h"
#include <string.h>



int main(void)
{
	const char *shmpath = "sh_mem_obj";
	const char *string = "The quick brown fox jumps over the lazy dog\n";
	size_t len = strlen(string);

	if ( len > BUF_SIZE )
	{
		fprintf(stderr, "String is too long\n");
		exit(EXIT_FAILURE);
	}

	// Open the existing shared memory object and map it into the caller's address space
	int fd = shm_open(shmpath, O_RDWR, 0);
	if ( fd == -1 )
		errExit("shm_open");

	struct shmbuf *shmp;
	shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE,
				MAP_SHARED, fd, 0);
	if ( shmp == MAP_FAILED )
		errExit("mmap");

	// Copy data into the shared memory object
	shmp->cnt = len;
	memcpy(&shmp->buf, string, len);

	// Tell peer that it can now access shared memory
	if ( sem_post(&shmp->sem1) == -1 )
		errExit("sem_post");

	// Wait until peer says that it has finished accessing the shared memory
	if ( sem_wait(&shmp->sem2) == -1 )
		errExit("sem_wait");

	// Write modified data in shared memory to standard output
	write(STDOUT_FILENO, &shmp->buf, len);

	exit(EXIT_SUCCESS);
}

// gcc -std=c17 -Wall -Wextra -o pshm_send pshm_ucase_send.c

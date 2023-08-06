#include "pshm_ucase.h"
#include <ctype.h>



int main(void)
{
	//Create shared memory object and set its size to the size of our structure
	const char *shmpath = "sh_mem_obj";
	int fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, 0600);
	if ( fd == -1 )
		errExit("shm_open");

	if ( ftruncate(fd, sizeof(struct shmbuf)) == -1 )
		errExit("ftruncate");

	//Map the object into the caller's address space
	struct shmbuf  *shmp;
	shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE,
				MAP_SHARED, fd, 0);
	if ( shmp == MAP_FAILED )
		errExit("mmap");

	// Initialize semaphores as process-shared, with value 0
	if ( sem_init(&shmp->sem1, 1, 0) == -1 )
		errExit("sem_init-sem1");
	if ( sem_init(&shmp->sem2, 1, 0) == -1 )
		errExit("sem_init-sem2");

	// Wait for 'sem1' to be posted by peer before touching shared memory
	if ( sem_wait(&shmp->sem1) == -1 )
		errExit("sem_wait");

	// Print and convert data in shared memory into upper case
	write(STDOUT_FILENO, &shmp->buf, shmp->cnt);
	for ( size_t j = 0; j < shmp->cnt; ++j )
		shmp->buf[j] = toupper((unsigned char)shmp->buf[j]);

	// Post 'sem2' to tell the peer that it can now access the modified data in shared memory
	if ( sem_post(&shmp->sem2) == -1 )
		errExit("sem_post");

	shm_unlink(shmpath);		// Unlink the shared memory object.
	// Even if the peer process is still using the object, this is okay.
	// The object will be removed only after all open references are closed.

	exit(EXIT_SUCCESS);
}

// gcc -std=c17 -Wall -Wextra -o pshm_bounce  pshm_ucase_bounce.c

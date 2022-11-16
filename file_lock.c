#include "error_handling.h"



#define MAX_TRIES 5
#define NAPLENGTH 2
#define LOCK_FILE "/tmp/lock"



bool lock(const char *full_name)
{
	EC_NULL(full_name)
	int fd, tries = 0;

	while ( (fd = open(full_name, O_WRONLY | O_CREAT | O_EXCL, 0)) == -1 &&
		errno == EEXIST )
	{
		if ( ++tries >= MAX_TRIES )
		{
			errno = EAGAIN;
			EC_CLEANUP
		}
		sleep(NAPLENGTH);
	}

	if ( fd == -1 )  EC_FAIL
	EC_NEG1(close(fd));
	return true;

EC_CLEANUP_BEGIN
	return false;
EC_CLEANUP_END
}



bool unlock(const char *full_name)
{
	EC_NULL(full_name)
	EC_NEG1(unlink(full_name))
	return true;

EC_CLEANUP_BEGIN
	return false;
EC_CLEANUP_END
}



void test_lock(void)
{
	for ( int i = 0; i < 4; ++i )
	{
		if ( lock(LOCK_FILE) )
		{
			printf("Process %ld set the lock\n", (long)getpid());
			sleep(rand() % 5 + 1);		// Some work here
			EC_FALSE(unlock(LOCK_FILE))
		}
		else
		{
			if ( errno == EAGAIN )
				printf("Process %ld couldn't set the lock\n", (long)getpid());
			else
				EC_FAIL					// Something bad happens
		}
		sleep(rand() % 5 + 5);
	}
	return;

EC_CLEANUP_BEGIN
EC_CLEANUP_END
}



int main(void)
{
	test_lock();
}

// Compile: $ gcc -std=c17 -Wall -Wextra file_lock.c
// Run:     $ ./a.out & ./a.out & ./a.out &

#include "error_handling.h"



bool some_func(void)
{
	char *p = NULL;
	char buf[100];
	EC_NULL( p = malloc(sizeof(buf)) )
	int fd_in = -1;
	EC_NEG1( fd_in  = open("infile", O_RDONLY) )
	int fd_out = -1;
	EC_NEG1( fd_out = open("outfile", O_WRONLY) )
	return true;

EC_CLEANUP_BEGIN
	free(p);
	if ( fd_in  != -1 )  (void)close(fd_in);
	if ( fd_out != -1 )  (void)close(fd_out);
	return false;
EC_CLEANUP_END
}



int main(void)
{
//#undef _SC_CHILD_MAX
//#define _SC_CHILD_MAX -1

#if defined(_SC_CHILD_MAX)
	errno = 0;  // Never set to 0 by any system call or library function
	long value1 = sysconf(_SC_CHILD_MAX);
	if ( value1 == -1 )			// Error or indeterminate
		if ( errno == 0 )		// Since errno is not changed, indeterminate
			puts("Max number of simultaneous processes per user ID: indeterminate\n");
		else					// Error
		{
//			perror("sysconf");
			fprintf(stderr, "Error in function '%s'\n%s:%d : %s (%d)\n",
				__func__, __FILE__, __LINE__, strerror(errno), errno);
//			exit(EXIT_FAILURE);
		}
	else						// Current value is returned
		printf("Max number of simultaneous processes per user ID: %ld\n", value1);
#else
	fprintf(stderr, "_SC_CHILD_MAX is not defined\n");
#endif



//#undef _SC_PAGE_SIZE
//#define _SC_PAGE_SIZE -1

#if defined(_SC_PAGE_SIZE)
	errno = 0;  // Never set to 0 by any system call or library function
	long value2;
	CHECK_ERROR(value2 = sysconf(_SC_PAGE_SIZE));
	printf("Size of a page: %ld b = %ld Kb \n", value2, value2/1024);
#else
	printf("_SC_PAGE_SIZE is not defined\n");
#endif



	EC_FALSE( some_func() )
	// Some code here

	exit(EXIT_SUCCESS);		// return 0
EC_CLEANUP_BEGIN
	exit(EXIT_FAILURE);		// return 1
EC_CLEANUP_END
}

// gcc -std=c17 -Wall -Wextra error_handling.c
// echo $?    - to see the last return in Unix

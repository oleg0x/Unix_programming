#include "error_handling.h"



int main(void)
{
	pid_t pid = fork();

	if ( pid == -1 )
		EC_FAIL
	else if ( pid > 0 )
		printf("This is the parent process, PID=%d\n", pid);
	else if ( pid == 0 )
	{
		puts("This is the child process, PID=0");
		char * const args[] = {"ls", "-l", NULL};
		EC_NEG1( execv("/bin/ls", args) )
	}

	exit(EXIT_SUCCESS);
EC_CLEANUP_BEGIN
	exit(EXIT_FAILURE);
EC_CLEANUP_END
}

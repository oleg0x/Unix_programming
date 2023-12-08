#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



/*struct sigaction
{
	void (*sa_handler)(int);
	void (*sa_sigaction)(int, siginfo_t *, void *);
	sigset_t sa_mask;
	int sa_flags;
};*/



static void clean_up()
{
	// Async-signal-safe code here
	char *msg = "Cleaning up... Finished.\n";
	write(STDERR_FILENO, msg, strlen(msg));
}



static void sig_handle(int sig_num)
{
	struct
	{
		int sig_num;
		char *msg;
	} sig_msg[] = {
		{ SIGBUS, "Bus error (bad memory access)!" },
		{ SIGFPE, "Floating-point exception!" },
		{ SIGILL, "Illegal instruction!" },
		{ SIGSEGV, "Invalid memory reference!" },
		{ SIGSYS, "Bad system call!" },
		{ SIGXCPU, "CPU time limit exceeded!" },
		{ SIGXFSZ, "File size limit exceeded!" },
		{ -1, NULL },
	};

	for ( int i = 0; sig_msg[i].sig_num > 0; ++i )
		if ( sig_msg[i].sig_num == sig_num )
		{
			write(STDERR_FILENO, sig_msg[i].msg, strlen(sig_msg[i].msg));
			write(STDERR_FILENO, "\n", 1);
			break;
		}

	clean_up();
	_exit(EXIT_FAILURE);		// Async-signal-safe version
}



static void sigint_handle(int)
{
	char *msg = "Caught a signal: SIGINT. Continue...\n";
	write(STDERR_FILENO, msg, strlen(msg));
}



void init_sig_handler()
{
	// Temporally block all signals
	sigset_t sig_set;
	sigfillset(&sig_set);					// Initialize to full, including all signals
	sigprocmask(SIG_SETMASK, &sig_set, NULL);
	// In multithreading use pthread_sigmask() instead

	// Ignore some signals
	struct sigaction sig_act;
	memset(&sig_act, 0, sizeof(sig_act));
	sigfillset(&sig_act.sa_mask);
	sig_act.sa_handler = SIG_IGN;			// Ignore signals
	sigaction(SIGQUIT, &sig_act, NULL);
	sigaction(SIGHUP, &sig_act, NULL);
	sigaction(SIGPIPE, &sig_act, NULL);

	// Set a handler for some signals
	sig_act.sa_handler = sigint_handle;
	sigaction(SIGINT, &sig_act, NULL);		// SIGINT == Ctrl+C
	sig_act.sa_handler = sig_handle;		// Handle signals
	sigaction(SIGBUS,  &sig_act, NULL);
	sigaction(SIGFPE,  &sig_act, NULL);
	sigaction(SIGILL,  &sig_act, NULL);
	sigaction(SIGSEGV, &sig_act, NULL);
	sigaction(SIGSYS,  &sig_act, NULL);
	sigaction(SIGXCPU, &sig_act, NULL);
	sigaction(SIGXFSZ, &sig_act, NULL);

	// Unblock all signals
	sigemptyset(&sig_set);					// Initialize to no signals
	sigprocmask(SIG_SETMASK, &sig_set, NULL);
}



int main()
{
	init_sig_handler();

	while ( 1 )
		pause();							// Wait for a signal
}

// gcc -std=gnu17 -Wall -Wextra signals_handling.c    (Note: gnu17 instead of c17)

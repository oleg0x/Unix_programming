#include "error_handling.h"
#include <pthread.h>

static long x = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;



static void* thread_func(void *arg)
{
	bool done;
	while ( true )
	{
		pthread_mutex_lock(&mtx);
		done = (x >= (long)arg);
		pthread_mutex_unlock(&mtx);
		if ( done )  break;
		pthread_mutex_lock(&mtx);
		printf("Thread 2, counter=%ld\n", ++x);
		pthread_mutex_unlock(&mtx);
		sleep(1);
	}
	return (void*)x;
}



int main(void)
{
	void *status;
	bool done;
	pthread_t tid;
	pthread_create(&tid, NULL, thread_func, (void*)6);
	while ( true )
	{
		pthread_mutex_lock(&mtx);
		done = (x >= 10);
		pthread_mutex_unlock(&mtx);
		if ( done )  break;
		pthread_mutex_lock(&mtx);
		printf("Thread 1, counter=%ld\n", ++x);
		pthread_mutex_unlock(&mtx);
		sleep(2);
	}
	pthread_join(tid, &status);
	printf("Thread 2 returned value: %ld\n", (long)status);
}

// gcc -std=c17 -Wall -Wextra thread_mutex.c

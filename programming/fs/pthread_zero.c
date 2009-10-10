#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 5

void *start_thread (void *);

int main (int argc, char **argv)
{
	pthread_t *threads;
	int i;

	threads = (pthread_t *) malloc (MAX_THREADS*sizeof (pthread_t));
	for (i = 0; i < MAX_THREADS; i++)
	{
		pthread_create (&threads[i], NULL, start_thread, (void *) i);
	}
	for (i = 0; i < MAX_THREADS; i++)
	{
		pthread_join (threads[i], NULL);
	}
	
	return 0;
}

void *start_thread (void *id)
{
	int tid = (int) id;

	printf ("test %d\n", tid);
}

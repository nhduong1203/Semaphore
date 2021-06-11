#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>

pthread_t *CreateThread(void *(*f)(void *), void *a)
{
	pthread_t *t = malloc(sizeof(pthread_t));
	assert(t != NULL);
	int ret = pthread_create(t, NULL, f, a);
	assert(ret == 0);
	return t;
}


static int elves;
static int reindeer;
static sem_t santaSem;
static sem_t reindeerSem;
static sem_t elfTex;
static sem_t mutex;
static sem_t c_Elves;

void *SantaClaus(void *arg)
{
	
	while (true)
	{
		sem_wait(&santaSem);
		sem_wait(&mutex);
		if (reindeer == 9)
		{
			printf("\nSanta Claus: Preparing Sleigh!! And Giving Gifts\n\n");
			for (int r = 0; r < 9; r++)
				sem_post(&reindeerSem);
				reindeer=0;
		}
		else if (elves == 3)
		{
			printf("\nSanta Claus: helping elves\n\n");
			for (int r = 0; r < 3; r++)
			sem_post(&c_Elves);
		}
		sem_post(&mutex);
	}
	return arg;
}

void *Reindeer(void *arg)
{

	int id = (int)arg;
	sleep(rand() % 5);
	while (true)
	{
		sem_wait(&mutex);
		if(reindeer==0){
			printf("\n-----Christmas is coming!!------\n\n");
		}
		printf("The reindeer %d arrives\n", id);
		reindeer++;
		if (reindeer == 9)
			sem_post(&santaSem);
		sem_post(&mutex);
		sem_wait(&reindeerSem);
		printf("Reindeer %d getting hitched\n", id);
		sleep(3+rand() % 5);
	}
	return arg;
}

void *Elve(void *arg)
{
	while (true)
	{
		int id = (int)arg;
		srand((unsigned int)time(NULL));
		int random = rand() % (30+id*2);
	    if (random<10)
		{
			
	        printf("Elve %d need help from Santa\n", id);
			sem_wait(&elfTex);
			sem_wait(&mutex);
			elves++;
			if (elves == 3)
				sem_post(&santaSem);
			else
				sem_post(&elfTex);
			sem_post(&mutex);

		    sem_wait(&c_Elves);
			printf("Elve %d will get help from Santa Claus\n", id);
			sleep(2);
			printf("Done!! Elve %d go back to making toy\n", id);
			sleep(2);

			sem_wait(&mutex);
			elves--;
			if (elves == 0)
				sem_post(&elfTex);
			sem_post(&mutex);
			sleep(rand() % (5));
		}
	}
	return arg;
}

int main(int ac, char **av)
{
	elves = 0;
	reindeer = 0;
	sem_init(&santaSem, 0, 0);
	sem_init(&reindeerSem, 0, 0);
	sem_init(&elfTex, 0, 1);
	sem_init(&mutex, 0, 1);
	sem_init(&c_Elves, 0, 0);

	pthread_t *santa_claus = CreateThread(SantaClaus, 0);

	pthread_t *reindeers[9];
	for (int r = 0; r < 9; r++)
		reindeers[r] = CreateThread(Reindeer, (void *)r + 1);

	pthread_t *elves[10];
	for (int e = 0; e < 10; e++)
		elves[e] = CreateThread(Elve, (void *)e + 1);

	int ret = pthread_join(*santa_claus, NULL);
	assert(ret == 0);
}
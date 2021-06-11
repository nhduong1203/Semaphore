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

static int thieves;
static int polices;
static sem_t mutex;
static sem_t thiefQueue;
static sem_t policeQueue;

void *ThiefT(void *arg)
{
    int id = (int)arg;
    sleep(rand() % id);
    sem_wait(&mutex);
    thieves++;

    printf("Thief %d come.\nThe queue have %d thieves and %d polices\n", id, thieves, polices);

    if (thieves == 4)
    {
        printf("Four thieves on the board.\n");
        thieves = 0;
        printf("The queue have %d thieves and %d polices\n", thieves, polices);

        for (int i = 0; i < 4; i++)
        {
            sem_post(&thiefQueue);
        }

        sleep(1);
        printf("Thief %d on the board and take the oars.\n", id);
        sem_post(&mutex);
        sem_wait(&thiefQueue);
    }

    else if (thieves >= 2 && polices >= 2)
    {

        printf("Two thieves and two polices on the board.\n");
        polices -= 2;
        thieves -= 2;
        printf("The queue have %d thieves and %d polices\n", thieves, polices);

        for (int i = 0; i < 2; i++)
        {
            sem_post(&thiefQueue);
            sem_post(&policeQueue);
        }

        sleep(1);
        printf("Thief %d on the board and take the oars.\n", id);
        sem_post(&mutex);

        sem_wait(&thiefQueue);
    }
    else
    {
        sem_post(&mutex);

        sem_wait(&thiefQueue);

        printf("Thief %d on the board\n", id);
    }
}

void *policeT(void *arg)
{
    int id = (int)arg;
    sleep(rand() % id);
    sem_wait(&mutex);
    polices++;
    printf("Police %d come.\nThe queue have %d thieves and %d polices\n", id, thieves, polices);

    if (polices == 4)
    {
        printf("Four polices on the board.\n");
        polices = 0;
        printf("The queue have %d thieves and %d polices\n", thieves, polices);

        for (int i = 0; i < 4; i++)
        {
            sem_post(&policeQueue);
        }

        sleep(1);
        printf("Police %d on the board and take the oars.\n", id);
        sem_post(&mutex);
        sem_wait(&policeQueue);
    }

    else if (thieves >= 2 && polices >= 2)
    {
        printf("Two thieves and two polices on the board.\n");
        polices -= 2;
        thieves -= 2;
        printf("The queue have %d thieves and %d polices\n", thieves, polices);

        for (int i = 0; i < 2; i++){
            sem_post(&thiefQueue);
            sem_post(&policeQueue);
        }

        sleep(1);
        printf("Police %d on the board and  take the oars.\n", id);
        sem_post(&mutex);
        sem_wait(&policeQueue);
    }
    else
    {
        sem_post(&mutex);

        sem_wait(&policeQueue);

        printf("Police %d on the board\n", id);
    }
}

void main(int ac, char **av)
{
    thieves = 0;
    polices = 0;
    sem_init(&mutex, 0, 1);
    sem_init(&thiefQueue, 0, 0);
    sem_init(&policeQueue, 0, 0);

    int countThief = 20;
    int countpolice = 20;

    pthread_t *ThiefThread[countThief];
    for (int i = 0; i < countThief; i++)
    {
        ThiefThread[i] = CreateThread(&ThiefT, (void *)i + 1);
    }

    pthread_t *policeThread[countpolice];
    for (int j = 0; j < countpolice; j++)
    {
        policeThread[j] = CreateThread(&policeT, (void *)j + 1);
    }

    for (;;);
}

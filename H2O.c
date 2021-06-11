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

static int oxygen;
static int hydrogen;
static sem_t mutex;
static sem_t oxyQueue;
static sem_t hydroQueue;

void *Oxygen(void *arg){

        int id = (int) arg;
        sleep(rand()%id);
        sem_wait(&mutex);
        oxygen++;

        printf("Add a oxygen\n");
        printf("Have %d oxygen and %d hydrogen\n\n",oxygen,hydrogen);

        if (hydrogen >= 2){
            sem_post(&hydroQueue);
            sem_post(&hydroQueue);
            hydrogen -= 2;
            sem_post(&oxyQueue);
            oxygen -= 1;
        }
        else{
            sem_post(&mutex);
        }
        sem_wait(&oxyQueue);

        //lien ket
        printf("Use 1 oxygen and 2 hydrogen to creat a water molecule\n");
        printf("Have %d oxygen and %d hydrogen\n\n",oxygen,hydrogen);

        sem_post(&mutex);
        
    return arg;
}

void *Hydrogen(void *arg) {
  
        int id = (int) arg;
        sleep(rand()%id);
        sem_wait(&mutex);
        hydrogen++;

        printf("Add a hydrogen\n");
        printf("Have %d oxygen and %d hydrogen\n\n",oxygen, hydrogen);

        if(hydrogen>=2 && oxygen>=1){
            sem_post(&hydroQueue);
            sem_post(&hydroQueue);
            hydrogen -= 2;
            sem_post(&oxyQueue);
            oxygen -= 1;
        }
        else{
            sem_post(&mutex);
        }
        sem_wait(&hydroQueue);
     return arg; 
}

void main(int ac, char** av){
    oxygen=0;
    hydrogen=0;
    sem_init(&mutex,0,1);
    sem_init(&oxyQueue,0,0);
    sem_init(&hydroQueue,0,0); 

    int countO=10; //nuber of oxygen threads
    int countH=20; //nuber of hydrogen threads
   
	pthread_t *Ogen[countO];
	for (int e = 0; e < countO; e++)
		Ogen[e] = CreateThread(Oxygen, (void *)e + 1);

    pthread_t *Hgen[countH];
	for (int r = 0; r < countH; r++)
		Hgen[r] = CreateThread(Hydrogen, (void *)r + 1);
    
    for(int j=0;j<countO;j++){
    pthread_join(*Ogen[j],NULL);
    }
    for(int i=0;i<countH;i++){
    pthread_join(*Hgen[i],NULL);
    }

}
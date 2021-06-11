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

static int chair;
static int customers;
static sem_t mutex ;
static sem_t customer;
static sem_t barber;
static sem_t customerDone;
static sem_t barberDone;

void *CustomerT(void *arg){
    int t = (int) arg;
    sleep( 2*(rand()% t));
    sem_wait(&mutex);
    if(customers == chair){
        //leave:
        printf("Customer %d: All chairs are occupied, I'm leaving the shop\n", t);
        sem_post(&mutex);
        
    }
    else{
        if(customers==0)
           printf("Wake up, barber\n"); 
        
        printf("Customer %d: I'm sitting in one of the free chairs\n",t);
        customers++;
        printf("%d free chairs\n",chair-customers);
    
        sem_post(&mutex);

        sem_post(&customer);
        sem_wait(&barber);

        //get hair cut
        printf("Customer %d: Get hair cut\n",t);

        sem_post(&customerDone);
        sem_wait(&barberDone);

    }

    return arg;
}

void *barberT(void *arg){
    while(1){
    sem_wait(&customer);
    sem_post(&barber);

    printf("Barber: Cutting hair\n");
    sleep(rand() % 3);
    printf("Barber: Finished cutting hair\n");
    customers -= 1;
    printf("%d free chairs\n",4-customers);
    
    sem_wait(&customerDone);
    sem_post(&barberDone);
    
    }
}


void main(int ac, char** av){

    chair=4;
    customers=0;
    sem_init(&mutex,0,1);
    sem_init(&customer,0,0);
    sem_init(&barber,0,0);
    sem_init(&customerDone,0,0);
    sem_init(&barberDone,0,0);

    int cuts = 30; //number of customer's threads

    pthread_t *threadCus[cuts] ; 

    for(int i=0;i<cuts;i++){
        threadCus[i] = CreateThread(CustomerT,(void*) i+1);
    }

    pthread_t  *threadBarber;
    threadBarber = CreateThread(barberT,0);

    int check = pthread_join(*threadBarber ,NULL);
    assert(check==0);


}
#include <stdlib.h> /* required for rand() */
#include <stdio.h>
#include "buffer.h"
#include <pthread.h>
#include <semaphore.h>
/* the buffer */
buffer_item buffer[BUFFER_SIZE]; //manipulated as a circular queue
int rear = 0;
int front = 0;
pthread_mutex_t mutex;
sem_t empty; 
sem_t full; 

int insert_item(buffer_item item) {
    /* insert item into buffer
    return 0 if successful, otherwise
    return -1 indicating an error condition */
    if((rear+1)%BUFFER_SIZE==front){
        return -1;
    }
    rear = (rear+1)%BUFFER_SIZE;
    buffer[rear] = item;
    return 0;
}

int remove_item(buffer_item *item) {
    /* remove an object from buffer
    placing it in item
    return 0 if successful, otherwise
    return -1 indicating an error condition */
    if(front==rear){
        return -1;
    }
    front = (front+1)%BUFFER_SIZE;
    item = &buffer[front];
    return 0;
}



void *producer(void *param) {
    buffer_item item;
    while (1) {
        /* sleep for a random period of time */
        sleep(rand()%3);
        /* generate a random number */
        item = rand();
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        if (insert_item(item))
            printf("insert error condition\n");
        else
            printf("producer produced %d\n",item);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *param) {
    buffer_item item;
    while (1) {
        /* sleep for a random period of time */
        sleep(rand()%3);
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        if (remove_item(&item))
            printf("remove error condition \n");
        else
            printf("consumer consumed %d \n",item);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}


int main(int argc, char *argv[]) {
/* 1. Get command line arguments argv[1],argv[2],argv[3] */
    int sleep_time, producer_threads, consumer_threads;
    if(argc != 4)
	{
		fprintf(stderr, "Input form:<sleep time> <producer threads number> <consumer threads number>\n");
		return -1;
	}
    sleep_time = atoi(argv[1]);
	producer_threads = atoi(argv[2]);
	consumer_threads = atoi(argv[3]);

/* 2. Initialize buffer */
    pthread_mutex_init(&mutex, NULL);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, BUFFER_SIZE-1);

/* 3. Create producer thread(s) */
    pthread_t producer_ids[producer_threads];
    for(int i=0;i<producer_threads;++i){
        pthread_create(&producer_ids[i], NULL, &producer, NULL);
    }

/* 4. Create consumer thread(s) */
    pthread_t consumer_ids[consumer_threads];
    for(int i=0;i<consumer_threads;++i){
        pthread_create(&consumer_ids[i], NULL, &consumer, NULL);
    }

/* 5. Sleep */
    sleep(sleep_time);

/* 6. Exit */
    return 0;

}
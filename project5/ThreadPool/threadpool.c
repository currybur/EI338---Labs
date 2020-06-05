/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"
#include <fcntl.h>

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
task worktodo[QUEUE_SIZE];

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];

int numTask = 0;
pthread_mutex_t mutex;
sem_t *sem;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    pthread_mutex_lock(&mutex);
    if(numTask >= QUEUE_SIZE){
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    worktodo[numTask++]=t;
    pthread_mutex_unlock(&mutex);
    return 0;
}

// remove a task from the queue
task *dequeue() 
{
    pthread_mutex_lock(&mutex);
    if(numTask<=0){
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    task *next_work = &worktodo[--numTask];
    pthread_mutex_unlock(&mutex);
    return next_work;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    // execute the task
    while(TRUE)
    {
        sem_wait(sem);
        task *newTask = dequeue();
        if (newTask == NULL) continue;
        execute(newTask->function, newTask->data);
    }
    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    task newTask;
    newTask.function = somefunction;
    newTask.data = p;
    int res = enqueue(newTask);
    if(!res) sem_post(sem);
    return res;
}

// initialize the thread pool
void pool_init(void)
{
    sem = sem_open("SEM", O_CREAT, 0666, 0);
    pthread_mutex_init(&mutex, NULL);
    for (int i=0; i<NUMBER_OF_THREADS; ++i){
        pthread_create(&bee[i],NULL,worker,NULL);
    }
}

// shutdown the thread pool
void pool_shutdown(void)
{
    for(int i=0; i < NUMBER_OF_THREADS; i++){
    	pthread_cancel(bee[i]);
        // pthread_join(bee[i],NULL);
    }
}

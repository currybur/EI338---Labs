#include "schedulers.h"
#include "cpu.h"
#include "task.h"
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

struct node *head;
int task_num = 0;

// add a task to the list 
void add(char *name, int priority, int burst)
{
    struct task* a_task = malloc(sizeof(struct task));
    a_task->name = name;
    a_task->priority = priority;
    a_task->burst = burst;
    insert(&head, a_task);
    task_num++;
}   

// invoke the scheduler
void schedule()
{
    struct node *tmp;
    struct node *prev = NULL;

    while(head->next!=NULL){
        tmp = head->next;
        head->next = prev;
        prev = head;
        head = tmp;
    }
    head->next = prev;

    while (task_num>0){
        if(tmp->task->burst <= 10){
            run(tmp->task,tmp->task->burst);    
            delete(&head, tmp->task);
            task_num--;
        }else{
            run(tmp->task,10);
            tmp->task->burst -= 10;
        }
        tmp = tmp->next;
        if(tmp == NULL){
            tmp = head;
        }
    }
}

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
    int crt_max = 10000;
    struct task *tmp_task;

    while (task_num>0){
        tmp = head;
        while (tmp!=NULL){
            if(tmp->task->burst<crt_max){
                crt_max = tmp->task->burst;
                tmp_task = tmp->task;
            }
            tmp = tmp->next;
        }
        run(tmp_task, tmp_task->burst);
        delete(&head, tmp_task);
        task_num--;
        crt_max = 100000;
    }
    
}

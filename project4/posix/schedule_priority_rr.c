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
struct node *new_head;
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
    struct node *rr_tail;
    int tmp_num = task_num;
    int pr;

    while (task_num>0){
        tmp = head;
        while (tmp!=NULL){
            if(tmp->task->priority < crt_max){
                crt_max = tmp->task->priority;
                tmp_task = tmp->task;
            }
            tmp = tmp->next;
        }
        insert(&new_head, tmp_task);
        delete(&head, tmp_task);
        task_num--;
        crt_max = 10000;
    }
    while(tmp_num > 0){
        if(new_head->next != NULL && new_head->task->priority == new_head->next->task->priority){
            tmp = new_head;
            pr = new_head->task->priority;
            while(new_head != rr_tail){
                if(tmp->task->burst<=10){
                    run(tmp->task, tmp->task->burst);
                    delete(&new_head, tmp->task);
                    tmp_num--;
                }else{
                    run(tmp->task, 10);
                    tmp->task->burst -= 10;
                }
                if(tmp->next == NULL || tmp->next->task->priority != pr ){
                    rr_tail = tmp->next;
                }
                tmp = tmp->next;
                if(tmp == rr_tail){
                    tmp = new_head;
                }
            }
        }else{
            run(new_head->task, new_head->task->burst);
            delete(&new_head, new_head->task);
            tmp_num--;
        }

    }
    
}

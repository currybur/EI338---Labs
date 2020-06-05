#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int *array;
int *res;
int lenth;
int count=0;
int thread_num=2;

void *bubbleSort(void *arg){
    int current_thread = count++;
    int low=current_thread*lenth/thread_num;
    int high=(current_thread+1)*lenth/thread_num;
        
    int tmp;

    for(int i=low;i<high;i++){
        for(int j=i+1;j<high;j++){
            if(array[i]>array[j]){
                tmp = array[j];
                array[j] = array[i];
                array[i] = tmp;
            }
        }
    }
}

void *merge(void *arg){
    int left_count = 0;
    int right_count = lenth/thread_num;
    for(int i=0;i<lenth;i++){
        if(left_count<lenth/thread_num && right_count<lenth){
            if(array[left_count]<=array[right_count]){
                res[i] = array[left_count++];
            }else{
                res[i] = array[right_count++];
            }
        }else{
            if(left_count<lenth/thread_num){
                res[i] = array[left_count++];
            }
            if(right_count<lenth){
                res[i] = array[right_count++];
            }
        }

    }


}


int main(){
    printf("input lenth\n");
    scanf("%d",&lenth);
    array = malloc(lenth*sizeof(int));
    res = malloc(lenth*sizeof(int));
    for(int i=0;i<lenth;i++){
        scanf("%d",&array[i]);
    }

    pthread_t sort_threads[thread_num];
    for(int i = 0;i < thread_num; i++){  
        pthread_create(&sort_threads[i],NULL,bubbleSort,NULL);
    }

    for (int i = 0; i < thread_num; i++){
        pthread_join(sort_threads[i], NULL); 
    }

    pthread_t merge_thread;
    pthread_create(&merge_thread, NULL, merge, NULL);
    pthread_join(merge_thread, NULL);

    for (int i=0; i<lenth; i++){
        printf("%d ", res[i]);
    }
    
    printf("\n"); 

    free(array);
    free(res);

}
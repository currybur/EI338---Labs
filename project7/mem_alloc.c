#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  

#define EMPTY "Unused"
int memorySize;
int memoryHoles[100];
char *allocatedName[100];
int holeNum;

int allocateMemory(char *name, int size, char* flag)
{
    if (flag[0] == 'F'){  // first fit
        for (int i=0; i<holeNum; i++){
            if (!strcmp(allocatedName[i], EMPTY)){
                if (size < memoryHoles[i]){  // 产生新hole
                    for (int j=holeNum; j>i; j--){  // 左边产生新hole，整体右移一位
                        allocatedName[j] = allocatedName[j-1];
                        memoryHoles[j] = memoryHoles[j-1]; 
                    }
                    memoryHoles[i+1] = memoryHoles[i] - size;
                    memoryHoles[i] = size;

                    allocatedName[i] = malloc(sizeof(char)*strlen(name));
                    strcpy(allocatedName[i], name);
                    holeNum ++;
                    return 1;
                }
                else if(size == memoryHoles[i])
                {
                    allocatedName[i] = malloc(sizeof(char)*strlen(name));
                    strcpy(allocatedName[i], name);
                    return 1;
                }
            }
        }
        return 0;
    }
    if(flag[0] == 'W'){
        int max_pos = -1;
        int crt_max = size-1;
        for(int i=0; i<holeNum; i++){
            if (!strcmp(allocatedName[i], EMPTY)){
                if (memoryHoles[i]>crt_max){
                    max_pos = i;
                    crt_max = memoryHoles[i];
                }
            }
        }
        if(max_pos == -1){
            return 0;
        }
        if(size < crt_max){
            for (int j=holeNum; j>max_pos; j--){  // 右移
                allocatedName[j] = allocatedName[j-1];
                memoryHoles[j] = memoryHoles[j-1]; 
            }
            memoryHoles[max_pos+1] = memoryHoles[max_pos] - size;  // 新hole
            memoryHoles[max_pos] = size;

            allocatedName[max_pos] = malloc(sizeof(char)*strlen(name));
            strcpy(allocatedName[max_pos], name);
            holeNum++;
        }
        else if (size == crt_max){
            allocatedName[max_pos] = malloc(sizeof(char)*strlen(name));
            strcpy(allocatedName[max_pos], name);
        }
    }
    if (flag[0] == 'B'){
        int min_pos = -1;
        int crt_min = memorySize+1;
        for (int i=0; i<holeNum; i++){
            if (!strcmp(allocatedName[i], EMPTY)){
                if (memoryHoles[i]>=size){
                    if(memoryHoles[i]<crt_min){
                        min_pos = i;
                        crt_min = memoryHoles[i];
                    }
                }
            }
        }
        if (min_pos == -1){
            return 0;
        }
        if (size < crt_min){
            for (int j=holeNum; j>min_pos; j--){
                allocatedName[j] = allocatedName[j-1];
                memoryHoles[j] = memoryHoles[j-1]; 
            }
            memoryHoles[min_pos+1] = memoryHoles[min_pos] - size;
            memoryHoles[min_pos] = size;

            allocatedName[min_pos] = malloc(sizeof(char)*strlen(name));
            strcpy(allocatedName[min_pos], name);
            holeNum++;
        }
        else if (size == crt_min){
            allocatedName[min_pos] = malloc(sizeof(char)*strlen(name));
            strcpy(allocatedName[min_pos], name);
        }
    }
}

int releaseMemory(char *name)
{
    for (int i=0; i<holeNum; i++){
        if (allocatedName[i][1]==name[1]){
            allocatedName[i] = EMPTY;
            int pos = i;
            if (pos>0 && !strcmp(allocatedName[pos-1], EMPTY)){  // 与左边的空hole合并
                memoryHoles[pos-1] += memoryHoles[pos];
                for (int j=pos; j<holeNum-1; j++){
                    memoryHoles[j] = memoryHoles[j+1];
                    allocatedName[j] = allocatedName[j+1];
                }
                holeNum--;
                pos--;
            }
            if (pos<holeNum-1 && !strcmp(allocatedName[pos+1], EMPTY)){  // 与右边的空hole合并
                memoryHoles[pos] += memoryHoles[pos+1];
                for (int j=pos+1; j<holeNum-1; j++){
                    memoryHoles[j] = memoryHoles[j+1];
                    allocatedName[j] = allocatedName[j+1];
                }
                holeNum--;
            }
            return 1;
        }
    }
    return 0;
}

void compactMemory()
{
    int count = 0;
    for (int i=0; i<holeNum; i++){
        if (strcmp(allocatedName[i], EMPTY)){
            int pos = i;
            while(pos>0 && !strcmp(allocatedName[pos-1], EMPTY)){  // 将该进程移到左边
                allocatedName[pos-1] = allocatedName[pos];
                int tmp = memoryHoles[pos-1];
                memoryHoles[pos-1] = memoryHoles[pos];
                allocatedName[pos] = EMPTY;
                memoryHoles[pos] = tmp;
                pos--;
            }
        }
    }
    for (int i=0; i<holeNum; i++){
        if (!strcmp(allocatedName[i], EMPTY)){  // 将所有空hole合并
            int pos = i;
            int tmp = 0;
            while (pos < holeNum){
                tmp += memoryHoles[pos++];
            }
            memoryHoles[i] = tmp;
            holeNum = i+1;
            return;
        }
    }
}

void reportState()
{
    int mem_pos = 0;
    for (int i=0; i<holeNum; i++){
        if (!strcmp(allocatedName[i], "Unused")){
            printf("Addresses [%d:%d] Unused\n", mem_pos, mem_pos+memoryHoles[i]-1);
        }
        else{
            printf("Addresses [%d:%d] Process %s\n", mem_pos, mem_pos+memoryHoles[i]-1, allocatedName[i]);
        }
        mem_pos += memoryHoles[i];
    }
}


int main(int argc, char *argv[])
{
    memorySize = atoi(argv[1]);;
    memoryHoles[0] = memorySize;
    for (int i=1; i<100; i++)
        memoryHoles[i] = 0;
    holeNum = 1;
    allocatedName[0] = EMPTY;

	while (1){
        char* cmd = malloc(sizeof(char)*30);
        printf("allocator>");
        fgets(cmd, 30, stdin);
        char *ch;
        ch = strtok(cmd," ");
		if (cmd[1] == 'Q'){  // 申请内存
            char *name;
            int size;
            char *flag;
            name = strtok(NULL, " ");
            size = atoi(strtok(NULL, " "));
            flag = strtok(NULL, " ");
			if(!allocateMemory(name, size, flag)){
                printf("Allocate failed!\n");
            }
		}
		else if (cmd[1] == 'L'){  // 释放内存
			char *name;
            name = strtok(NULL, " ");
            if(!releaseMemory(name)){
                printf("Release failed!\n");
            }
        }
        else if(cmd[0] == 'C'){
			compactMemory();
		}
		else if(cmd[0] == 'S'){
		    reportState();
		}
        else{
            printf("Invalid command.\n");
        }
	}
    return 0;
}

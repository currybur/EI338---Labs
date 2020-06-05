#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/stat.h>  

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int finish[NUMBER_OF_CUSTOMERS];

void update_need()
{
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		for(int j = 0; j < NUMBER_OF_RESOURCES; j++){
			need[i][j] = maximum[i][j] - allocation[i][j];
		}
	}
}

int request_resources(int customer_num, int request[])
{
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
		if(request[i] > maximum[customer_num][i] - allocation[customer_num][i]){
			printf("REQUEST EXCEED MAXIMUM\n");
			return 0;
		}
	}
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
		allocation[customer_num][i] += request[i];
		available[i] -= request[i];
	}
	update_need();
	
    // safety algorithm
	int count = 0, work[NUMBER_OF_RESOURCES];
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		finish[i] = 0;
	}
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++){	
		work[i] = available[i];
	}

    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
        if(!finish[i]){
            int j;
            for(j = 0; j < NUMBER_OF_RESOURCES; j++){
                if(work[j] < need[i][j])
                    break;
            }
            if(j == NUMBER_OF_RESOURCES){
                finish[i] = 1;
                count++;
                for(j = 0; j < NUMBER_OF_RESOURCES; j++)
                    work[j] += allocation[i][j];
            }
        }
    }
    if(count == NUMBER_OF_CUSTOMERS){  // safe
        return 0;
    }
    else{  // not safe
		for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
			allocation[customer_num][i] -= request[i];
			available[i] += request[i];
		}
		
        return -1;
    }
}

void release_resources(int customer_num, int release[])
{
	for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		if(release[i] > allocation[customer_num][i]){
			allocation[customer_num][i] = 0;
		    available[i] += allocation[customer_num][i];
        }
        else
        {
		    allocation[customer_num][i] -= release[i];
		    available[i] += release[i];
        }
	}
	update_need();
}

void load_max()
{
    FILE * file_fd;  
    int row_len, row = 0, col = 0, j = 0; 
    char file_buffer[2 * NUMBER_OF_RESOURCES], char_data[5];
  
    file_fd = fopen("./max.txt","rb");  

    while(1){
        row_len = fread(file_buffer, 1, 2 * NUMBER_OF_RESOURCES, file_fd);  
		if(row_len == 0){    
           break;  
       	} 
        col = 0;
        for(int i = 0; i < row_len; i++){  
            if(file_buffer[i] == ',' || file_buffer[i] == '\n'){
                char_data[j] = '\0';
                j = 0;
                maximum[row][col] = atoi(char_data);
                col++;
            }
            else{
                char_data[j] = file_buffer[i];
                j++;
            }
        }
        row++;    
    }  
    fclose(file_fd);    
}

void get_cmd(char *cmd, int *target_thread, int args[])
{
	int i=3, j=0, k=0, flag_thread_num=1;
	char tmp[5];
	while(1){
		if(cmd[i] == ' '){
			tmp[j] = '\0';
			j = 0;
			if(flag_thread_num){
				flag_thread_num = 0;
				*target_thread = atoi(tmp);
			}
			else{
				args[k] = atoi(tmp);
				k++;
			}
		}
		else if(cmd[i] == '\n'){
			args[k] = atoi(tmp);
            break;
		}
		else{
			tmp[j++] = cmd[i];
		}
		i++;
	}
}

void display()
{
	int i, j;
	printf("Avaliable\n");
	for(i = 0; i < NUMBER_OF_RESOURCES; i++){
		printf("%d\t", available[i]);
	}
	printf("\n");

	printf("Maximum\n");
	for(i = 0 ; i < NUMBER_OF_CUSTOMERS; i++){
		printf("T%d\t", i);
		for(j = 0; j < NUMBER_OF_RESOURCES; j++){
			printf("%d\t", maximum[i][j]);
		}
		printf("\n");
	}

	printf("Allocation\n");
	for(i = 0 ; i < NUMBER_OF_CUSTOMERS; i++){
		printf("T%d\t", i);
		for(j = 0; j < NUMBER_OF_RESOURCES; j++){
			printf("%d\t", allocation[i][j]);
		}
		printf("\n");
	}

	printf("Need\n");
	for(i = 0 ; i < NUMBER_OF_CUSTOMERS; i++){
		printf("T%d\t", i);
		for(j = 0; j < NUMBER_OF_RESOURCES; j++){
			printf("%d\t", need[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	int args[NUMBER_OF_RESOURCES], target_thread; 

	for(int i = 0; i < argc - 1; i++)
	{
		available[i] = atoi(argv[i + 1]);
	}

    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		for(int j = 0; j < NUMBER_OF_RESOURCES; j++){
			allocation[i][j] = 0;
		}
	}

	load_max();  // 从文件加载maximum
	update_need();  // 根据maximum和allocation初始化need
	char cmd[50];

	printf("cmd>");
	while (fgets(cmd, 50, stdin)){
		if(cmd[0] == '*'){
			display();
		}
		else if(cmd[1] == 'Q'){
			get_cmd(cmd, &target_thread, args);
			if(request_resources(target_thread, args)==0){
				printf("Satisfied.\n");
			}
			else{
				printf("Denied.\n");
			}
		}
		else if (cmd[1] == 'L'){
			get_cmd(cmd, &target_thread, args);
			release_resources(target_thread, args);
		}
		printf("cmd>");
	}
}
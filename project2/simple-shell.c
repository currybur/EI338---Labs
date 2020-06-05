/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>    
#include <sys/stat.h>

#define MAX_LINE 80 /* 80 chars per line, per command */
#define EMPTY_CMD 0
#define ACON_CMD 1
#define CONC_CMD 2
#define EMPTY_CMD 3

char **history[20];
int hstry_count=-1;




int get_cmd(char *args[]){
	char line[MAX_LINE];
	int pos = 0;
	char c;
	char blk = ' ';
	int count = 0;
	int is_his = 0;
	while(1){//read input
		c = getchar();
		if(c=='\n'){
			line[pos] = '\0';
			break;
		}
		else{
			line[pos++] = c;
		}
	}

	if(line[0]=='\0'){
		return EMPTY_CMD;
	}

	pos = 0;
	if(line[pos]=='!' || strcmp(line,"history")==0){
		is_his = 1;
	}
	if(!is_his){
		hstry_count++;
		history[hstry_count] = malloc(MAX_LINE);
	}

	while(line[pos]!='\0'){//split input to arguments, also store history
		char arg[MAX_LINE/4];
		int cp = 0;
		while((line[pos] != blk) && (line[pos] != '\0')){
			arg[cp++] = line[pos++];
		}

		args[count] = malloc((1+cp) * sizeof(char*));
		if(!is_his){
			history[hstry_count][count] = malloc((1+cp) * sizeof(char*));
		}
		for(int i=0;i<cp;i++){
			args[count][i] = arg[i];
			if(!is_his){
				history[hstry_count][count][i] = arg[i];
			}
		}
		args[count][cp] = '\0';
		if(!is_his){
			history[hstry_count][count][cp] = '\0';
		}
		count++;
		if(line[pos]!='\0')pos++;
	}
	if(!is_his){
		history[hstry_count][count] = NULL;
	}
	
	// check whether repeated cmd is stored
	int rpt = 1;
	pos = 0;
	if(hstry_count>0){
		while(rpt&&history[hstry_count-1][pos]&&history[hstry_count][pos]){
			if(strcmp(history[hstry_count-1][pos],history[hstry_count][pos])!=0){
				rpt = 0;
				break;
			}
			pos++;
		}
		if(history[hstry_count-1][pos]||history[hstry_count][pos]){
			rpt = 0;
		}
		if(rpt){
			hstry_count--;
		}
	}

	// check empty cmd and concurrency
	if(line[0]=='\0')
		return 0;
	if(args[count-1][0]=='&'){
		args[count-1] = NULL;
		return CONC_CMD;
	}
	else{
		args[count] = NULL;
		// for(int i=0;i<count+1;i++){printf(args[i]);}
		return ACON_CMD;
	}
	

}

int run_history(){
	int flag = ACON_CMD;
	// char **args = history[hstry_count];
	char *his_args[MAX_LINE/2+1];
	int len = 0;


	while(history[hstry_count][len]){
		his_args[len] = malloc(20*sizeof(char*));
		strcat(his_args[len],history[hstry_count][len]);
		len++;
	}
	
	
	if(strcmp(his_args[len-1],"&")==0){
		flag = CONC_CMD;
		his_args[len-1] = NULL;
	}else{
		his_args[len] = NULL;
	}
	// for(int i=0;i<strlen(args);i++)printf(args[i]);
	return execute_cmd(his_args,flag);
}

int redirect_output(char **args, int flag){
	pid_t pid, wpid;
    int status;
	pid = fork();

	if(pid<0){
		perror("Fork failed!\n");
		return 1;
	}else if(pid==0){
		char **ori_args = malloc(MAX_LINE * sizeof(char*));
		int i=0;
		while(args[i][0]!='>'){
			ori_args[i] = args[i];
			i++;
		}
		char *out_path = args[i+1];
		int out_put = open(out_path, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
		dup2(out_put, STDOUT_FILENO);
		close(out_put);
		if(execvp(ori_args[0],ori_args)==-1){
			perror(ori_args[0]);
			exit(1);
		}
	}else{
		if(flag==ACON_CMD){
			do{
                wpid =  waitpid(pid, &status, WUNTRACED);
            }while(!WIFEXITED(status) && !WIFSIGNALED(status));
			return 1;
		}else{
			return 1;
		}

	}
}

int redirect_input(char **args, int flag){
	pid_t pid, wpid;
    int status;
	pid = fork();
	int i=0;

	if(pid<0){
		perror("Fork failed!\n");
		return 1;
	}else if(pid==0){
		char **ori_args = malloc(MAX_LINE * sizeof(char*));
		while(args[i][0]!='<'){
			ori_args[i] = args[i];
			i++;
		}
		char *in_path = args[i+1];
		int in_put = open(in_path, O_RDONLY);
		dup2(in_put, STDIN_FILENO);
		close(in_put);
		if(execvp(ori_args[0],ori_args)==-1){
			perror(ori_args[0]);
			exit(1);
		}
	}else{
		if(flag==ACON_CMD){
			do{
                wpid =  waitpid(pid, &status, WUNTRACED);
            }while(!WIFEXITED(status) && !WIFSIGNALED(status));
			return 1;
		}else{
			return 1;
		}

	}
}

int piped_cmd(char **args, int flag){
	pid_t cpid;
	cpid = fork();
	if(cpid<0){
		perror("Fork failed!\n");
		return 1;
	}else if(cpid == 0){
		char *child_args[MAX_LINE/2 + 1];
		char *grand_args[MAX_LINE/2 + 1];
		int pos=0;
		int i=0;
		while(args[i][0]!='|'){
			child_args[i]=args[i];
			i++;
			pos++;
		}
		i++;
		child_args[pos] = NULL;
		while(args[i] != NULL){
			grand_args[i-pos-1]=args[i];
			i++;
		}
		grand_args[i-pos-1] = NULL;

		int pipefd[2];
		if(pipe(pipefd)==-1){
			perror("pipe error!");
			exit(1);
		}

		int gpid;
		gpid = fork();
		if(gpid<0){
			perror("Fork failed!\n");
			exit(1);
		}else if(gpid>0){

			//right cmd, read from pipe[0]/output
			wait(NULL);
			dup2(pipefd[0],STDIN_FILENO);//read from pipe
			close(pipefd[1]);
			if(execvp(grand_args[0],grand_args)==-1){
				perror(grand_args[0]);
				exit(1);
			}
		}else{
			
			//left cmd, output to pipe[1]/input
			dup2(pipefd[1],STDOUT_FILENO);
			close(pipefd[0]);
			if(execvp(child_args[0],child_args)==-1){
				perror(child_args[0]);
				exit(1);
			}
		}

	}else{
		if(flag==ACON_CMD){
			wait(NULL);
		}
	}
	return 1;

}

int execute_cmd(char *args[], int flag){

	if(flag == EMPTY_CMD){
		return 1;
	}

	// redirect output
	int i=0;
    while (args[i])
    {
        if (strcmp(args[i], ">") == 0)
            return redirect_output(args, flag);
        i++;
    }

	// redirect input
	i=0;
    while (args[i])
    {
        if (strcmp(args[i], "<") == 0)
            return redirect_input(args, flag);
        i++;
    }

	i=0;
    while (args[i])
    {
        if (strcmp(args[i], "|") == 0)
            return piped_cmd(args, flag);
        i++;
    }

	if(flag==0){
		return 1;
	}else if(flag==ACON_CMD){

		// !! cmd
		if (args[0][0]=='!'){
			if(hstry_count==-1){
				printf("No commands in history.\n");
				return 1;
			}else{
				return run_history();
			}
		}

		// show_history cmd
		if (strcmp(args[0],"history")==0){
			for(int i=0;i<=hstry_count;i++){
				printf("%d	",i+1);
				int c=0;
				while(history[i][c]){
					printf("%s ",history[i][c++]);
				}
				printf("\n");
			}
			return 1;
		}


		// common cmd not concurrency
		pid_t pid;
		pid = fork();
		if(pid < 0){ 
			perror("Fork failed!\n");
			return 1;
		}
		else if(pid == 0){//child process
			if(execvp(args[0],args)==-1){
				perror(args[0]);
				exit(1);
			}
		}
		else{//parent process
			wait(NULL);
			return 1;
		}
	}
	else if(flag==CONC_CMD){

		// common cmd concurrency
		pid_t pid;
		pid = fork();
		if(pid < 0){ 
			perror("Fork failed!\n");
			return 1;
		}
		else if(pid == 0){//child process
			if(execvp(args[0],args)==-1){
				perror(args[0]);
				exit(1);
			}
		}
		else{//parent process
			return 1;
		}
	}
	return 1;
}


int main(void)
{
	int cmd_flag;
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
	int should_run = 1;
		
    	while (should_run){
        	printf("osh>");
        	fflush(stdout);
			fflush(stdin);
			wait(NULL);
			cmd_flag = get_cmd(args);
			// printf("%d",cmd_flag);
			should_run = execute_cmd(args, cmd_flag);
			
        	/**
         	 * After reading user input, the steps are:
         	 * (1) fork a child process
         	 * (2) the child process will invoke execvp()
         	 * (3) if command included &, parent will invoke wait()
         	 */
    	}
    
	return 0;
}

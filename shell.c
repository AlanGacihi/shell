#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/wait.h>
#include <signal.h>

#define MAX_CHARS 200

char *get_cmd(){
    printf(">> ");
    char *cmd = (char *)malloc(MAX_CHARS);
    fgets(cmd, MAX_CHARS, stdin);
    return cmd;
}

char **parse_space(char *cmd){
    int token_num = 1;
    char **args = (char **)malloc(token_num*sizeof(char *));
  
    int count = 0;
 
    char * token = strtok(cmd, " \n");
    
    while(token != NULL){
        args[count] = (char *)malloc(sizeof(char *));
        args[count] = token;
        token = strtok(NULL, " \n");
        count++;
        if( count > token_num){
            args = realloc(args, token_num * sizeof(char*));
        }
    }
    args[count] = NULL;
    return args;
}

void run_command(char **args){
	pid_t child_pid;
    int status;
    char first[50] = "/bin/";

    child_pid = fork();
    if (child_pid == -1)
    {
        perror("Error:");
        exit(EXIT_FAILURE);
    }
    if (child_pid == 0)
    {
        char *argv[] = {"/bin/ls", "-l", "/usr/", NULL};

        if (execve(strcat(first, args[0]), args, NULL) == -1)
        {
            perror("Error:");
        }
    }
    else
    {
        wait(&status);
    }
}

int main(){
    while(1){
        char* cmd = get_cmd();
        
        if(strcmp(cmd, "exit\n") == 0) break;
        if(strcmp(cmd, "\n") == 0) continue;
        
        char** args = parse_space(cmd);
        
        if(strcmp(args[0], "cd") == 0){
            chdir(args[1]);
        }
        else if(strcmp(args[0], "pwd") == 0){
            char cwd[100];
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
        }
        else{ 
            run_command(args);
        }
   	    free(cmd);
    }
}
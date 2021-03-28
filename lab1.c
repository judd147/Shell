#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int status;
    while(1){
        FILE *fd;
        char input[50] = "";
        char outFile[20] = "";
        char *argv[30];
        int i = 0;
        int flag = 0;
        int newOut = 0;
        
        printf("$");
        fgets(input, 50, stdin);
        //handle empty lines and first character as space
        while(input[0] == '\n' || input[0] == ' '){
            printf("$");
            fgets(input, 50, stdin);
        }
        //check for background execution; fixed single & segfault
        if(input[strlen(input)-2] == '&'){
            if(strlen(input)>2){
                input[strlen(input)-2] = '\0';
                flag = 1;
            }
        }
        else{
            input[strlen(input)-1] = '\0';
        }
        //split the line into tokens; setup filenames; delete the string after seeing >
        argv[0] = strtok(input, " ");
        while(argv[i]!=NULL){
            i++;
            argv[i] = strtok(NULL, " ");
            //handle only > and > as the last char
            if(strcmp(argv[i-1], ">") == 0 && i!=1 && argv[i]!=NULL){
                newOut = 1;
                strcpy(outFile, argv[i]);
                argv[i-1] = '\0';
            }
        }
        //output redirection
        if(newOut){
            fd = fopen(outFile, "w");
            dup2(fileno(fd), fileno(stdout));
            close(fileno(fd));
        }
        //exit if user enters exit
        if(strcmp(argv[0], "exit") == 0){
            exit(0);
        }
        
        //create child process and execute the commands    
        int pid = fork();
        if(pid == -1){    
            perror("Error creating process");   
            exit(1);
        }
        if(pid == 0){
            //in child process
            execvp(argv[0], &argv[0]);  
            perror("exec failure");  
            return 1;
        }
        if(flag == 0){
            while((pid = wait(&status)) != -1){
                //wait for child process
            }
        }
        if(newOut)
            close(fileno(fd));
        //in parent process
    }
    return 0;
}

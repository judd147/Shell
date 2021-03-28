#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    int status;
    FILE *fd, *fin, *fout;
    char outFile[20] = "";
    char buffer[50];  
    int n;
    int i = 1;
    int newOut = 0;
    int flag = 1;
    
    if(argc < 2){
        printf("Usage: %s command \n", argv[0]);
        return 1;
    }
    
    while(argv[i]!=NULL){
        i++;
        //handle > as the first/last char; setup filename
        if(strcmp(argv[i-1], ">") == 0 && i!=2 && argv[i]!=NULL){
            newOut = 1;
            strcpy(outFile, argv[i]);
            argv[i-1] = '\0';
        }
        //set up the pipe
        if(strcmp(argv[i-1], "|") == 0 && i!=2 && argv[i]!=NULL){
            flag = 0;
            fin  = popen(argv[i-2], "r");  
            fout = popen(argv[i], "w");
            fflush(fout);
            while((n = read(fileno(fin), buffer, 50)) > 0)
                write(fileno(fout), buffer, n);
            pclose(fin);  
            pclose(fout);
        }
    }
    //output redirection
    if(newOut){
        fd = fopen(outFile, "w");
        dup2(fileno(fd), fileno(stdout));
        close(fileno(fd));
    }
    //create child process and execute the commands    
    if(flag){
        int pid = fork();
        if(pid == -1){    
            perror("Error creating process");   
            exit(1);
        }
        if(pid == 0){
            //in child process
            execvp(argv[1], &argv[1]);  
            perror("exec failure");  
            return 1;
        }
        while((pid = wait(&status)) != -1){
            //wait for child process
        }
        if(newOut)
            close(fileno(fd));
        //in parent process
    }
    return 0;
}
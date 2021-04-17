#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    FILE *fd, *fin, *fout, *file_ptr;
    int status, n, m, k, r, maxhistory;
    int count = 0;
    char history[10][50] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    file_ptr = fopen(".maxhistory", "r"); //read from .maxhistory
    fscanf(file_ptr, "%d", &maxhistory);
    fclose(file_ptr);
    if(maxhistory > 10){
        printf("The number is too large. No more than 10.");
        exit(1);
    }
    while(1){
        char input[50] = "";
        char piped[50] = "";
        char outFile[20] = "";
        char buffer[50];
        char *argv[30];
        char *temp[30];
        char ch = 0, k = 0;
        int i = 0;
        int j = 0;
        int flag = 0;
        int newOut = 0;
        int pipe = 0;
        int display = 0;
        
        file_ptr = fopen(".myhistory", "w");
        printf("$");
        fgets(input, 50, stdin);
        //handle empty lines and first character as space
        while(input[0] == '\n' || input[0] == ' '){
            printf("$");
            fgets(input, 50, stdin);
        }
        count = count+1;
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
        strcpy(piped, input);
        
        if(strcmp(input, "^[[A") == 0)
            strcpy(input, history[count-2]);
        //add/update the history
        if(count <= maxhistory)
            strcpy(history[count-1], input);
        else{
            for(m = 0; m < maxhistory-1; m++)
                strcpy(history[m], history[m+1]);
            count--;    
            strcpy(history[count-1], input);
        }
        //write to file
        for(r = 0; r < count; r++){
            fprintf(file_ptr, "%d %s\n", r+1, history[r]);
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
            //pipe signal
            else if(strcmp(argv[i-1], "|") == 0 && i!=1 && argv[i]!=NULL)
                pipe = 1;
            //exit if user enters exit
            else if(strcmp(argv[0], "exit") == 0)
                exit(0);
            else if(strcmp(argv[0], "history") == 0)
                display = 1;
        }//output redirection
        if(newOut){
            fd = fopen(outFile, "w");
            dup2(fileno(fd), fileno(stdout));
            close(fileno(fd));
        }//set up the pipe
        if(pipe){
            temp[0] = strtok(piped, "|");
            while(temp[j]!=NULL){
                j++;
                temp[j] = strtok(NULL, "|");
            }
            fin = popen(temp[0], "r");
            fout = popen(temp[1], "w");
            while((n = read(fileno(fin), buffer, 50)) > 0)
                write(fileno(fout), buffer, n);
            pclose(fin); 
            pclose(fout);
        }//display the history
        if(display){
            for(k = 0; k < count-1; k++)
                printf("%d %s\n", k+1, history[k]);
        }
        //create child process and execute the commands  
        if(!pipe && !display){
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
        close(fileno(fd));
        fclose(file_ptr);
        freopen("/dev/tty","w",stdout);
    }
    return 0;
}
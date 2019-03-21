#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include"vector.h"

int get_error(int argc, char* argv[]){
    if(argc){
        printf("The usage: ");
        return 1;
    }
    return 0;
}

// time_format, n_proc, exit_code, cmd, args
int main(int argc, char* argv[]){

    if(get_error(argc, argv)) return 1;

    unsigned n_proc=0, exit_code=0, interval=1;
    char time_format[15], cmd[50], args[100];
    int tube[2];
    // Vector *old_buff=NULL, *new_buff=NULL;

    if(pipe(tube)==-1){ EXCEPT(PIPE, "Process failed!"); }
    for(unsigned i=0;i<n_proc;++i){
        pid_t pid=fork();
        if(pid==-1){
            EXCEPT(PIPE, "Couldn't create a process!");
            return -1;
        }
        else if(pid==0){
            // strftime(,,,);

            int output=open("out", O_CREAT);
            if(output<0){ EXCEPT(NFILE, "File coudln't be created successfully!"); }
            if(dup2(output, 1)==-1){ EXCEPT(NFILE, "Duplication failed!"); }
        }
        else{
            int why;
            wait(&why);
            if(exit_code) printf("Exit \n");
            usleep(interval);
        }
    }

    return 0;
}
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<ctype.h>
#include<getopt.h>
#include<stdlib.h>
#include<sys/mman.h>
#include"vector.h"

int get_error(int argc, char* argv[]){
    if(argc<4){
        printf("The usage: \
            ./detect -t [time format] (-c) -l [limit] cmd args\n");
        return 1;
    }
    return 0;
}

// time_format, n_proc, exit_code, cmd, args
int main(int argc, char* argv[]){

    // if(get_error(argc, argv)) return 1;

    unsigned n_proc=0, exit_code=0, interval=0;
    char time_format[15], cmd[50], args[100];
    int tube[2];
    Vector *old_buff=NULL, *new_buff=NULL;
    // old_buff=new_Vector(100, old_buff);
    new_buff=new_Vector(100, new_buff);

    void* mem=malloc(1);
    char* old_buf=(char*)mmap(NULL, 100*sizeof(char), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    CHECK_MEMORY(old_buf);
    memcpy(old_buf, "_", 1);

    int opt;
    while((opt=getopt(argc, argv, "cl:t:i:"))!=-1){
        // printf("\tparsing..\n");
        switch(opt){
            case 'c':
                exit_code=1;
                printf("Exit code status: %d\n", exit_code);
                break;
            case 'l':
                n_proc=atoi(optarg);
                printf("Limit: %d\n", n_proc);
                break;
            case 't':
                strcpy(time_format, optarg);
                printf("Time format: %s\n", time_format);
                break;
            case 'i':
                interval=atoi(optarg);
                printf("Interval: %d\n", interval);
                break;
            default:
                printf("Other argument: %s\n", optarg);
                break;
        }
    }
    // printf("optind: %d\n", optind);
    for(int i=optind;i<argc;++i){
        if(i==optind){
            strcpy(cmd, argv[i]);
        } else{
            strcat(strcat(args, argv[i]), " ");
        }
    }
    printf("Command: %s\n", cmd);
    printf("Arguments: %s\n", args);
    if(!n_proc || !interval || !strlen(cmd) || !strlen(args) || !strlen(time_format)){
        printf("Not enough information has been given!\n");
        exit(1);
    }
    // exit(0);
    if(pipe(tube)==-1){ EXCEPT(PIPE, "Process failed!"); }
    char TIME[50];
    time_t raw_time;
    time(&raw_time);
    struct tm* info=localtime(&raw_time);
    strftime(TIME, 50, time_format, info);
    printf("%s\n", TIME);
    for(unsigned i=0;i<n_proc;++i){
        pid_t pid=fork();
        if(pid==-1){ EXCEPT(PIPE, "Couldn't create a process!"); }
        else if(pid==0){
            printf("\tC\n");
            pid_t new_pid=fork();
            if(new_pid==-1){ EXCEPT(PIPE, "Couldn't create a process!"); }
            else if(new_pid==0){
                printf("\t\tCC\n");
                close(tube[0]);
                if(dup2(tube[1], 1)==-1){ EXCEPT(NFILE, "Duplication failed!"); }

                char* args[3]={"ls", NULL};
                if(execvp(args[0], args)<0){ EXCEPT(FATAL, "Execution failed!\n"); }
            }
            else{
                wait(NULL);
                printf("\t\tPP\n");
                close(tube[1]);
                char buff[100];
                read(tube[0], buff, 100);
                close(tube[0]);
                if(memcmp(buff, old_buf, 100) || old_buf[0]=='_'){
                    printf("Context: %s\n", buff);
                    memcpy(old_buf, buff, 100);
                }
                exit(0);
            }
        }
        else{
            int stat;
            wait(&stat);
            printf("\tP\n");
            if(exit_code) printf("Exit %u\n", WEXITSTATUS(stat));
            if(i<n_proc-1){
            /*if(*/sleep(interval/1000);//==-1){ EXCEPT(WARNING, "The program didn't consider the given interval!"); }
                time(&raw_time);
                info=localtime(&raw_time);
                strftime(TIME, 50, time_format, info);
                printf("%s\n", TIME);
            }
        }
    }

    return 0;
}
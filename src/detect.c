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

#define MAX_SIZE 65100

int get_error(int argc, char* argv[]){
    if(argc<4){
        printf("The usage: \
            ./detect -t [time format] (-c) -l [limit] cmd args\n");
        return 1;
    }
    return 0;
}

void print_time(const char* time_format){
    char TIME[50];
    time_t raw_time;
    time(&raw_time);
    struct tm* info=localtime(&raw_time);
    strftime(TIME, 50, time_format, info);
    printf("%s\n", TIME);
}

// time_format, n_proc, exit_code, cmd, args
int main(int argc, char* argv[]){

    // if(get_error(argc, argv)) return 1;

    int n_proc=0, exit_code=0, interval=0;
    char time_format[30], *cmd, *args;
    int tube[2];

    char* old_buf=(char*)mmap(NULL, MAX_SIZE*sizeof(char), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    CHECK_MEMORY(old_buf);
    cmd=(char*)mmap(NULL, 100*sizeof(char), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    args=(char*)mmap(NULL, 100*sizeof(char), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    old_buf[0]='\0';
    time_format[0]='\0';

    int opt, cnt=0;
    while((opt=getopt(argc, argv, "+cl:t:i: "))!=-1){
        switch(opt){
            case 'c':
                if(!exit_code){
                    exit_code=1;
                    // printf("Exit code status: %d\n", exit_code);
                }
                break;
            case 'l':
                if(!n_proc){
                    n_proc=atoi(optarg);
                    // printf("Limit: %d\n", n_proc);
                }
                break;
            case 't':
                if(time_format[0]=='\0'){
                    strcpy(time_format, optarg);
                    // printf("Time format: %s\n", time_format);
                }
                break;
            case 'i':
                if(!interval){
                    interval=atoi(optarg);
                    // printf("Interval: %d\n", interval);
                }
                break;
            default:
                // printf("Other argument: %s\n", optarg);
                fprintf(stderr, "Not such an option");
                exit(1);
                break;
        }
        // if(n_proc && interval && time_format[0]!='\0') break;
    }
    args[0]='\0';
    cmd[0]='\0';
    for(int i=optind;i<argc;++i){
        if(i==optind){
            strcpy(cmd, argv[i]);
        } else{
            strcat(strcat(args, argv[i]), ",");
        }
    }
    // printf("Command: %s\t", cmd);
    // printf("Arguments: %s\n", args);
    if(n_proc<=0 || interval<=0 || cmd[0]=='\0'){
        printf("Not appropriate information has been given!\n");
        return 1;
    }
    if(time_format[0]!='\0') print_time(time_format);
    int exit_stat[2];
    exit_stat[0]=-1;
    for(int i=0;i<n_proc;++i){
        if(pipe(tube)==-1){ EXCEPT(PIPE, "Process failed!"); exit(1); }
        pid_t pid=fork();
        if(pid<0){ EXCEPT(PIPE, "Couldn't create a process!"); exit(1); }
        else if(pid==0){
            close(tube[0]);
            if(dup2(tube[1], 1)==-1){ EXCEPT(NFILE, "Duplication failed!"); exit(1); }
            char* args_[100];
            args_[0]=cmd;
            unsigned beg, in=0, j=1;
            char tmp[100][100];
            for(unsigned t=0;t<100;++t) tmp[t][0]='\0';
            for(;j<50;++j){
                if(in>=strlen(args)) break;
                for(beg=in;args[in]!=',';++in) tmp[j-1][in-beg]=args[in];
                // tmp[j-1][in]='\0';
                args_[j]=tmp[j-1];
                ++in;
            }
            args_[j]=NULL;
            // printf("Executing: %s %s %s %s\n", args_[0], args_[1], args_[2], args_[3]);
            if(execvp(args_[0], args_)<0){ EXCEPT(FATAL, "Execution failed!\n"); }
            exit(1);
        }
        else{
            int stat;
            wait(&stat);
            close(tube[1]);
            exit_stat[1]=WEXITSTATUS(stat);
            char buff[MAX_SIZE];
            for(unsigned t=0;t<100;++t) buff[t]='\0';
            size_t sz;
            // while((sz=read(tube[0], buff, MAX_SIZE))>0);
            read(tube[0], buff, MAX_SIZE);
            // printf("strlen of current buff : %ld\n", strlen(buff));
            if(memcmp(buff, old_buf, MAX_SIZE)/* || exit_stat[0]!=exit_stat[1]*/){
                write(1, buff, strlen(buff));
                if(exit_code){
                    char c='0'+exit_stat[1];
                    write(1, "exit ", 5); //printf("exit %d\n", exit_stat[1]);
                    write(1, &c, 1);
                    write(1, "\n", 1);
                }
                memcpy(old_buf, buff, MAX_SIZE);
                exit_stat[0]=exit_stat[1];
            }
            // if(exit_stat[1]) exit(1);
            if(i<n_proc-1){
                sleep(interval/1000);
                if(time_format[0]!='\0') print_time(time_format);
            }
        }
    }

    return 0;
}
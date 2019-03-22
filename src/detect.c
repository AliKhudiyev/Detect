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
    char time_format[30], *cmd, *args;
    int tube[2];
    Vector *old_buff=NULL, *new_buff=NULL;
    // old_buff=new_Vector(100, old_buff);
    new_buff=new_Vector(100, new_buff);

    char* old_buf=(char*)mmap(NULL, 10000*sizeof(char), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    CHECK_MEMORY(old_buf);
    cmd=(char*)mmap(NULL, 100*sizeof(char), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    args=(char*)mmap(NULL, 100*sizeof(char), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    old_buf[0]='\0';
    time_format[0]='\0';

    int opt, cnt=0;
    while((opt=getopt(argc, argv, "cl:t:i:"))!=-1){
        // printf("\tparsing..\n");
        ++cnt;
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
                break;
        }
        if(n_proc && interval && time_format[0]!='\0') break;
    }
    // printf("optind: %d\n\n", optind);
    args[0]='\0';
    cmd[0]='\0';
    for(int i=optind;i<argc;++i){
        if(i==optind){
            strcpy(cmd, argv[i]);
        } else{
            strcat(strcat(args, argv[i]), ",");
        }
    }
    // printf("Command: %s\n", cmd);
    // printf("Arguments: %s\n", args);
    if(!n_proc || !interval || cmd[0]=='\0' || time_format[0]=='\0'){
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
            pid_t new_pid=fork();
            if(new_pid==-1){ EXCEPT(PIPE, "Couldn't create a process!"); }
            else if(new_pid==0){
                close(tube[0]);
                if(dup2(tube[1], 1)==-1){ EXCEPT(NFILE, "Duplication failed!"); }

                char* args_[50]={"ls", "obj/", NULL};
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
            }
            else{
                int stat, exit_stat;
                wait(&stat);
                if((exit_stat=WEXITSTATUS(stat))){
                    exit(exit_stat);
                }
                close(tube[1]);
                char buff[10000];
                read(tube[0], buff, 10000);
                close(tube[0]);
                if(memcmp(buff, old_buf, 10000)){
                    printf("%s", buff);
                    memcpy(old_buf, buff, 10000);
                }
                exit(0);
            }
        }
        else{
            int stat;
            wait(&stat);
            int exit_stat;
            if(exit_code) printf("exit %u\n\n", (exit_stat=WEXITSTATUS(stat)));
            if(exit_stat) return 1;
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
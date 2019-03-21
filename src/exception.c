#include"exception.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

Exception* new_Exception(ERROR type, const char* msg){
    Exception* exception=(Exception*)malloc(sizeof(Exception));
    strcpy(exception->message, msg);
    return exception;
}

void init_Exception(ERROR type, const char* msg, Exception* const exception){
    exception->type=type;
    strcpy(exception->message, msg);
}

void get_Exception(const Exception* const exception){
    switch(exception->type){
        case NO_EXCEPT:
            // nothing to do here
            break;
        case WARNING:
            printf("WARNING: %s\n", exception->message);
            break;
        case DEFAULT:
            printf("DEFAULT ERROR: %s\n", exception->message);
            break;
        case MEMORY:
            printf("MEMORY ERROR: %s\n", exception->message);
            break;
        case NFILE:
            printf("FILE ERROR: %s\n", exception->message);
            break;
        case PROCESS:
            printf("PROCESS ERROR: %s\n", exception->message);
            break;
        case CMD:
            printf("COMMAND ERROR: %s\n", exception->message);
            break;
        case FORMAT:
            printf("FORMAT ERROR: %s\n", exception->message);
            break;
        case PIPE:
            printf("PIPE ERROR: %s\n", exception->message);
            break;
        case FATAL:
            printf("FATAL ERROR: %s\n", exception->message);
            break;
        default: break;
    }
}

void delete_Exception(Exception* exception){
    free(exception);
    exception=NULL;
}
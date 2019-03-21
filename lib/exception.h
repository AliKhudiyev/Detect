
#ifndef EXCEPTION
#define EXCEPTION

#include<string.h>

#define EXCEPT(type,msg)                \
    Exception except;                   \
    init_Exception(type, msg, &except); \
    get_Exception(&except)

#define CHECK_MEMORY(mem)                         \
    if(!mem){                                     \
        EXCEPT(MEMORY, "Bad memory allocation!"); \
        exit(1);                                  \
    }

#define CHECK_REACH(mem)                                 \
    if(!mem){                                            \
        EXCEPT(MEMORY, "Memory is unreachable! (null)"); \
        exit(1);                                         \
    }

typedef enum{
    NO_EXCEPT=0,
    WARNING,
    DEFAULT,
    MEMORY,
    NFILE,
    PROCESS,
    CMD,
    FORMAT,
    PIPE,
    FATAL
}ERROR;

typedef struct{
    ERROR type;
    char message[50];
}Exception;

Exception* new_Exception(ERROR type, const char* msg);
void init_Exception(ERROR type, const char* msg, Exception* const exception);
void get_Exception(const Exception* const exception);
void delete_Exception(Exception* exception);

#endif

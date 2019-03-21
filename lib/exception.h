
#ifndef EXCEPTION
#define EXCEPTION

#include<string.h>

#define EXCEPT(type,msg)                \
    Exception except;                   \
    init_Exception(type, msg, &except); \
    get_Exception(&except)

#define CHECK_EXCEPTION(mem)                         \
    if(!mem){                                        \
        EXCEPT(MEMORY, "Bad exception allocation!"); \
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

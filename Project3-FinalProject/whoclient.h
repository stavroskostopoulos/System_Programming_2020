
/* Stavros Kostopoulos sdi1700068 */

#ifndef CLIENT
#define CLIENT

#define BFSIZE 11

typedef struct thread_arguments{
    char* query;
    char* servIP;
    int servPort;
}thread_arguments;

typedef thread_arguments* thrargPtr;

thrargPtr create_argument(char* query, char* servIP, int servPort);//Create the argument that you will send to the thread
void destroy_argument(thrargPtr arg);//A function to destroy a thread argument struct



#endif

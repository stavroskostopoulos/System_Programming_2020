
/* Stavros Kostopoulos sdi1700068 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include  <fcntl.h>
#include  <error.h>
#include  <sys/wait.h>
#include <pthread.h>

#include "whoclient.h"

thrargPtr create_argument(char* query, char* servIP, int servPort){//Create the argument that you will send to the thread

    thrargPtr temp;
    temp = (thrargPtr)malloc(sizeof(thread_arguments));


    temp->query = strdup(query);
    temp->servIP = strdup(servIP);
    temp->servPort = servPort;


    return temp;

}


void destroy_argument(thrargPtr arg){//A function to destroy a thread argument struct
    free(arg->query);
    free(arg->servIP);
    free(arg);
}


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

#include<netinet/in.h>/*  for  sockaddr_in  */
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>/*  for  hton* */

#include "whoclient.h"

#include "hashlib.h"
#include "kouvas.h"
#include "dentro.h"
#include "topk.h"
#include "dateandlist.h"
#include "functions.h"



#define perror2(s, e) fprintf(stderr , "%s: %s\n", s, strerror(e))
#define LIMITUP 100


pthread_cond_t  cvar;/*  Condition  variable to synchronize socket connect() functions */
pthread_mutex_t  mtx;


pthread_mutex_t  mtx_lineprotect;

pthread_mutex_t  mtx_answerprotect;


void *client_thread(void *argument){
    int err;

    //keep the argument struct
    thrargPtr temp;
    temp = (thrargPtr)argument;

    thrargPtr my_arguments;//The argument pointer will change with the next getline
    my_arguments = create_argument(temp->query, temp->servIP, temp->servPort);

    //That's why we protect with the below conditional variable

    if(err = pthread_mutex_unlock (&mtx_lineprotect)) {/*  Unlock  mutex  */perror2("pthread_mutex_unlock", err); exit (1); }
    //You can go on and read the other line(query from the query file)


/* Let's do some preparations fot the connection with the whoServer */


    /* Now i want to bind()  to a specific IP (which was printed in the execution of the WhoServer)*/
    //To bind to a specific IP adress i used this source:
    //www.geeksforgeeks.org/socket-programming-cc/


    int sockfd;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){    perror("socket");   exit(-1); }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(my_arguments->servPort);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, my_arguments->servIP, &serv_addr.sin_addr)<=0){    perror("Invalid address/ Address not supported"); exit(-1); }

/*-------------------------------------------------------------------*/



    //printf("query is %s  servIP %s servPort is %d\n\n", my_arguments->query, my_arguments->servIP, my_arguments->servPort);

/* Synchronize all threads to connect() at once */
    if(err = pthread_mutex_lock (&mtx)) {/*  Lock  mutex  */perror2("pthread_mutex_lock", err); exit (1); }

    pthread_cond_wait(&cvar , &mtx);/*  Wait  for  signal  */

    if(err = pthread_mutex_unlock (&mtx)) {/*  Unlock  mutex  */perror2("pthread_mutex_unlock", err); exit (1); }
/* -------------------------------------------*/

    printf("Thread %ld: trying to connect to whoServer...\n", pthread_self());

/* Connect to whoServer */

    //Attempt to connect() to whoServer
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){    perror("Connect"); exit(-1); }

    printf("Thread %ld: successfully connected to whoServer!\n", pthread_self());

    write_message(sockfd, my_arguments->query, BFSIZE);

    if(err = pthread_mutex_lock (&mtx_answerprotect)) {/*  Lock  mutex  */perror2("pthread_mutex_lock", err); exit (1); }
    char* answer;
    answer = read_message(sockfd, BFSIZE);

    //Gia na mhn mperdeutoun ta prints sto stdout


    printf("\n%s\n", my_arguments->query);
    printf("%s \n\n", answer);

    if(err = pthread_mutex_unlock (&mtx_answerprotect)) {/*  Unlock  mutex  */perror2("pthread_mutex_unlock", err); exit (1); }



/* Some things before we exit.. */
    free(answer);
    close(sockfd);
    destroy_argument(my_arguments);

    //Au revouir
    pthread_exit(NULL);



}



int main(int argc, char *argv[]){

    int err;
    pthread_cond_init(&cvar , NULL);/*  Initialize  condition  variable  */
    pthread_mutex_init (&mtx , NULL);
    pthread_mutex_init (&mtx_lineprotect , NULL);
    pthread_mutex_init (&mtx_answerprotect , NULL);




    printf("\n\n");

/*Command line arguments check*/

    if(argc!=9){
        printf("Please check your command line arguments.\n\n\n");
        return 1;
    }


/*Get command line arguments*/

    int i;
    int servPort;//queryPortNum
    char* servIP;
    int numThreads;
    char* queryFile;

    for(i=0 ; i<9 ; i++){

        //Get input file name from command line
        if( !strcmp(argv[i],"-q") ){
            queryFile=strdup(argv[i+1]);
            printf("Query file: %s\n", queryFile);
        }


        //Get servPort
        if( !strcmp(argv[i],"-sp") ){
            servPort = atoi(argv[i+1]);
        }

        //Get servIP
        if( !strcmp(argv[i],"-sip") ){
            servIP=strdup(argv[i+1]);
            printf("Server IP: %s\n", servIP);
        }

        //Get numThreads
        if( !strcmp(argv[i],"-w") ){
            numThreads = atoi(argv[i+1]);
        }


    }

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    if ((fp = fopen(queryFile, "r")) == NULL){
        free(servIP);
        free(queryFile);

        exit(EXIT_FAILURE);
    }

    int querycounter=0;
    while ((read = getline(&line, &len, fp)) != -1) {//Count the total queries
        querycounter++;
        //free(line);
    }

    //Allocate tid array | We will create a number of threads equal to the number of queries | But only numThreads threads will be alive at the same time
    pthread_t *tids;
    if(querycounter>numThreads){//create numThreads threads to do a numThreads queries
        if( ( tids = malloc(numThreads*sizeof(pthread_t)) ) == NULL ){
            perror("Malloc"); exit(1);
        }
        querycounter -= numThreads;

    }else if(querycounter<numThreads){//create querycounter threads because the queries are less than numThreads
        if( ( tids = malloc(querycounter*sizeof(pthread_t)) ) == NULL ){
            perror("Malloc"); exit(1);
        }

    }

    //Rewind fp
    rewind(fp);

    //Check numThread number
    if(numThreads>LIMITUP){//Avoid too many threads
        printf("Number of threads should be up to 100!\n");
        exit(EXIT_FAILURE);
    }else if(numThreads==0){
        printf("Can't do anything with zero threads.\n");
        exit(EXIT_FAILURE);
    }


/* Give queries to the threads */
    int z=0;
    i=0;
    thrargPtr arguments;//A struct that wil lcarry the arguments for my threads


    //Wanna turn mutex to 0
    if(err = pthread_mutex_lock (&mtx_lineprotect)) {/*  Lock  mutex  */perror2("pthread_mutex_lock", err); exit (1); }


    while ((read = getline(&line, &len, fp)) != -1) {//Get each query one by one

        if(i==numThreads){//If all the threads got a query

            //So all our threads for now,got a query to solve,unlock them all and let them connect()
            sleep(1);//sleep for 3 secs so the synchronization is visible
            pthread_cond_broadcast(&cvar);


            //Wait for them to finish with the queries,so you can create new threads with the queries

            for(z=0 ; z<i ; z++){
                if(err = pthread_join (*( tids+z), NULL)) {/*  Wait  for  thread  termination  */
                    perror2("pthread_join", err); exit (1);
                }
            }


            //If all the previous threads have exited we can create new threads to take care fo the rest of the queries
            free(tids);
            if(querycounter>numThreads){//create numThreads threads to do a numThreads queries
                if( ( tids = malloc(numThreads*sizeof(pthread_t)) ) == NULL ){
                    perror("Malloc"); exit(1);
                }
                querycounter -= numThreads;

            }else{//querycounter<numThreads
                if( ( tids = malloc(querycounter*sizeof(pthread_t)) ) == NULL ){
                    perror("Malloc"); exit(1);
                }
            }

            i=0;
        }



        //Get rid of the new line character in my line(query)
        line[strlen(line)-1] = '\0';

        arguments = create_argument(line, servIP, servPort);




        //Create each thread giving it a query (line)
        if( err = pthread_create(tids+i, NULL, client_thread, (void*) arguments) ){
            perror2("pthread_create", err);
            exit(1);
        }




        //Se orismenes periptwseis o pointer *line allaze,logw scheduling,kai to programma phgaine prwta sthn getline kai meta to thread apo8hkeue ton pointer pou tou do8hke
        //me apotelesma na xanontai kapoia lines,gia thn antimetwpish autou,xrhsimopoihsa allo ena cond variable (ara kai allo ena mutex)
        //mtx_lineprotect(mutex) kai lineprotect(cond var)

        //Twra to programma perimenei na apo8hkeusei to thread prwta to line(query) pou tou anate8hke kai afou to apo8hkeusei,
        //mporei na proxwrhsei sto diavasma neou line(query) kai thn dhmiourgia neou thread
        if(err = pthread_mutex_lock (&mtx_lineprotect)) {/*  Lock  mutex  */perror2("pthread_mutex_lock", err); exit (1); }






        destroy_argument(arguments);//We passed the arguments,we're fine,we can go on with the next getline


        i++;
        //free(line);
    }

/* This one is for the last threads(After we met EOF with getline) */


    //So all our threads for now,got a query to solve,unlock them all and let them connect()
    sleep(1);//sleep for 3 secs so the synchronization is visible
    pthread_cond_broadcast(&cvar);


    //Wait for the last threads to finish
    for(z=0 ; z<i ; z++){
        if(err = pthread_join (*( tids+z), NULL)) {/*  Wait  for  thread  termination  */
            perror2("pthread_join", err); exit (1);
        }
    }
    free(tids);

    printf("Done!\n" );


/*-----------------------------------------*/


/* Free allocated memory and exit */
    if(err = pthread_cond_destroy (&cvar)) {/*  Destroy  condition  variable  */perror2("pthread_cond_destroy", err); exit (1); }


    //pthread_cond_destroy(&cvar);
    pthread_mutex_destroy(&mtx_lineprotect);
    pthread_mutex_destroy(&mtx_answerprotect);
	pthread_mutex_destroy(&mtx);
    fclose(fp);
    free(servIP);
    free(queryFile);

    return 1;
}


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

#include<netinet/in.h>/*  for  sockaddr_in  */
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>/*  for  hton* */
#include <netdb.h>

#include<pthread.h>

#include "whoserver.h"

#define LIMITUP 100


#include "hashlib.h"
#include "kouvas.h"
#include "dentro.h"
#include "topk.h"
#include "dateandlist.h"
#include "functions.h"

#define perror2(s, e) fprintf(stderr , "%s: %s\n", s, strerror(e))


char worker_ip[INET_ADDRSTRLEN];

workerfd_listPtr root=NULL;//The root of the worker fd list
pthread_mutex_t fd_attach_mtx;



void *server_thread(void *argument){

    int buffersize = *((int*)argument);
    //printf("\n\nHello from the thread\n\n");

    while ( 1 ) {
        int fd;
        int err;//perror2 function argument


        fd =  obtain(pool, buffersize);//Get the fd you are going to handle

        pthread_cond_signal(&cond_nonfull);

        char* pipe_message;

        pipe_message = read_message(fd, BFSIZE);//Read whatever the fd has to tell us

        if(pipe_message[0]!='/'){//an phres summary stats kai neo port

            int workerport;
            char* summary_stats;
            char *token;
            token = strtok(pipe_message, "@");

            int count=0;
            while(token!=NULL){

                if(count==0){
                    workerport = atoi(token);

                }else if(count==1){
                    summary_stats = strdup(token);//Print summary stats
                }

                count++;
                token = strtok(NULL, "@");//Get the next token

            }


            /*Now that we know the worker's IP (worker_ip) and it's port number let's connect to it*/
            /* Now i want to bind()  to a specific IP (which was printed in the execution of the WhoServer)*/
            //To bind to a specific IP adress i used this source:
            //www.geeksforgeeks.org/socket-programming-cc/


            int workersockfd;
            struct sockaddr_in work_addr;

            if ((workersockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){    perror("socket");   exit(-1); }

            work_addr.sin_family = AF_INET;
            work_addr.sin_port = htons(workerport);

            // Convert IPv4 and IPv6 addresses from text to binary form
            if(inet_pton(AF_INET, worker_ip, &work_addr.sin_addr)<=0){    perror("Invalid address/ Address not supported"); exit(-1); }


            //Attempt to connect() to worker that waits for our connection
            if (connect(workersockfd, (struct sockaddr *)&work_addr, sizeof(work_addr)) < 0){    perror("Connect"); exit(-1); }
            printf("\n------------------------------------------------\nConnected to worker with port number %d!\n------------------------------------------------\n%s\n\n", workerport, summary_stats);

            //Now we gotta attach this worker's fd in the worker fd list so we can use it for the queries

            //We dont want to race condition,so we are gonna use a mutex
            if(err = pthread_mutex_lock (&fd_attach_mtx)) {/*  Lock  mutex  */perror2("pthread_mutex_lock", err); exit (1); }


            if(root==NULL){//If the lsit is empty
                root = create_list_node(workersockfd);
            }else{//list is not empty
                workerfd_listPtr newnode = create_list_node(workersockfd);
                attach_fd_to_list(root, newnode);
            }


            if(err = pthread_mutex_unlock (&fd_attach_mtx)) {/*  Unlock  mutex  */perror2("pthread_mutex_unlock", err); exit (1); }

            free(summary_stats);
            //print_fd_list(root);

        }else{//An dexthikes query

            char* query = strdup(pipe_message);
            char* token;//To get the first word of the query(query name)
            token = strtok(query, " ");



            printf("\nReceived a query: %s\n", pipe_message);

            /* variables to store the query answers */
            int total_krousmata=0;//diseaseFrequency &&

            char* result;//numPatientAdmissions && numPatientDischarges

            int found=0;//searchPatientRecord && numPatientAdmissions && numPatientDischarges
            char* patient_found;//searchPatientRecord

            char* topkstr;//topk-AgeRanges


            /* I gotta send the query to the workers and gather the answer so send the final answer back to the whoClient */

            workerfd_listPtr temp = root;

            while(temp!=NULL){//send query to each worker

                char* answer;
                //Lock so only this thread uses this worker to get its results
                if(err = pthread_mutex_lock(&(temp->mtx)) ){/*  Lock  mutex  */perror2("pthread_mutex_lock", err); exit (1); }

                //Write the query to the worker you got permission on right now
                write_message(temp->fd, pipe_message, BFSIZE);


                //(Block until you)Get the answer

                answer = read_message(temp->fd, BFSIZE);







                if(!strcmp(token, "/diseaseFrequency")){//If it was a diseaseFrequency query

                    total_krousmata += atoi(answer);

                }else if(!strcmp(token, "/searchPatientRecord")){//If it was a searchPatientRecord query

                    if(strcmp(answer, "NO")){//If you just got the result
                        found=1;
                        patient_found = strdup(answer);
                    }

                }else if((!strcmp(token, "/numPatientAdmissions")) || (!strcmp(token, "/numPatientDischarges"))){//If it was a numPatientAdmissions && numPatientDischarges query

                    if(strcmp(answer, "NO")&&strcmp(answer, "N0")){//If you got a result


                        found=1;
                        result=strdup(answer);


                    }

                }else if(!strcmp(token, "/topk-AgeRanges")){//If it was a topk-AgeRanges query

                    if(strcmp(answer, "NO")){//If you just got the result
                        found=1;
                        topkstr = strdup(answer);
                    }

                }


                free(answer);
                //read_message(temp->fd, BFSIZE);
                //sleep(1);



                if( err = pthread_mutex_unlock(&(temp->mtx)) ) {/*  Unlock  mutex  */perror2("pthread_mutex_unlock", err); exit (1); }
                
                temp = temp->next;

            }


            /* Write the answers to the whoClient */
            if(!strcmp(token, "/diseaseFrequency")){

                char number[7];
                sprintf(number, "%d", total_krousmata);

                //Let the whoServer print the query and the result
                printf("\n%s\n%s\n\n", pipe_message, number);

                //send the result to the whoClient
                write_message(fd, number, BFSIZE);

            }else if(!strcmp(token, "/searchPatientRecord")){

                if(found==0){//if no patient was found
                    //Let the whoServer print the query and the result
                    printf("\n%s\nNo patient with such record id was found!\n\n", pipe_message);

                    //send the result to the whoClient
                    write_message(fd, "No patient with such record id was found!", BFSIZE);
                }else{//If a patient was found

                    //Let the whoServer print the query and the result
                    printf("\n%s\n%s\n\n", pipe_message, patient_found);

                    //send the result to the whoClient
                    write_message(fd, patient_found, BFSIZE);
                    free(patient_found);
                }

            }else if((!strcmp(token, "/numPatientAdmissions")) || (!strcmp(token, "/numPatientDischarges"))){

                //Let the whoServer print the query and the result
                printf("\n%s\n%s\n\n", pipe_message, result);

                //send the result to the whoClient
                write_message(fd, result, BFSIZE);
                free(result);

            }else if(!strcmp(token, "/topk-AgeRanges")){

                //Let the whoServer print the query and the result
                printf("\n%s\n%s\n\n", pipe_message, topkstr);

                //send the result to the whoClient
                write_message(fd, topkstr, BFSIZE);
                free(topkstr);


            }






            free(query);
        }







        free(pipe_message);

        //sleep(4);

    }

}

int main(int argc, char *argv[]){

    printf("\n\n");

    pthread_mutex_init(&mtx , NULL);
    pthread_mutex_init(&fd_attach_mtx , NULL);

    pthread_cond_init(&cond_nonempty , 0);
    pthread_cond_init(&cond_nonfull , 0);
    int err;//perror2 function argument








/*Command line arguments check*/

    if(argc!=9){
        printf("Please check your command line arguments.\n\n\n");
        return 1;
    }


/*Get command line arguments*/
    int i;
    int queryPortNum;
    int statisticsPortNum;
    int numThreads;
    int buffersize;


    for(i=0 ; i<9 ; i++){

        //Get buffersize
        if( !strcmp(argv[i],"-b") ){
            buffersize = atoi(argv[i+1]);
        }

        //Get queryPortNum
        if( !strcmp(argv[i],"-q") ){
            queryPortNum = atoi(argv[i+1]);
        }

        //Get statisticsPortNum
        if( !strcmp(argv[i],"-s") ){
            statisticsPortNum = atoi(argv[i+1]);
        }

        //Get numThreads
        if( !strcmp(argv[i],"-w") ){
            numThreads = atoi(argv[i+1]);
        }


    }


/* Create circular buffer (pool) */
//implementation apo tis diafaneies tou kuriou Ntoula

    pool = (poolPtr)malloc(sizeof(pool_t));
    initialize(pool, buffersize);



/* Create num Threads */

    //Check numThread number
    if(numThreads>LIMITUP){//Avoid too many threads
        printf("Number of threads should be up to 100!\n");
        exit(EXIT_FAILURE);
    }else if(numThreads==0){
        printf("Can't do anything with zero threads.\n");
        exit(EXIT_FAILURE);
    }



    //Allocate tid array
    pthread_t *tids;
    if( ( tids = malloc(numThreads*sizeof(pthread_t)) ) == NULL ){
        perror("Malloc"); exit(1);
    }


    int* bufsize_ptr=(int*)malloc(sizeof(int));
    int temp=buffersize;
    *bufsize_ptr = temp;
    for(i=0 ; i<numThreads ; i++){

        //Create each thread
        if( err = pthread_create(tids+i, NULL, server_thread, (void*) bufsize_ptr) ){
            perror2("pthread_create", err);
            exit(1);
        }



    }








/* Needed to find a way to print my actual IP address and not the INADDR_ANY (0.0.0.0) */


//Found this:
//www.geeksforgeeks.org/c-program-display-hostname-ip-address/

    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;

    // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    if (hostname == -1){    perror("gethostname");  exit(1); }

    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL){    perror("gethostbyname"); exit(1); }

    // To convert an Internet network
    // address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
    printf("IP address: %s\n\n", IPbuffer);

    //This prints 0.0.0.0 (INADDR_ANY)
    //printf("IP address %s\n\n", inet_ntoa(query_address.sin_addr));

/*---------------------------------------------------------------------------------------*/




/* Listen to queryPortNum */

    int queryserver_fd, new_socket_query;
    struct sockaddr_in query_address;
    int opt=1;//To enable the socket options
    int addrlen = sizeof(query_address);


    // Creating socket file descriptor
    if ((queryserver_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(queryserver_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    query_address.sin_family = AF_INET;
    query_address.sin_addr.s_addr = INADDR_ANY;
    query_address.sin_port = htons(queryPortNum);


    if (bind(queryserver_fd, (struct sockaddr *)&query_address, sizeof(query_address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(queryserver_fd, 20) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }






/* Listen to statisticsPortNum */

    int workerserver_fd, new_socket_worker;
    struct sockaddr_in worker_address;

    addrlen = sizeof(worker_address);



    // Creating socket file descriptor
    if ((workerserver_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(workerserver_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    worker_address.sin_family = AF_INET;
    worker_address.sin_addr.s_addr = INADDR_ANY;
    worker_address.sin_port = htons(statisticsPortNum);


    if (bind(workerserver_fd, (struct sockaddr *)&worker_address, sizeof(worker_address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(workerserver_fd, 20) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }






    printf("Server is listening...\n\n");



/* Now I gotta listen to any new connections from these two sockets */
//And add any new socket fd to the circular buffer
    struct sockaddr_in address;

    int fds[2];//Create an array with the 2 fds produced by the socket() function (each one connected to different ports)-> queryPortNum and statisticsPortNum
    for(i=0 ; i<2 ; i++){
        if(!i) fds[i] = queryserver_fd;
        else fds[i] = workerserver_fd;
    }


    int firsttime=0;//the first connection we will accept will be wiuth a worker.IO want to grab the worker's IP adress from τhis connection
    while(1){

        int newfd;
        newfd=network_accept_any(fds, 2, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        if(!firsttime){//print the workers' ip address
            strcpy(worker_ip, inet_ntoa(address.sin_addr));
            printf("Workers' IP address: %s\n\n----------------------------------------\n\n", worker_ip);
            firsttime=1;
        }


        //Place new fd in buffer
        place(pool, newfd, buffersize);
        printf("Accepted new connection on fd %d!\n", newfd);

        pthread_cond_signal(&cond_nonempty);


    }

    //pthread_join isn't necessary here cause we are always waiting for a new connection in queryPort or statisticsPort








/* Free allocated memory and quit */
    destroy_fd_list(root);
    pthread_cond_destroy(&cond_nonempty);
    pthread_cond_destroy(&cond_nonfull);
    pthread_mutex_destroy(&mtx);
    close(queryserver_fd);
    close(workerserver_fd);
    free(tids);
    free(pool->fds);
    free(pool);

}

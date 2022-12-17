
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
#include  <signal.h>


#include "hashlib.h"
#include "kouvas.h"
#include "dentro.h"
#include "topk.h"
#include "dateandlist.h"
#include "functions.h"

void send_signal(int sig, pid_t *pdarray, int numberofworkers){
    int i=0;
    for(i=0 ; i<numberofworkers ; i++){
        kill(pdarray[i], sig);
    }
}


int main(int argc, char *argv[]){

    printf("\n\n");

/*Command line arguments check*/

    if(argc!=7){
        printf("Please check your command line arguments.\n\n\n");
        return 1;
    }


/*Get command line arguments*/



    int i;
    int numWorkers=0;
    int buffersize=0;
    char* input_dir;

    for(i=0 ; i<7 ; i++){

        //Get input file name from command line
        if( !strcmp(argv[i],"-i") ){
        	input_dir=strdup(argv[i+1]);
            printf("Input file: %s\n", input_dir);
        }

        //Get numWorkers
        if( !strcmp(argv[i],"-w") ){
            numWorkers = atoi(argv[i+1]);
        }

        //Get buffersize
        if( !strcmp(argv[i],"-b") ){
            buffersize = atoi(argv[i+1]);
        }

    }

/*Count the files in the input_dir directory*/
    DIR* folder;
    struct dirent *direntp;

    int filecount=0;//The number of directory files in the input_dir directory

    if( ( folder = opendir(input_dir) ) == NULL ){//If there is a problem while opening the input_dir directory
        fprintf(stderr, "Cannot open %s \n", input_dir);

        //Close the program and let the user try again
        free(input_dir);
        exit(1);
    }else{

        while( ( direntp=readdir(folder) ) != NULL ){
            if(direntp->d_type==DT_DIR && strcmp( direntp->d_name, "." ) && strcmp( direntp->d_name, ".." ) ){//If it is a folder and its not the "." and ".." directory count it
                filecount++;
            }
        }

    }

    //printf("\nGia des exw tosa file %d\n", filecount);


/* Create the worker processes*/
    pid_t pid;

    pid_t *pidar;
    pidar = (pid_t*)malloc(numWorkers*sizeof(pid_t));


    for(i=0 ; i<numWorkers ; i++){

        pid = fork();

        if( pid==-1 ){
            perror("Problem with fork");\

            //Close the program and let the user try again
            closedir(folder);
            free(input_dir);
            exit(1);
        }


        if( pid==0 ){//Child

            //The worker has to know its number
            //char worker_number[2];

            //Convert i to string
            //sprintf(worker_number, "%d", i);

            //Convert buffer size to string
            //char buffersize_str[5];
            //sprintf(buffersize_str, "%d", buffersize);

            //Run the worker processes

            //Here an execlp bug occured that wasted 4 days of my life and my mental health :)
            //execlp("./worker", worker_number, buffersize_str, input_dir, NULL);

            pidar[i] = getpid();

            //The internet suggests to use execvp to solve this bug,I trust the internet
            char arg_str[100];
            sprintf(arg_str, "%d %d %s", i, buffersize, input_dir);

            char *argums[]={"./worker", arg_str, NULL};

            execv(argums[0], argums);



            perror("Exec fail");
            free(input_dir);
            closedir(folder);

            exit(29);
        }else{//Parent
            //Do nothing
        }


    }




/* Create the pipes */

    char pipename[12];//Pipename is: pipenumberx ,where x is the worker number.It consists of 11 characters + 1 for the terminal character
    int fd_array[numWorkers];//An array which contains all the fd of the workers(diseaseAggregator to worker pipes)
    int w2a_fd_array[2][numWorkers];///An array which contains all the fd of the workers (workers to Aggregator pipes)| The second line of the array will let us know how many sumamry stats we are gonna get

    //Set allt he values of the array's second line as zero
    for(i=0 ; i<numWorkers ; i++){
        w2a_fd_array[1][i] = 0;
    }

    for(i=0 ; i<numWorkers ; i++){

        sprintf(pipename, "pipenumber%d", i);

        if( mkfifo(pipename, 0666) == -1 ){//Create an amount of numWorkers named pipes
            perror("mkfifo");

            closedir(folder);
            free(input_dir);
            exit(1);
        }

        if( ( fd_array[i]=open(pipename, O_RDWR |  O_NONBLOCK) ) < 0 ){
            perror("fifo open problem");
            free(input_dir);
            closedir(folder);

            exit(4);
        }

        sprintf(pipename, "worker2agg%d", i);

        if( mkfifo(pipename, 0666) == -1 ){//Create an amount of numWorkers named pipes
            perror("mkfifo");

            closedir(folder);
            free(input_dir);
            exit(1);
        }

        if( ( w2a_fd_array[0][i]=open(pipename, O_RDONLY |  O_NONBLOCK) ) < 0 ){
            perror("fifo open problem");
            free(input_dir);
            closedir(folder);

            exit(4);
        }


    }





/* Diamoirasmos fakelwn stous workers*/

    rewinddir(folder);//reset the position of a directory stream to the beginning of a directory

    i=0;
    int nwrite;

    while( ( direntp=readdir(folder) ) != NULL ){
        if(direntp->d_type==DT_DIR && strcmp( direntp->d_name, "." ) && strcmp( direntp->d_name, ".." ) ){//If it is a folder and its not the "." and ".." directory
            //printf("%s\n", direntp->d_name);


            //Write the folder name in the pipe with an fd of a fd_array position
            //Auth h texnikh Round-robin tha xrhsimopoih8ei gia ton diamoirasmo twn arxeiwn

            write_message(fd_array[i], direntp->d_name, buffersize);

            //At first we will store the number of folders we have in every worker
            w2a_fd_array[1][i] += 1;


            i < (numWorkers-1) ? i++ : (i=0);



        }
    }

/* Before we get the stats we have to know the total files number of the input dir */

    int total_files=0;
    int files_in_every_folder=0;//every folder has the same number o Regular files
    int flag=0;//will help me count the files_in_every_folder


    rewinddir(folder);//reset the position of a directory stream to the beginning of a directory

    DIR* contained_folder;
    struct dirent *confold;



    while( ( direntp=readdir(folder) ) != NULL ){//Get in the input_dir folder
        if(direntp->d_type==DT_DIR && strcmp( direntp->d_name, "." ) && strcmp( direntp->d_name, ".." ) ){//If it is a folder and its not the "." and ".." directory
            //Count the files of each folder contained in the inout_dir folder
            char* tempath;
            tempath = (char*)malloc( (strlen(input_dir)+1+strlen(direntp->d_name)+1)*sizeof(char) ); // create a path input_dir/direntp->d_name
            sprintf(tempath, "%s/%s", input_dir, direntp->d_name);

            if( ( contained_folder = opendir(tempath) ) == NULL ){//If there is a problem while opening the contained folder
                fprintf(stderr, "Cannot open %s \n", tempath);

                //Close the program and let the user try again
                closedir(folder);
                free(tempath);
                exit(1);
            }else{//everything is ok
                //Count the number of files of the specific folder
                while( ( confold=readdir(contained_folder) ) != NULL ){
                    if( confold->d_type==DT_REG ){//Check for files in the folder
                        total_files++;
                        if(flag==0) files_in_every_folder++;

                    }
                }

            }

            //printf("\n\n TOTAL FILES %d\n\n", total_files);
            //printf("\n\n TOTAL FILES %d\n\n", files_in_every_folder);


            flag++;//Dont increment the files_in_every_folder again

            closedir(contained_folder);
            free(tempath);
        }
    }

    //Now lets multiply the numebr of folders of each worker with the numebr of files every folder has | That's the number of summary stats,every worker will send
    for(i=0 ; i<numWorkers ; i++){
        w2a_fd_array[1][i] *= files_in_every_folder;
        //printf("%d\n\n", w2a_fd_array[1][i]);
    }




//Now we got the total files
    //Sort the w2a_fd_array array
    int z;
    int maxfd;
    for(z=0;z<numWorkers;z++){


        if(z==0){
            maxfd = w2a_fd_array[0][z];
        }else if(maxfd < w2a_fd_array[0][z]){
            maxfd = w2a_fd_array[0][z];
        }

    }


    int completed=0;//This will let us know whether we read the stats of all the files

/*  Get the stats fromt he worker2aggx pipe(x is the worker number) */

    for (;;){

        fd_set fds;

        int res;
        char buf[buffersize];

        FD_ZERO(&fds); // Clear FD set for select

        for(i=0 ; i<numWorkers ; i++){

            if(w2a_fd_array[1][i]!=0){

                FD_SET(w2a_fd_array[0][i], &fds);
            }else{
                //Do nothing
                //printf("\n\n\nmeta apo 0\n\n");
            }

        }









        select(maxfd + 1, &fds, NULL, NULL, NULL);



        for(i=0 ; i<numWorkers ; i++){

            if (FD_ISSET(w2a_fd_array[0][i], &fds)){

                // We can read from fileDescriptor1
                memset(buf, 0, sizeof(buf));

                res = read(w2a_fd_array[0][i], buf, buffersize);
                if (res > 0)
                {
                        char* ti;
                        //printf("TTTIII %d        %s\n\n", i, buf);
                        ti = read_message(w2a_fd_array[0][i], buf, buffersize);//My pipe read message protocol
                        printf("\n%s\n", ti);



                        w2a_fd_array[1][i]--;


                        free(ti);
                        sleep(1);

                        completed +=1;

                    }



              }




        }









        for(i=0 ; i<numWorkers ; i++){

            if(w2a_fd_array[1][i]==0){
                //printf("\n\n\negina 0 %d\n\n\n", i);


                FD_CLR(w2a_fd_array[0][i], &fds);
            }

        }

        //printf("completed %d\n", completed);
        if(completed==total_files){
          //If you got the stats of all the files,break
            break;
        }


    }

    printf("YEAH\n\n");




//By now,we have printed ALL the summary stats nad we are ready to answer some user queries

/*                 ------Queries------                  */

    char* what_to_do = (char*)malloc(30*sizeof(char));

    while(1){

            char* line=NULL;
            size_t len;

            getline(&line, &len, stdin);//Get the [date1 date2]
            line[strlen(line)-1] = '\0';//Do this to remove the new line character from the end of the line string

            for(i=0 ; i<numWorkers ; i++){

                write_message(fd_array[i], line, buffersize);

            }


            //Let's get the query name
            char* token;//Will use it to split the string and get the query arguments
            token = strtok(line, " ");

            //First word is always the query name. token right now is the first word


//In case of /diseaseFrequency
            if(!strcmp(token, "/diseaseFrequency")){

                //Collect the total number of krousmata

                int total_krousmata=0;
//Do the select procedure to get the results from the workers
                completed=0;//mhdenizw to completed pou tha mou pei pote na stamathsw to select procedure(dhladh pote exw diavasei ta apotelesmata olwn twn workers)
                fd_set fds;
                FD_ZERO(&fds); // Clear FD set for select

                for (;;){


                    int maxfd;
                    int res;
                    char buf[buffersize];


                    for(i=0 ; i<numWorkers ; i++){
                        FD_SET(w2a_fd_array[0][i], &fds);
                    }

                    int z;
                    int temp;

                    //Sort the w2a_fd_array array
                    for(z=0;z<numWorkers;z++){
                        if(z==0){
                            maxfd = w2a_fd_array[0][z];
                        }else if(maxfd < w2a_fd_array[0][z]){
                            maxfd = w2a_fd_array[0][z];
                        }

                    }




                    select(maxfd + 1, &fds, NULL, NULL, NULL);



                    for(i=0 ; i<numWorkers ; i++){

                        if (FD_ISSET(w2a_fd_array[0][i], &fds)){

                            // We can read from fileDescriptor1
                            res = read(w2a_fd_array[0][i], buf, buffersize);
                            if (res > 0)
                            {
                                    char* ti;
                                    ti = read_message(w2a_fd_array[0][i], buf, buffersize);
                                    total_krousmata += atoi(ti);

                                    free(ti);

                                    completed +=1;

                                }


                            }







                    }


                    if(completed==numWorkers){//If you got the results from all the workers,break
                        break;
                    }


                }

                //Print the result
                printf("%d\n", total_krousmata);


                //free(line);

            }else if(!strcmp(token, "/searchPatientRecord")){

                int found=0;
                int total_krousmata=0;
//Do the select procedure to get the results from the workers
                completed=0;//mhdenizw to completed pou tha mou pei pote na stamathsw to select procedure(dhladh pote exw diavasei ta apotelesmata olwn twn workers)

                for (;;){

                    fd_set fds;
                    int maxfd;
                    int res;
                    char buf[buffersize];

                    FD_ZERO(&fds); // Clear FD set for select

                    for(i=0 ; i<numWorkers ; i++){
                        FD_SET(w2a_fd_array[0][i], &fds);
                    }

                    int z;
                    int temp;

                    //Sort the w2a_fd_array[0] array
                    for(z=0;z<numWorkers;z++){
                        if(z==0){
                            maxfd = w2a_fd_array[0][z];
                        }else if(maxfd < w2a_fd_array[0][z]){
                            maxfd = w2a_fd_array[0][z];
                        }

                    }




                    select(maxfd + 1, &fds, NULL, NULL, NULL);



                    for(i=0 ; i<numWorkers ; i++){

                        if (FD_ISSET(w2a_fd_array[0][i], &fds)){

                            // We can read from fileDescriptor1
                            res = read(w2a_fd_array[0][i], buf, buffersize);
                            if (res > 0)
                            {
                                    char* ti;
                                    ti = read_message(w2a_fd_array[0][i], buf, buffersize);
                                    if(strcmp(ti, "NO")){//If you just got the result
                                        //print it
                                        found=1;
                                        printf("%s\n", ti);
                                    }

                                    free(ti);

                                    completed +=1;

                                }


                            }







                    }


                    if(completed==numWorkers){//If you got the results from all the workers,break
                        break;
                    }


                }//Select for loop

                if(found==0){//If there was not such patient
                    printf("No patient with such record id was found!\n");
                }

            }else if((!strcmp(token, "/numPatientAdmissions")) || (!strcmp(token, "/numPatientDischarges"))){

                int found=0;
                int total_krousmata=0;
//Do the select procedure to get the results from the workers
                completed=0;//mhdenizw to completed pou tha mou pei pote na stamathsw to select procedure(dhladh pote exw diavasei ta apotelesmata olwn twn workers)

                for (;;){

                    fd_set fds;
                    int maxfd;
                    int res;
                    char buf[buffersize];

                    FD_ZERO(&fds); // Clear FD set for select

                    for(i=0 ; i<numWorkers ; i++){
                        FD_SET(w2a_fd_array[0][i], &fds);
                    }

                    int z;
                    int temp;

                    //Sort the w2a_fd_array[0] array
                    for(z=0;z<numWorkers;z++){
                        if(z==0){
                            maxfd = w2a_fd_array[0][z];
                        }else if(maxfd < w2a_fd_array[0][z]){
                            maxfd = w2a_fd_array[0][z];
                        }

                    }




                    select(maxfd + 1, &fds, NULL, NULL, NULL);



                    for(i=0 ; i<numWorkers ; i++){

                        if (FD_ISSET(w2a_fd_array[0][i], &fds)){

                            // We can read from fileDescriptor1
                            res = read(w2a_fd_array[0][i], buf, buffersize);
                            if (res > 0)
                            {
                                    char* ti;
                                    ti = read_message(w2a_fd_array[0][i], buf, buffersize);
                                    if(strcmp(ti, "NO")){//If you just got the result
                                        //print it

                                        found=1;
                                        printf("%s\n", ti);
                                    }

                                    free(ti);

                                    completed +=1;

                                }


                            }







                    }


                    if(completed==numWorkers){//If you got the results from all the workers,break
                        break;
                    }


                }//Select for loop

                if(!found){//If there was not such patient
                    printf("No such country was found!\n");
                }

            }else if(!strcmp(token, "/listCountries")){

                int found=0;

//Do the select procedure to get the results from the workers
                completed=0;//mhdenizw to completed pou tha mou pei pote na stamathsw to select procedure(dhladh pote exw diavasei ta apotelesmata olwn twn workers)

                for (;;){

                    fd_set fds;
                    int maxfd;
                    int res;
                    char buf[buffersize];

                    FD_ZERO(&fds); // Clear FD set for select

                    for(i=0 ; i<numWorkers ; i++){
                        FD_SET(w2a_fd_array[0][i], &fds);
                    }

                    int z;
                    int temp;

                    //Sort the w2a_fd_array[0] array
                    for(z=0;z<numWorkers;z++){
                        if(z==0){
                            maxfd = w2a_fd_array[0][z];
                        }else if(maxfd < w2a_fd_array[0][z]){
                            maxfd = w2a_fd_array[0][z];
                        }

                    }




                    select(maxfd + 1, &fds, NULL, NULL, NULL);



                    for(i=0 ; i<numWorkers ; i++){

                        if (FD_ISSET(w2a_fd_array[0][i], &fds)){

                            // We can read from fileDescriptor1
                            res = read(w2a_fd_array[0][i], buf, buffersize);
                            if (res > 0)
                            {
                                    char* ti;
                                    ti = read_message(w2a_fd_array[0][i], buf, buffersize);
                                    if(strcmp(ti, "NO")){//If you just got the result
                                        //print it

                                        found=1;
                                        printf("%s\n", ti);
                                    }

                                    free(ti);

                                    completed +=1;

                                }


                            }







                    }


                    if(completed==numWorkers){//If you got the results from all the workers,break
                        break;
                    }


                }//Select for loop



            }else if(!strcmp(token, "/exit")){
              send_signal(SIGQUIT, pidar, numWorkers);
              free(line);
              break;

            }else if(!strcmp(token, "/topk-AgeRanges")){

                int found=0;

//Do the select procedure to get the results from the workers
                completed=0;//mhdenizw to completed pou tha mou pei pote na stamathsw to select procedure(dhladh pote exw diavasei ta apotelesmata olwn twn workers)

                for (;;){

                    fd_set fds;
                    int maxfd;
                    int res;
                    char buf[buffersize];

                    FD_ZERO(&fds); // Clear FD set for select

                    for(i=0 ; i<numWorkers ; i++){
                        FD_SET(w2a_fd_array[0][i], &fds);
                    }

                    int z;
                    int temp;

                    //Sort the w2a_fd_array[0] array
                    for(z=0;z<numWorkers;z++){
                        if(z==0){
                            maxfd = w2a_fd_array[0][z];
                        }else if(maxfd < w2a_fd_array[0][z]){
                            maxfd = w2a_fd_array[0][z];
                        }

                    }




                    select(maxfd + 1, &fds, NULL, NULL, NULL);



                    for(i=0 ; i<numWorkers ; i++){

                        if (FD_ISSET(w2a_fd_array[0][i], &fds)){

                            // We can read from fileDescriptor1
                            res = read(w2a_fd_array[0][i], buf, buffersize);
                            if (res > 0)
                            {
                                    char* ti;
                                    ti = read_message(w2a_fd_array[0][i], buf, buffersize);
                                    if(strcmp(ti, "NO")){//If you just got the result
                                        //print it

                                        found=1;
                                        printf("%s\n", ti);
                                    }

                                    free(ti);

                                    completed +=1;

                                }


                            }







                    }


                    if(completed==numWorkers){//If you got the results from all the workers,break
                        break;
                    }


                }//Select for loop

            }

            free(line);

    }



    free(pidar);
    for(i=0 ; i<numWorkers ; i++){
        memset(pipename, 0 ,sizeof(pipename));

        sprintf(pipename, "pipenumber%d", i);
        unlink(pipename);

        memset(pipename, 0, sizeof(pipename));

        sprintf(pipename, "worker2agg%d", i);
        unlink(pipename);
    }


    free(what_to_do);
    free(input_dir);
    closedir(folder);
    return 0;
}

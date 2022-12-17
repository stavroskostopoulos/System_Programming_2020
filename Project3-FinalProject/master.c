
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
#include<sys/wait.h>

#include "hashlib.h"
#include "kouvas.h"
#include "dentro.h"
#include "topk.h"
#include "dateandlist.h"
#include "functions.h"


int main(int argc, char *argv[]){

    printf("\n\n");




/*Command line arguments check*/

    if(argc!=11){
        printf("Please check your command line arguments.\n\n\n");
        return 1;
    }


/*Get command line arguments*/



    int i;
    int numWorkers=0;
    int buffersize=0;
    char* input_dir;
    char* serverIP;
    char* serverPort;

    for(i=0 ; i<11 ; i++){

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

        //Get serverIP
        if( !strcmp(argv[i],"-s") ){
            serverIP=strdup(argv[i+1]);
            printf("ServerIP: %s\n", serverIP);
        }

        //Get serverPort
        if( !strcmp(argv[i],"-p") ){
            serverPort=strdup(argv[i+1]);
            printf("serverPort: %s\n", serverPort);
        }


    }

/*Create the worker processes and let them wait for the master to open the pipes*/
    pid_t pid;
    pid_t wpid;
    int status = 0;

    for(i=0 ; i<numWorkers ; i++){

        pid = fork();

        if( pid==-1 ){
            perror("Problem with fork");\

            //Close the program and let the user try again
            free(serverPort);
            free(serverIP);
            free(input_dir);
            exit(1);
        }


        if( pid==0 ){//Child


            char arg_str[100];
            sprintf(arg_str, "%d %d %s", i, buffersize, input_dir);

            char *argums[]={"./worker", arg_str, NULL};

            execv(argums[0], argums);



            perror("Exec fail");

            //Close the program and let the user try again
            free(serverPort);
            free(serverIP);
            free(input_dir);


            exit(29);
        }else{//Parent
            //Do nothing
        }


    }




/* Create the pipes */

    char pipename[12];//Pipename is: pipenumberx ,where x is the worker number.It consists of 11 characters + 1 for the terminal character
    int fd_array[numWorkers];//An array which contains all the fd of the workers(diseaseAggregator to worker pipes)
    int w2a_fd_array[numWorkers];///An array which contains all the fd of the workers (workers to Aggregator pipes)| The second line of the array will let us know how many sumamry stats we are gonna get

    for(i=0 ; i<numWorkers ; i++){

        sprintf(pipename, "pipenumber%d", i);

        if( mkfifo(pipename, 0666) == -1 ){//Create an amount of numWorkers named pipes
            perror("mkfifo");


            free(serverPort);
            free(serverIP);
            free(input_dir);
            exit(1);
        }


   }




   for(i=0 ; i<numWorkers ; i++){

       sprintf(pipename, "pipenumber%d", i);



       if( ( fd_array[i]=open(pipename, O_WRONLY) ) < 0 ){
           perror("fifo open problem");
           free(input_dir);
           free(serverPort);
           free(serverIP);


           exit(4);
       }


   }


/*Count the files in the input_dir directory*/
    DIR* folder;
    struct dirent *direntp;

    int filecount=0;//The number of directory files in the input_dir directory

    if( ( folder = opendir(input_dir) ) == NULL ){//If there is a problem while opening the input_dir directory
        fprintf(stderr, "Cannot open %s \n", input_dir);

        //Close the program and let the user try again
        free(serverPort);
        free(serverIP);
        free(input_dir);
        exit(1);
    }else{

        while( ( direntp=readdir(folder) ) != NULL ){
            if(direntp->d_type==DT_DIR && strcmp( direntp->d_name, "." ) && strcmp( direntp->d_name, ".." ) ){//If it is a folder and its not the "." and ".." directory count it
                filecount++;
            }
        }

    }


/* Diamoirasmos fakelwn stous workers*/

    rewinddir(folder);//reset the position of a directory stream to the beginning of a directory

    i=0;
    int nwrite;



    char** folnamearr;//An arrray that the messages of each worker will be sent
    folnamearr = (char**)malloc(numWorkers*sizeof(char*));

    while( ( direntp=readdir(folder) ) != NULL ){
        if(direntp->d_type==DT_DIR && strcmp( direntp->d_name, "." ) && strcmp( direntp->d_name, ".." ) ){//If it is a folder and its not the "." and ".." directory
            //printf("%s\n", direntp->d_name);


            //Write the folder name in the pipe with an fd of a fd_array position
            //Auth h texnikh Round-robin tha xrhsimopoih8ei gia ton diamoirasmo twn arxeiwn

            //write_message(fd_array[i], direntp->d_name, buffersize);

            if(folnamearr[i]==NULL){
                folnamearr[i]=(char*)malloc ( (strlen(direntp->d_name)+1)*sizeof(char));
                strcpy(folnamearr[i], direntp->d_name);

            }else{
                char* temp;
                temp = strdup(folnamearr[i]);

                free(folnamearr[i]);

                folnamearr[i] = (char*)malloc ( (strlen(temp)+1/*@*/+strlen(direntp->d_name)+1)*sizeof(char));
                strcpy(folnamearr[i], temp);
                strcat(folnamearr[i], "@");
                strcat(folnamearr[i], direntp->d_name);

                free(temp);
            }


            i < (numWorkers-1) ? i++ : (i=0);



        }
    }

    //Now on each string of the folnamearr i have to add the serverPort and the serverIP
    char* serverinfo;
    serverinfo = malloc((1+strlen(serverIP)+1+strlen(serverPort)+1+1)*sizeof(char));

    sprintf(serverinfo, "$%s#%s!", serverIP, serverPort);


    for(i=0 ; i<numWorkers ; i++){
        char* temp;
        temp = strdup(folnamearr[i]);

        free(folnamearr[i]);

        folnamearr[i] = (char*)malloc( (strlen(temp)+strlen(serverinfo)+1)*sizeof(char) );
        strcpy(folnamearr[i], temp);
        strcat(folnamearr[i], serverinfo);

        free(temp);
    }



    //Now each string of the folnamearr is like this: countryname@countryname$serverIP#serverPort!


    for(i=0 ; i<numWorkers ; i++){//Write each folnamearr string to the worker it refers to

        write_message(fd_array[i], folnamearr[i], buffersize);

    }



    while ((wpid = wait(&status)) > 0); // this way, the father waits for all the child processes

/* Free the allocated memory */
    free(serverinfo);
    free(serverPort);
    free(input_dir);
    free(serverIP);
    for(i=0 ; i<numWorkers ; i++){
        free(folnamearr[i]);
        close(fd_array[i]);
    }
    free(folnamearr);
    closedir(folder);

}

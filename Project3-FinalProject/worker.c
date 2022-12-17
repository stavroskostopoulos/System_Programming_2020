
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

#include <netdb.h>
#include<netinet/in.h>/*  for  sockaddr_in  */
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>/*  for  hton* */

#include "hashlib.h"
#include "kouvas.h"
#include "dentro.h"
#include "topk.h"
#include "dateandlist.h"
#include "functions.h"



#define BUCKETSIZE 80

int main(int argc, char *argv[]){

    //sleep(1);

//Get the command line arguments
    int workernumber;// is the number of the worker
    int buffersize; //is the buffersize
    char input_dir[15];//the name of the parent folder

    sscanf(argv[1], "%d %d %s", &workernumber, &buffersize, input_dir);



    char pipename[12];//Pipename is: pipenumberx ,where x is the worker number.It consists of 11 characters + 1 for the terminal character

//Generate the diseaseAggregator(master) to Worker pipename
    sprintf(pipename, "pipenumber%d", workernumber);

    int i=0;

    int worker_fd;//The fd of this worker (Aggregator to worker communication)

    worker_fd = open(pipename,  O_RDONLY);//Here I want it to block and wait for the other side of the pipe to open


    char* pipe_message;

    pipe_message = read_message(worker_fd, buffersize);
    //pipe_message is something like China@Turkey@Greece$123213#53434!

    //printf("%s\n", pipe_message);

//Lets count how many countries we have to manage so we can create an array of these countries

    char* tempstr = strdup(pipe_message);//So i can protect the pipe_message
    int countrycounter=0;//The counter of the countries

    char* token;
    token = strtok(tempstr, "@");

    while(token!=NULL){
        countrycounter++;
        token = strtok(NULL, "@");//Get the next token
    }

    free(tempstr);
    //printf("I got %d countries\n", countrycounter);


//Now its time to store all the data we got from the master to worker pipe ("pipenumberx")

    char* tempcountrysting;//A string to temporalily keep the countries ( example. China@Turkey@Greece )
    char* tempserverinfo;//A string to temporalily keep the server info ( contains serverIP and serverPort )

    char** countryarray;//An array to keep the names of the countries we have to go through
    countryarray = (char**)malloc(countrycounter*sizeof(char*));

    char* serverIP;
    char* serverPort;

    i=0;

    token = strtok(pipe_message, "$");

    while(token!=NULL){

        if(i==0) tempcountrysting = strdup(token);
        if(i==1) tempserverinfo = strdup(token);

        i++;
        token = strtok(NULL, "$");//Get the next token
    }





    //This one is to get the countries one by one from tempcountrysting
    token = strtok(tempcountrysting, "@");

    for(i=0 ; i<countrycounter ; i++){//Store each country in teh country array

        countryarray[i] = strdup(token);
        token = strtok(NULL, "@");//Get the next token
    }


    //This one is to get the serverIP and the serverPort
    token = strtok(tempserverinfo, "#");

    i=0;
    while(token!=NULL){
        if(i==0) serverIP = strdup(token);
        if(i==1) serverPort = strdup(token);

        i++;
        token = strtok(NULL, "#");//Get the next token
    }

    serverPort[(strlen(serverPort))-1]='\0';//Get rid of the '!'

    for(i=0 ; i<countrycounter ; i++){
        printf("%s %d\n", countryarray[i], workernumber);
    }
    printf("%s %s\n", serverIP, serverPort);

/*------------------------- Now we stored everything the master sent us-----------------------------------*/

/* Let's open the folder (names of folders stored in countryarray) and get the summary stats for each of the files */
    char* summary_stats;


    int firsttime=0;//A flag to let us know when we received a new folder name that we should manage | Some things(such as mallocs) have to be done only once
    int flag=0;

    Bucket_ptr *recordhash;//record hash table
    Bucket_ptr *diseasehash;//disease hash table
    Bucket_ptr *countryhash;//country hash table

    ptrList root;//The root node of the patient list
    ptrList temproot;//A temporary list,for ever new file so we add only this list's patients to the hash tables | Will be attached to root after all this

    char* path;



    int l=0;
    for(l=0 ; l<countrycounter ; l++){//countryarray[l]
        //Create the path
        path = (char*)malloc( (strlen(input_dir)+1+strlen(countryarray[l])+1)*sizeof(char) );//argv[2]->input_dir
        sprintf(path, "%s/%s", input_dir, countryarray[l]);


        DIR* folder;
        struct dirent *direntp;

        //OPEN FOLDER
        if( ( folder = opendir(path) ) == NULL ){//If there is a problem while opening the folder
            fprintf(stderr, "Cannot open %s \n", path);

            //Close the program and let the user try again
            free(path);
            break;


        }else{//Open the folder


            free(path);

/* Sort directory by date */

            int filecount=0;//The number of directory files in the folder directory

            //Count the number of files
            while( ( direntp=readdir(folder) ) != NULL ){
                if( direntp->d_type==DT_REG ){//Check for files in the folder
                    filecount++;
                }
            }

            //Create an array that will keep the names of the folders
            char** date;
            date = (char**)malloc(filecount*sizeof(char*));



            rewinddir(folder);//reset the position of a directory stream to the beginning of a directory

            //Store each date of the folder in the date array
            int k=0;
            while( ( direntp=readdir(folder) ) != NULL ){
                if( direntp->d_type==DT_REG  ){//Check for files in the folder
                    date[k]=(char*)malloc( (strlen(direntp->d_name)+1)*sizeof(char) );
                    strcpy(date[k], direntp->d_name);
                    //date[k]=strdup(direntp->d_name);

                    k++;

                }
            }

            int files_perdir=k;

            k=0;


            int z,j;
            char* temp;
            //Sort the array of dates
            for(z=0;z<filecount;z++){
                for(j=z+1;j<filecount;j++){

                    ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                    ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                    setDate(tempdate1, date[z]);
                    setDate(tempdate2, date[j]);

                    if(compare_dates(tempdate1, tempdate2)==1){


                        temp = date[z];
                        date[z] = date[j];
                        date[j] = temp;



                    }

                    free(tempdate1);
                    free(tempdate2);
                }
            }



/* Before we start reading the files */

            /* RecordId hashtable creation */
            if(firsttime==0){
                recordhash = (Bucket_ptr*)malloc(sizeof(Bucket_ptr)*RECORDHASHENTRIES);

                for(z=0 ; z<RECORDHASHENTRIES ; z++){//Create all the buckets of the hash table
                    recordhash[z] = create_bucket_in_hashtable(BUCKETSIZE);
                }
            }

/*----------------------------*/


/* ----------------------------------*/

/* Open the files and add the data to the data structures */
            for(j=0 ; j<filecount ; j++){//Take one by one the file names sorted in the date array

                //sleep(1);

                //Construct the path of the specific file


                //Something like input_dir/message/date[j]
                path = (char*)malloc( (strlen(input_dir)+1+strlen(countryarray[l])+1+strlen(date[j])+1)*sizeof(char) );//argv[2]->input_dir
                sprintf(path, "%s/%s/%s", input_dir, countryarray[l], date[j]);


                //Open it
                FILE* file;
                if( (file = fopen(path, "r")) == NULL ){//If there is a problem while opening the file
                    fprintf(stderr, "Cannot open %s \n", path);

                    //Close the program and let the user try again
                    closedir(folder);

                    //free the date array
                    for(i=0 ; i<filecount ; i++){
                        free(date[i]);
                    }
                    free(date);
                    //free the country array
                    for(i=0 ; i<countrycounter ; i++){
                        free(countryarray[i]);
                    }
                    free(countryarray);

                    free(pipe_message);
                    free(tempcountrysting);
                    free(tempserverinfo);
                    free(path);
                    exit(1);

                }

                free(path);

                //We opened the file!Let's start!
                char* recordID = (char*)malloc(20*sizeof(char));
                char* patientFirstName = (char*)malloc(13*sizeof(char));//Afou ta onomata pou ginontai generate apo to script einai apo 3 ews 12 chars
                char* patientLastName = (char*)malloc(13*sizeof(char));//Afou ta onomata pou ginontai generate apo to script einai apo 3 ews 12 chars
                char* diseaseID = (char*)malloc(20*sizeof(char));
                char* country = (char*)malloc(20*sizeof(char));
                char* age = (char*)malloc(4*sizeof(char));
                char* status = (char*)malloc(6*sizeof(char));//ENTER or EXIT


                ptrList temp;//To attach the new nodes of the list


                Bucket_node_ptr temp_bnode;
                unsigned int hash_number;//The number that the hash function returns

                i=0;

                /* Get the records from the patientfile until you get EOF */
                while(fscanf(file, "%s %s %s %s %s %s\n", recordID, status, patientFirstName, patientLastName, diseaseID, age)!=EOF){//Get the records from the patientfile until you get EOF



                    if(i==0){//For the first record create the first node

                        if( firsttime==0 ){//If it is the first time(The first file we are handling)
                            root = createListNode(recordID, patientFirstName, patientLastName, diseaseID, countryarray[l], date[j], age, status);//message->country name | date[j]->date (will figure out in the function if it is the ENTRY or EXIT date)
                            //Insert it to the record hash table

                            recordID_to_record_hash_insert( root, recordID, recordhash, BUCKETSIZE);


                        }else{
                            temproot = createListNode(recordID, patientFirstName, patientLastName, diseaseID, countryarray[l], date[j], age, status);//message->country name | date[j]->date (will figure out in the function if it is the ENTRY or EXIT date)

                            //Insert it to the record hash table
                            recordID_to_record_hash_insert( temproot, recordID, recordhash, BUCKETSIZE);
                        }



                        i++;


                    }else{//If it is not the first record

                        //An prokeitai gia ENTER
                        if(!strcmp(status, "ENTER")){

                            //Check an uparxei
                            //If exists,ekypwsh la8ous kai synexise

                            //Lets check if a same recordId exists in any bucket
                            hash_number = hash(recordID)%RECORDHASHENTRIES;
                            if( (check_if_bucketnode_with_this_name_exists( recordID, recordhash, hash_number, BUCKETSIZE)==1) ){//If there is a recordID equal tothis one
                                printf("A patient with id: %s already exists and has not exited yet.This entry has been skipped.\n", recordID);
                                continue;
                            }else{

                                //If not exists,kanonikh eisagwgh sthn lista
                                temp = createListNode(recordID, patientFirstName, patientLastName, diseaseID, countryarray[l], date[j], age, status);

                                if(firsttime==0){
                                    attach_patient_to_List( root, temp);
                                }else{
                                    attach_patient_to_List( temproot, temp);
                                }


                                recordID_to_record_hash_insert( temp, recordID, recordhash, BUCKETSIZE);

                            }



                        }else{//An prokeitai gia EXIT

                            //Check an yparxei
                            //An den yparxei,ektypwsh la8ous kai continue

                            if( (check_if_bucketnode_with_this_name_exists( recordID, recordhash, hash_number, BUCKETSIZE)==0) ){//If there is a recordID equal tothis one
                                //printf("A patient with id: %s cannot EXIT because he/she never entered.This exit has been skipped.\n", recordID);
                                //fprintf(stderr, "ERROR\n");
                                continue;
                            }else{
                                //An yparxei check an to exitdate einai meta to entrydate,kai ananewsh ths metavlhths exit date sthn lista
                                temp_bnode = find_bucketnode(recordID, recordhash, hash_number, BUCKETSIZE);
                                ptrDate tempdate = (ptrDate)malloc(sizeof(myDate));
                                setDate(tempdate, date[j]);

                                if(compare_dates(temp_bnode->rbtree->patient->entryDate, tempdate)==1){// to exitDate(edw to date[j] afou status = EXIT) eiani prin to entrydate
                                    printf("A patient with id: %s has an exit date that's before his/her entry date.This exit has been skipped.", recordID);
                                    continue;
                                }else{//Ola kala me to exit date (date[j])

                                    if(temp_bnode->rbtree->patient->exitDate->day==0){//An den eixe exitDate
                                        //Apla enhmerwse to exitDate tou patient list node
                                        setDate(temp_bnode->rbtree->patient->exitDate, date[j]);
                                    }else{//an eixe exitDate already
                                        printf("A patient with id: %s already has an exit date.Exit date updated!\n", recordID);
                                        setDate(temp_bnode->rbtree->patient->exitDate, date[j]);
                                    }


                                }


                            }

                        }





                    }




                }


                free(recordID);
                free(patientFirstName);
                free(patientLastName);
                free(diseaseID);
                free(country);
                free(age);
                free(status);
                fclose(file);


/*Data from this date/file added to the patient list and the recordID hashtable*/



/*           Create the hash tables(ONLY ONCE) and manage them     */

            if(firsttime==0){
                //The mallocs must be done only once

                diseasehash = (Bucket_ptr*)malloc(sizeof(Bucket_ptr)*DISEASEHASHENTRIES);

                for(z=0 ; z<DISEASEHASHENTRIES ; z++){//Create all the buckets of the hash table
                    diseasehash[z] = create_bucket_in_hashtable(BUCKETSIZE);
                }


                countryhash = (Bucket_ptr*)malloc(sizeof(Bucket_ptr)*COUNTRYHASHENTRIES);

                for(z=0 ; z<COUNTRYHASHENTRIES ; z++){//Create all the buckets of the hash table
                    countryhash[z] = create_bucket_in_hashtable(BUCKETSIZE);
                }



                //print_patientList(root);


            }

            if(firsttime==0){//Pros8ese at stoixeia tou patient list "root" an eiani h prwth fora
                //Pass records to hash tables

                record_to_hash_tables_insert(root, diseasehash, countryhash, BUCKETSIZE, DISEASEHASHENTRIES, COUNTRYHASHENTRIES);

                firsttime=1;

            }else{//An den eiani h prwth fora, pros8ese sta hash tables ta stoixeia tou "temproot" patient list kai enwse to temproot list me to root list
                //Pass records to hash tables

                record_to_hash_tables_insert(temproot, diseasehash, countryhash, BUCKETSIZE, DISEASEHASHENTRIES, COUNTRYHASHENTRIES);
                attach_patient_to_List(root,temproot);

            }

/*------------------------------------------------------*/

/* Now we get the stats for the specific date (date[j]) for the specific country(message),and we traverse through the diseases (diseasehash)*/

            //Create the introduction of the stat for this file
            //05-09-1992
            //China
            char* introduction;
            introduction = (char*)malloc( (strlen(date[j])+1+strlen(countryarray[l])+1+1)*sizeof(char) );
            sprintf(introduction, "%s\n%s\n", date[j], countryarray[l]);







            char* tempstring;//To string pou 8a apo8hkeutoun ta summary stats
            tempstring = (char*)malloc( (1024)*sizeof(char) );

            sprintf(tempstring, "%s\n", introduction);//Add to introductions to tempstring (pou einai ta summary stats mou)


            //Convert date[j] string to date type so we can use it in some functions
            ptrDate datetemp;
            datetemp = (ptrDate)malloc(sizeof(myDate));
            setDate(datetemp, date[j]);

            //The age groups | Here we will save the number of cases we have
            int age_group1=0;//0-20
            int age_group2=0;//21-40
            int age_group3=0;//41-60
            int age_group4=0;//61+

            Bucket_ptr tempo;

            for(k=0 ; k<DISEASEHASHENTRIES ; k++){
                tempo=diseasehash[k];

                while(tempo!=NULL){//Pairnw ena ena ta buckets
                    for(z=0 ; z<(BUCKETSIZE/(sizeof(Bucket_node))) ; z++){//Pairnw ena ena ta bucket nodes(dhladh ta diseases)

                        if(tempo->bucket_node_array[z]!=NULL){


                            age_group1 = all_diseases_of_a_bucket_country_and_age_and_date(tempo->bucket_node_array[z], BUCKETSIZE, countryarray[l], 0, 20, datetemp);
                            age_group2 = all_diseases_of_a_bucket_country_and_age_and_date(tempo->bucket_node_array[z], BUCKETSIZE, countryarray[l], 21, 40, datetemp);
                            age_group3 = all_diseases_of_a_bucket_country_and_age_and_date(tempo->bucket_node_array[z], BUCKETSIZE, countryarray[l], 41, 60, datetemp);
                            age_group4 = all_diseases_of_a_bucket_country_and_age_and_date(tempo->bucket_node_array[z], BUCKETSIZE, countryarray[l], 61, 500, datetemp);
                            if( (age_group1) || (age_group2) || (age_group3) || (age_group4) ){//an uphr3e auto to disease sto sugkekrtimeno file,tote sumperilamvanetai sta stats
                                sprintf(tempstring, "%s\n%s\nAge range 0-20 years: %d cases\nAge range 21-40 years: %d cases\nAge range 41-60 years: %d cases\nAge range 60+ years: %d cases\n", tempstring, tempo->bucket_node_array[z]->name, age_group1, age_group2, age_group3, age_group4);
                                //printf("%d %d %d %d\n\n", age_group1, age_group2, age_group3, age_group4);


                            }


                        }

                    }

                    tempo=tempo->next;
                }

            }

            free(datetemp);

/*Let me add this to the summary stats string*/


            if(flag==0){
                summary_stats = strdup(tempstring);//tempstring carries the sumamrystats for this file
                flag=1;
            }else{
                tempstr = strdup(summary_stats);

                free(summary_stats);
                summary_stats = (char*)malloc( (strlen(tempstr)+2+strlen(tempstring)+1)*sizeof(char) );

                sprintf(summary_stats, "%s\n\n%s", tempstr, tempstring);

                free(tempstr);
            }






            free(introduction);
            free(tempstring);


        }//done with all the files of this folder

        closedir(folder);
        int x=0;
        for(x=0 ; x<files_perdir ; x++){
          free(date[x]);
        }
        free(date);


      }//Everything went well with the folder

    }//Check the other folder ( here the for loop with the countryarray ends)

    sleep(1);

    //printf("%s\n--------------------------------------------------------------------\n\n", summary_stats);

/* We just combined all the summary stats for thsi worker and we can move on */

/* Let's combine the sumamry stats with this worker's port number */

    //First we gotta open a socket to generate the port number
    int workerfd, new_socket_worker;
    struct sockaddr_in worker_address;
    int opt=1;//To enable the socket options
    int addrlen = sizeof(worker_address);



    // Creating socket file descriptor
    if ((workerfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(workerfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    worker_address.sin_family = AF_INET;
    worker_address.sin_addr.s_addr = INADDR_ANY;
    worker_address.sin_port = htons(0); /* bind() will choose a random port*/

    if (bind(workerfd, (struct sockaddr *)&worker_address, sizeof(worker_address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(workerfd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }


//Used this to find out which port was given to thsi worker ( 0 generates a random available port number)
//Trusted this source:
//stackoverflow.com/questions/4046616/sockets-how-to-find-out-what-port-and-address-im-assigned

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(workerfd, (struct sockaddr *)&sin, &len) == -1){
        perror("getsockname");
    }else{
        printf("Port number of worker #%d: %d\n\n", workernumber, ntohs(sin.sin_port));
    }



/* Let's consstruct the final string to send to the whoServer (workerportnum+sumamrystats) */
    char portnumber[6];
    sprintf(portnumber, "%d", ntohs(sin.sin_port));

    char* final_server_message;
    final_server_message = (char*)malloc( (strlen(portnumber)+1/*@*/+strlen(summary_stats)+1)*sizeof(char) );

    sprintf(final_server_message, "%s@%s", portnumber, summary_stats);



/* Let's connect to whoServer to send the summary stats and this worker's port number */


    /* Now i want to bind()  to a specific IP (which was printed in the execution of the WhoServer)*/
    //To bind to a specific IP adress i used this source:
    //www.geeksforgeeks.org/socket-programming-cc/

    int servPort = atoi(serverPort);

    int sockfd;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){    perror("socket");   exit(-1); }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(servPort);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, serverIP, &serv_addr.sin_addr)<=0){    perror("Invalid address/ Address not supported"); exit(-1); }


    //Attempt to connect() to whoServer
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){    perror("Connect"); exit(-1); }


    printf("Worker number %d: successfully connected to whoServer!\n\n", workernumber);

    write_message(sockfd, final_server_message, buffersize);


    close(sockfd);



    /* Now we wait for connection with the thread that got our summary stats and port number*/
    printf("Worker number #%d is waiting for the whoServer to connect...\n\n", workernumber);
    if ((new_socket_worker = accept(workerfd, (struct sockaddr *)&worker_address, (socklen_t*)&addrlen))<0){
            perror("accept");
            exit(EXIT_FAILURE);
    }
    printf("The whoServer connected to worker number #%d!\n\n", workernumber);


/* Now you will start getting queries */

    while(1){
        char* msg;
        msg = read_message(new_socket_worker, buffersize);

        char* tok;//Will use it to split the string and get the query arguments
        tok = strtok(msg, " ");


        printf("Worker #%d got a message: %s\n", workernumber, msg);

        if(!strcmp(tok, "/diseaseFrequency")){

            int count=0;//To help us store the arguments

            char* virusname;
            char* arg2;
            char* arg3;
            char* arg4;

            while(tok != NULL){

                if(count==1){
                    virusname = strdup(tok);//virusname
                }else if(count==2){
                    arg2 = strdup(tok);// date1
                }else if(count==3){
                    arg3 = strdup(tok);// date2

                }else if(count==4){// Country was given
                    arg4 = strdup(tok);
                }

                count++;
                tok = strtok(NULL, " ");//Get the next token
            }

            int disease_hash_position=0;
            int country_hash_position=0;



            if(count==2){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(virusname);

            }else if(count==3){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(virusname);
                free(arg2);

            }else if(count==4){
                //No country was given
                //arg2 = date1  arg3 = date2

                disease_hash_position = hash(virusname)%DISEASEHASHENTRIES;
                //printf("\n\nINSANE\n\n");
                //An o worker autos den exei asxolithei me to specific virus,apla steleni sto pipe 0
                if( (check_if_bucketnode_with_this_name_exists(virusname, diseasehash, disease_hash_position, BUCKETSIZE)==0) ){//Check if this virus exists

                    char krousmata[2] = "0";

                    write_message(new_socket_worker, "DEN PHRA TIPOTA GAMW", buffersize);

                    free(virusname);
                    free(arg2);
                    free(arg3);



                    //printf("\nNo such virus was found!\n");
                    continue;
                }
                //printf("\n\nINSANE\n\n");

                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(arg2)==0 || valid_date(arg3)==0 ){//Check if the dates given are valid
                    if(workernumber==0) printf("\nPlease type valid dates!\n");

                    free(tempdate1);
                    free(tempdate2);
                    free(arg2);
                    free(arg3);

                    continue;
                }



                setDate(tempdate1, arg2);
                setDate(tempdate2, arg3);

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        if(workernumber==0) printf("\nThe dates you typed are the same!\n");
                        free(tempdate1);
                        free(tempdate2);
                        free(virusname);
                        free(arg2);
                        free(arg3);

                        continue;
                    }else{//If date1>date2
                        if(workernumber==0) printf("\n%s is before %s\n", arg3, arg2);
                        free(tempdate1);
                        free(tempdate2);
                        free(virusname);
                        free(arg2);
                        free(arg3);

                        continue;
                    }
                }


                //Everyting is ok with the dates let's move
                int total_krousmata=0;
                total_krousmata = disease_frequency(diseasehash, disease_hash_position, virusname, BUCKETSIZE, tempdate1, tempdate2);

                //Count the numebr of digits so we knwo the length of the string we want to store the number in
                int digit_num=0;
                int temp_int=total_krousmata;
                //printf("krousmata\n\n");

                while (temp_int != 0) {
                    temp_int /= 10;
                    ++digit_num;
                }

                char* krousmata;

                krousmata = (char*)malloc( (digit_num+1)*sizeof(char) );
                //store the int to the string
                sprintf(krousmata, "%d", total_krousmata);
                //printf("krousmata 22222\n\n");

                //Write the number of virusname cases to the pipe
                write_message(new_socket_worker, krousmata, buffersize);


                //free(msg);
                free(virusname);
                free(tempdate1);
                free(tempdate2);
                free(arg2);
                free(arg3);

            }else if(count==5){//Country argument was given
                //arg4 = country arg2 = date1 arg3 = date2
                disease_hash_position = hash(virusname)%DISEASEHASHENTRIES;
                country_hash_position = hash(arg4)%COUNTRYHASHENTRIES;


                //An o worker autos den exei asxolithei me to specific country,apla stelnei sto pipe 0
                if( (check_if_bucketnode_with_this_name_exists( arg4, countryhash, country_hash_position, BUCKETSIZE)==0) ){//Check if this country exists

                    char krousmata[2] = "0";

                    write_message(new_socket_worker, krousmata, buffersize);
                    //write_message(new_socket_worker, krousmata, buffersize);
                    //printf("PETYXAINWWWW\n" );
                    free(virusname);
                    free(arg2);
                    free(arg3);
                    free(arg4);


                    //printf("\nNo such country was found!\n");
                    continue;
                }


                //An o worker autos den exei asxolithei me to specific virus,apla stelnei sto pipe 0
                if( (check_if_bucketnode_with_this_name_exists(virusname, diseasehash, disease_hash_position, BUCKETSIZE)==0) ){//Check if this virus exists

                    char krousmata[2] = "0";

                    write_message(new_socket_worker, krousmata, buffersize);

                    free(virusname);
                    free(arg2);
                    free(arg3);
                    free(msg);
                    free(arg4);


                    //printf("\nNo such virus was found!\n");
                    continue;
                }

                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(arg2)==0 || valid_date(arg3)==0 ){//Check if the dates given are valid
                    if(workernumber==0) printf("\nPlease type valid dates!\n");

                    free(tempdate1);
                    free(tempdate2);
                    free(arg2);
                    free(arg3);
                    free(msg);
                    continue;
                }



                setDate(tempdate1, arg2);
                setDate(tempdate2, arg3);

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        if(workernumber==0) printf("\nThe dates you typed are the same!\n");
                        free(tempdate1);
                        free(tempdate2);
                        free(virusname);
                        free(arg2);
                        free(arg3);

                        continue;
                    }else{//If date1>date2
                        if(workernumber==0) printf("\n%s is before %s\n", arg3, arg2);
                        free(tempdate1);
                        free(tempdate2);
                        free(virusname);
                        free(arg2);
                        free(arg3);

                        continue;
                    }
                }



                //Everyting is ok with the dates let's move


                int total_krousmata=0;
                total_krousmata = disease_frequency_bycountry(diseasehash, disease_hash_position, virusname, BUCKETSIZE, tempdate1, tempdate2, arg4);

                //Count the numebr of digits so we knwo the length of the string we want to store the number in
                int digit_num=0;
                int temp_int=total_krousmata;
                //printf("krousmata\n\n");

                while (temp_int != 0) {
                    temp_int /= 10;
                    ++digit_num;
                }

                char* krousmata;
                krousmata = (char*)malloc( (digit_num+1)*sizeof(char) );
                //store the int to the string
                sprintf(krousmata, "%d", total_krousmata);
                //printf("krousmata 22222\n\n");

                //Write the number of virusname cases to the pipe
                write_message(new_socket_worker, krousmata, buffersize);



                free(tempdate1);
                free(tempdate2);
                free(virusname);
                free(arg2);
                free(arg3);
                free(arg4);

            }
        }else if(!strcmp(tok, "/searchPatientRecord")){
            int count=0;//To help us store the arguments
            printf("MPHKA GIA SEARCH\n\n\n");
            char* rec_id;


            while(tok != NULL){

                if(count==1){
                    rec_id = strdup(tok);//get the record ID
                }

                count++;
                tok = strtok(NULL, " ");//Get the next token
            }

            if(count!=2){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(msg);
                if(count>2){
                    free(rec_id);
                }
                continue;
            }

            unsigned int hash_number;//The number that the hash function returns


            hash_number = hash(rec_id)%RECORDHASHENTRIES;
            if( (check_if_bucketnode_with_this_name_exists( rec_id, recordhash, hash_number, BUCKETSIZE)!=1) ){//If there is NOT such a recordID in the record hash table
                //Let the aggregator know that you got no such record id
                write_message(new_socket_worker, "NO", buffersize);

                //Just ignore the query
                continue;
            }
            //Get the patient string
            char* result = search_patient(rec_id, recordhash, hash_number, BUCKETSIZE);
            //Write the message to the aggregator
            write_message(new_socket_worker, result, buffersize);

            free(result);//Free the allocate dmemory from the function(chack functions.c)


        }else if(!strcmp(tok, "/numPatientAdmissions")){

            int count=0;//To help us store the arguments

            char* diseasename;
            char* date1;
            char* date2;
            char* countryn;

            while(tok != NULL){

                if(count==1){
                    diseasename = strdup(tok);//get the disease name
                }else if(count==2){
                    date1 = strdup(tok);
                }else if(count==3){
                    date2 = strdup(tok);
                }else if(count==4){
                    countryn = strdup(tok);
                }

                count++;
                tok = strtok(NULL, " ");//Get the next token
            }

            if(count==2){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(diseasename);
                free(msg);
            }else if(count==3){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(diseasename);
                free(date1);
                free(msg);
            }else if(count==1){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(msg);
            }else if(count>5){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(diseasename);
                free(date1);
                free(msg);
                free(date2);
            }else if(count==4){//No country was given
                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(date1)==0 || valid_date(date2)==0 ){//Check if the dates given are valid
                    if(workernumber==0) printf("\nPlease type valid dates!\n");

                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(diseasename);
                    free(msg);
                    continue;
                }



                setDate(tempdate1, date1);
                setDate(tempdate2, date2);

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        if(workernumber==0) printf("\nThe dates you typed are the same!\n");
                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(diseasename);
                        free(msg);
                        continue;
                    }else{//If date1>date2
                        if(workernumber==0) printf("\n%s is before %s\n", date2, date1);
                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(diseasename);
                        free(msg);
                        continue;
                    }
                }



                //Everyting is ok with the dates let's move
                Bucket_ptr temp;
                char* final_result;
                int first=0;//First string of the final result flag


                int num_of_krousmata;

                for(i=0 ; i<COUNTRYHASHENTRIES ; i++){
                    temp = countryhash[i];



                    int k=0;
                    while(temp!=NULL){
                        for(k=0 ; k<(BUCKETSIZE/(sizeof(Bucket_node))) ; k++){

                            if( temp->bucket_node_array[k] != NULL){

                                num_of_krousmata = count_rbtree_nodes_with_dates_and_disease(temp->bucket_node_array[k]->rbtree, tempdate1, tempdate2, diseasename);


                                int digit_num=0;
                                int temp_int=num_of_krousmata;

                                //Count the num_of_krousmata digits so we allocate the memory
                                while (temp_int != 0) {
                                    temp_int /= 10;
                                    ++digit_num;
                                }

                                //Create the result string for this country
                                char* this_result;
                                this_result = (char*)malloc( (strlen(temp->bucket_node_array[k]->name)+1+digit_num+1)*sizeof(char) );

                                sprintf(this_result, "%s %d", temp->bucket_node_array[k]->name, num_of_krousmata);


                                if(first==0){//First string of the final result
                                    final_result = strdup(this_result);
                                    first=1;
                                    free(this_result);
                                }else{
                                    char* temps;
                                    temps = strdup(final_result);//Keep the final_result

                                    //Construct the new one
                                    free(final_result);

                                    final_result = (char*)malloc( (strlen(temps)+1+strlen(this_result)+1)*sizeof(char) );

                                    sprintf(final_result, "%s\n%s", temps, this_result);

                                    free(this_result);
                                    free(temps);
                                }


                            }
                        }


                        temp = temp->next;
                    }

                }

                write_message(new_socket_worker, final_result, buffersize);

                free(final_result);
                free(tempdate1);
                free(tempdate2);
                free(date1);
                free(date2);
                free(diseasename);


            }else if(count==5){//Country argument was given
                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(date1)==0 || valid_date(date2)==0 ){//Check if the dates given are valid
                    if(workernumber==0) printf("\nPlease type valid dates!\n");

                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(countryn);
                    free(diseasename);
                    free(msg);
                    continue;
                }



                setDate(tempdate1, date1);
                setDate(tempdate2, date2);

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        if(workernumber==0) printf("\nThe dates you typed are the same!\n");
                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(diseasename);
                        free(countryn);
                        free(msg);
                        continue;
                    }else{//If date1>date2
                        if(workernumber==0) printf("\n%s is before %s\n", date2, date1);
                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(diseasename);
                        free(countryn);
                        free(msg);
                        continue;
                    }
                }

                int country_hash_position=0;

                country_hash_position = hash(countryn)%COUNTRYHASHENTRIES;

                //Everyting is ok with the dates let's move
                //An o worker autos den exei asxolithei me to specific country,apla stelnei sto pipe 0
                if( (check_if_bucketnode_with_this_name_exists( countryn, countryhash, country_hash_position, BUCKETSIZE)==0) ){//Check if this country exists

                    char krousmata[3] = "N0";

                    write_message(new_socket_worker, krousmata, buffersize);

                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(diseasename);
                    free(countryn);
                    free(msg);

                    //printf("\nNo such country was found!\n");
                    continue;
                }

                //I have this country

                Bucket_node_ptr temp;

                temp = find_bucketnode(countryn, countryhash, country_hash_position, BUCKETSIZE);

                int num_of_krousmata;
                num_of_krousmata = count_rbtree_nodes_with_dates_and_disease(temp->rbtree, tempdate1, tempdate2, diseasename);

                int digit_num=0;
                int temp_int=num_of_krousmata;

                //Count the num_of_krousmata digits so we allocate the memory
                while (temp_int != 0) {
                    temp_int /= 10;
                    ++digit_num;
                }

                //Create the result string for this country
                char* result;
                result = (char*)malloc( (strlen(countryn)+1+digit_num+1)*sizeof(char) );

                sprintf(result, "%s %d", countryn, num_of_krousmata);

                write_message(new_socket_worker, result, buffersize);

                free(result);

            }

        }else if(!strcmp(tok, "/numPatientDischarges")){

            int count=0;//To help us store the arguments

            char* diseasename;
            char* date1;
            char* date2;
            char* countryn;

            while(tok != NULL){

                if(count==1){
                    diseasename = strdup(tok);//get the disease name
                }else if(count==2){
                    date1 = strdup(tok);
                }else if(count==3){
                    date2 = strdup(tok);
                }else if(count==4){
                    countryn = strdup(tok);
                }

                count++;
                tok = strtok(NULL, " ");//Get the next token
            }

            if(count==2){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(diseasename);
                free(msg);
            }else if(count==3){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(diseasename);
                free(date1);
                free(msg);
            }else if(count==1){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(msg);
            }else if(count>5){
                if(workernumber==0) printf("\nPlease check your query arguments.\n");
                free(diseasename);
                free(date1);
                free(msg);
                free(date2);
            }else if(count==4){//No country was given
                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(date1)==0 || valid_date(date2)==0 ){//Check if the dates given are valid
                    if(workernumber==0) printf("\nPlease type valid dates!\n");

                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(diseasename);
                    free(msg);
                    continue;
                }



                setDate(tempdate1, date1);
                setDate(tempdate2, date2);

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        if(workernumber==0) printf("\nThe dates you typed are the same!\n");
                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(diseasename);
                        free(msg);
                        continue;
                    }else{//If date1>date2
                        if(workernumber==0) printf("\n%s is before %s\n", date2, date1);
                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(diseasename);
                        free(msg);
                        continue;
                    }
                }



                //Everyting is ok with the dates let's move
                Bucket_ptr temp;
                char* final_result;
                int first=0;//First string of the final result flag


                int num_of_krousmata;

                for(i=0 ; i<COUNTRYHASHENTRIES ; i++){
                    temp = countryhash[i];



                    int k=0;
                    while(temp!=NULL){
                        for(k=0 ; k<(BUCKETSIZE/(sizeof(Bucket_node))) ; k++){

                            if( temp->bucket_node_array[k] != NULL){

                                num_of_krousmata = count_rbtree_nodes_with_exitdates_and_disease(temp->bucket_node_array[k]->rbtree, tempdate1, tempdate2, diseasename);
                                printf("%d\n", num_of_krousmata);

                                int digit_num=0;
                                int temp_int=num_of_krousmata;

                                //Count the num_of_krousmata digits so we allocate the memory
                                while (temp_int != 0) {
                                    temp_int /= 10;
                                    ++digit_num;
                                }

                                //Create the result string for this country
                                char* this_result;
                                this_result = (char*)malloc( (strlen(temp->bucket_node_array[k]->name)+1+digit_num+1)*sizeof(char) );

                                sprintf(this_result, "%s %d", temp->bucket_node_array[k]->name, num_of_krousmata);


                                if(first==0){//First string of the final result
                                    final_result = strdup(this_result);
                                    first=1;
                                    free(this_result);
                                }else{
                                    char* temps;
                                    temps = strdup(final_result);//Keep the final_result

                                    //Construct the new one
                                    free(final_result);

                                    final_result = (char*)malloc( (strlen(temps)+1+strlen(this_result)+1)*sizeof(char) );

                                    sprintf(final_result, "%s\n%s", temps, this_result);

                                    free(this_result);
                                    free(temps);
                                }


                            }
                        }


                        temp = temp->next;
                    }

                }

                write_message(new_socket_worker, final_result, buffersize);

                free(final_result);
                free(tempdate1);
                free(tempdate2);
                free(date1);
                free(date2);
                free(diseasename);


            }else if(count==5){//Country argument was given
                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(date1)==0 || valid_date(date2)==0 ){//Check if the dates given are valid
                    if(workernumber==0) printf("\nPlease type valid dates!\n");

                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(countryn);
                    free(diseasename);
                    free(msg);
                    continue;
                }



                setDate(tempdate1, date1);
                setDate(tempdate2, date2);

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        if(workernumber==0) printf("\nThe dates you typed are the same!\n");
                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(diseasename);
                        free(countryn);
                        free(msg);
                        continue;
                    }else{//If date1>date2
                        if(workernumber==0) printf("\n%s is before %s\n", date2, date1);
                        free(tempdate1);
                        free(tempdate2);
                        free(date1);
                        free(date2);
                        free(diseasename);
                        free(countryn);
                        free(msg);
                        continue;
                    }
                }

                int country_hash_position=0;

                country_hash_position = hash(countryn)%COUNTRYHASHENTRIES;

                //Everyting is ok with the dates let's move
                //An o worker autos den exei asxolithei me to specific country,apla stelnei sto pipe 0
                if( (check_if_bucketnode_with_this_name_exists( countryn, countryhash, country_hash_position, BUCKETSIZE)==0) ){//Check if this country exists
                    printf("DEN EXW \n" );
                    char krousmata[3] = "NO";

                    write_message(new_socket_worker, krousmata, buffersize);
                    printf("DEN EXW EIPA\n" );


                    free(tempdate1);
                    free(tempdate2);
                    free(date1);
                    free(date2);
                    free(diseasename);
                    free(countryn);
                    free(msg);

                    //printf("\nNo such country was found!\n");
                    continue;
                }

                //I have this country

                Bucket_node_ptr temp;

                temp = find_bucketnode(countryn, countryhash, country_hash_position, BUCKETSIZE);

                int num_of_krousmata;
                num_of_krousmata = count_rbtree_nodes_with_exitdates_and_disease(temp->rbtree, tempdate1, tempdate2, diseasename);
                printf("num of krousmatikh %d \n", num_of_krousmata );

                int digit_num=0;
                int temp_int=num_of_krousmata;

                //Count the num_of_krousmata digits so we allocate the memory
                while (temp_int != 0) {
                    temp_int /= 10;
                    ++digit_num;
                }

                //Create the result string for this country
                char* result;
                result = (char*)malloc( (strlen(countryn)+1+digit_num+1)*sizeof(char) );

                sprintf(result, "%s %d", countryn, num_of_krousmata);
                printf("num of krousmatikh %s \n", result );

                write_message(new_socket_worker, result, buffersize);
                printf("num of krousmatikh EIPA %d \n", num_of_krousmata );

                free(result);

            }

        }else if(!strcmp(tok, "/topk-AgeRanges")){
            int count=0;//To help us store the arguments

            int k=0;
            char* diseasename;
            char* date1;
            char* date2;
            char* countryn;

            while(tok != NULL){

                if(count==1){
                    k = atoi(tok);//get the k number
                }else if(count==2){
                    countryn = strdup(tok);
                }else if(count==3){
                    diseasename = strdup(tok);
                }else if(count==4){
                    date1 = strdup(tok);
                }else if(count==5){
                    date2 = strdup(tok);
                }

                count++;
                tok = strtok(NULL, " ");//Get the next token
            }


            int disease_hash_position=0;
            int country_hash_position=0;

            if(count!=6){//Wrong number of arguments :(

                if(workernumber==0) printf("\nPlease check your query arguments.\n");//only one worker should print this
                if(count>6){
                    free(diseasename);
                    free(countryn);
                    free(msg);
                    free(date1);
                    free(date2);
                    continue;
                }else if(count<6){
                    if(count==1 || count==2){
                        free(msg);
                        continue;
                    }else if(count==3){
                        free(countryn);
                        free(msg);
                        continue;
                    }else if(count==4){
                        free(countryn);
                        free(msg);
                        free(diseasename);
                        continue;
                    }else if(count==5){
                        free(countryn);
                        free(msg);
                        free(diseasename);
                        free(date1);
                        continue;
                    }
                }

            }else{//The number of arguments is correct
                disease_hash_position = hash(diseasename)%DISEASEHASHENTRIES;
                country_hash_position = hash(countryn)%COUNTRYHASHENTRIES;


                //An o worker autos den exei asxolithei me to specific country,apla stelnei sto pipe 0
                if( (check_if_bucketnode_with_this_name_exists( countryn, countryhash, country_hash_position, BUCKETSIZE)==0) ){//Check if this country exists

                    char krousmata[3] = "NO";

                    write_message(new_socket_worker, krousmata, buffersize);

                    free(diseasename);
                    free(countryn);
                    free(msg);
                    free(date1);
                    free(date2);
                    continue;
                }


                //An o worker autos den exei asxolithei me to specific virus,apla stelnei sto pipe 0
                if( (check_if_bucketnode_with_this_name_exists(diseasename, diseasehash, disease_hash_position, BUCKETSIZE)==0) ){//Check if this virus exists

                    char krousmata[3] = "NO";

                    write_message(new_socket_worker, krousmata, buffersize);

                    free(diseasename);
                    free(countryn);
                    free(msg);
                    free(date1);
                    free(date2);
                    continue;
                }


                //Check if the dates are valid and Convert date strings to actual dates
                ptrDate tempdate1 = (ptrDate)malloc(sizeof(myDate));
                ptrDate tempdate2 = (ptrDate)malloc(sizeof(myDate));

                if( valid_date(date1)==0 || valid_date(date2)==0 ){//Check if the dates given are valid
                    if(workernumber==0) printf("\nPlease type valid dates!\n");

                    free(tempdate1);
                    free(tempdate2);
                    free(diseasename);
                    free(countryn);
                    free(msg);
                    free(date1);
                    free(date2);
                    continue;
                }



                setDate(tempdate1, date1);
                setDate(tempdate2, date2);

                if( (compare_dates(tempdate1, tempdate2)==1) || (compare_dates(tempdate1, tempdate2)==0) ){//If date1>date2 OR date1==date2
                    if((compare_dates(tempdate1, tempdate2)==0)){//If date1==date2
                        if(workernumber==0) printf("\nThe dates you typed are the same!\n");
                        free(tempdate1);
                        free(tempdate2);
                        free(diseasename);
                        free(countryn);
                        free(msg);
                        free(date1);
                        free(date2);
                        continue;
                    }else{//If date1>date2
                        if(workernumber==0) printf("\n%s is before %s\n", date2, date1);
                        free(tempdate1);
                        free(tempdate2);
                        free(diseasename);
                        free(countryn);
                        free(msg);
                        free(date1);
                        free(date2);
                        continue;
                    }
                }

                //Find the country in the disease
                Bucket_node_ptr temp;

                temp = find_bucketnode(countryn, countryhash, country_hash_position, BUCKETSIZE);

                int ageranges_cases[4];

                ageranges_cases[0] = agegroup_count_country_disease_and_date(temp, BUCKETSIZE, diseasename, 0, 20, tempdate1, tempdate2);
                ageranges_cases[1] = agegroup_count_country_disease_and_date(temp, BUCKETSIZE, diseasename, 21, 40, tempdate1, tempdate2);
                ageranges_cases[2] = agegroup_count_country_disease_and_date(temp, BUCKETSIZE, diseasename, 41, 60, tempdate1, tempdate2);
                ageranges_cases[3] = agegroup_count_country_disease_and_date(temp, BUCKETSIZE, diseasename, 61, 500, tempdate1, tempdate2);

                int total_krousmata = ageranges_cases[0]+ageranges_cases[1]+ageranges_cases[2]+ageranges_cases[3];

                if( total_krousmata ==0 ){//If there are no incidents

                    char krousmata[3] = "NO";

                    write_message(new_socket_worker, krousmata, buffersize);

                    free(diseasename);
                    free(countryn);
                    free(msg);
                    free(date1);
                    free(date2);
                    continue;
                }




                char* final_result;
                int first=0;//First string of the final result flag
                int x=0;
                int a=0;
                for(x=0 ; x<k ; x++){

                    int num_of_krousmata=0;
                    int index_of_max=0;//So we know in which age range we have the max cases(0->0-20 | 1->21-40 | 2->41-60 | 3->)

                    for(a=0 ; a<4 ; a++){//find the max
                        if(a==0){
                            num_of_krousmata = ageranges_cases[a];
                            index_of_max = 0;
                        }else if(num_of_krousmata<ageranges_cases[a]){
                            num_of_krousmata = ageranges_cases[a];

                            index_of_max = a;
                        }
                    }


                    int digit_num=0;
                    int temp_int=num_of_krousmata;

                    //Count the num_of_krousmata digits so we allocate the memory
                    while (temp_int != 0) {
                        temp_int /= 10;
                        ++digit_num;
                    }

                    //Create the result string
                    char* this_result;
                    this_result = (char*)malloc( (5/*age range*/+1/*:*/+2/*double space*/+digit_num+1/*%*/+1)*sizeof(char) );

                    double percentage = ((double)num_of_krousmata/(double)total_krousmata)*100;

                    if(a==0) sprintf(this_result, "0-20:  %lf%c", percentage, '%');
                    if(a==1) sprintf(this_result, "21-40:  %lf%c", percentage, '%');
                    if(a==2) sprintf(this_result, "41-60:  %lf%c", percentage, '%');
                    if(a==3) sprintf(this_result, "61+:  %lf%c", percentage, '%');

                    if(first==0){//First string of the final result
                        final_result = strdup(this_result);
                        first=1;
                        free(this_result);
                    }else{
                        char* temps;
                        temps = strdup(final_result);//Keep the final_result

                        //Construct the new one
                        free(final_result);

                        final_result = (char*)malloc( (strlen(temps)+1+strlen(this_result)+1)*sizeof(char) );

                        sprintf(final_result, "%s\n%s", temps, this_result);

                        free(this_result);
                        free(temps);
                    }

                    //Don't use the max for the next one
                    ageranges_cases[a] = -1;


                }


                write_message(new_socket_worker, final_result, buffersize);

                free(final_result);
                free(tempdate1);
                free(tempdate2);
                free(diseasename);
                free(countryn);
                free(date1);
                free(date2);


            }



        }


    }



//Free the allocated memory
    for(i=0 ; i<countrycounter ; i++){
        free(countryarray[i]);
    }
    free(countryarray);
    free(summary_stats);
    free(final_server_message);
    free(pipe_message);
    free(tempcountrysting);
    free(tempserverinfo);

    for(i=0 ; i<RECORDHASHENTRIES ; i++){//Destroy recordID buckets
        destroy_bucket(recordhash[i], BUCKETSIZE);
    }

    free(recordhash);

    for(i=0 ; i<DISEASEHASHENTRIES ; i++){
        destroy_bucket(diseasehash[i], BUCKETSIZE);
    }

    free(diseasehash);

    for(i=0 ; i<COUNTRYHASHENTRIES ; i++){
        destroy_bucket(countryhash[i], BUCKETSIZE);
    }

    free(countryhash);



    free(serverPort);
    free(serverIP);
    close(workerfd);
    destroy_patient_list(root);

}
